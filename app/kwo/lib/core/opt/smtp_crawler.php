<?php

//" */15 * * * * root /usr/local/bin/php /etc/kernix/bin/smtp_crawler.php "
 
error_reporting(E_ERROR);

define('FILE_READ_ERROR', 1);
define('DB_CREATE_ERROR', 2);
define('BLOCKER', '/tmp/smtp_crawler.lock');
define('LOG_FILE', '/var/log/smtp');

try {
  if (!file_exists(LOG_FILE)) {
    throw new Exception('Add this line in /etc/syslog.conf'."\n".
                        '---------------------------------------------------'."\n".
                        'mail.*  /var/log/smtp'."\n".
                        '---------------------------------------------------'."\n".
                        'Be Careful: use tabs (\t)');
  }
  if (file_exists(BLOCKER)) {
    throw new Exception(BLOCKER.' exists');
  }
  $crawler = new SmtpCrawler();
  touch(BLOCKER);
  $crawler->parse();
  if (file_exists(BLOCKER) && strlen(BLOCKER) >= 5) {
    unlink(BLOCKER);
  }
}
catch (Exception $e) { 
  if (file_exists(BLOCKER) && strlen(BLOCKER) >= 5) {
    unlink(BLOCKER);
  }
  print('smpt_crawler exception ['.$_SERVER['HOSTNAME'].']:'."\n".
        $e->getMessage()."\n"); 
}

class SmtpCrawler {
  
  private $dbo;
  private $log_path = '/var/tmp/smtp';
  private $sender = '@kwm.fr';
  private $months = array('Jan' => '01', 'Feb' => '02', 'Mar' => '03', 'Apr' => '04',
                          'May' => '05', 'Jun' => '06', 'Jul' => '07', 'Aug' => '08',
                          'Sep' => '09', 'Oct' => '10', 'Nov' => '11', 'Dec' => '12');

  private $replace = array('', '', '');

  private $from2 = array('relay=', '. [', '[', ']');
  private $to2 = array('', ' ', '', '');

  private $from1 = array('to=', '<', '>');
  private $to1 = array('', '', '');
  
  public function __construct() {
    $this->timestamp = mktime();
    $this->dbo = DataBaseObject::getInstance();
  }
   
  public function parse() {
    $fd = fopen(LOG_FILE, 'r+');
    flock($fd, LOCK_EX);
    copy(LOG_FILE, $this->log_path.'.'.$this->timestamp);
    ftruncate($fd, 0);
    fclose($fd);
    $fd = @fopen($this->log_path.'.'.$this->timestamp, 'r');
    if ($fd === false) {
      throw new Exception('open failure ['.$this->log_path.'.'.$this->timestamp.']', FILE_READ_ERROR);
    }
    while ($line = fgets($fd)) {
      if (!(preg_match('/ from=/', $line) && 
            preg_match('/'.$this->sender.'/', $line)) && 
          !preg_match('/ to=/', $line)) continue;
      list ($smtp_date, $smtp_key, $smtp_data) = explode(': ', $line, 3);
      if (empty($smtp_date) || empty($smtp_key) || empty($smtp_data)) continue;
      $smtp_key = trim($smtp_key);
      $smtp_date = $this->clean(preg_split('/(\s)+/', trim($smtp_date)));
      $smtp_data = trim($smtp_data);
      if (preg_match('/ from=</', $line) && preg_match('/'.$this->sender.'/', $line)) {
        $smtp_data = str_replace(array('from=', $this->sender, '<', '>'), $this->replace, $smtp_data);
        $smtp_data = substr($smtp_data, 0, strpos($smtp_data, ','));
        list ($campaign_infos, $sender_url) = explode('+', $smtp_data);
        list ($contact_id, $campaign_id, $sending_id) = explode('.', $campaign_infos);
        $contact_id = (int) $contact_id;
        $campaign_id = (int) $campaign_id;
        $sending_id = (int) $sending_id;
        if (empty($contact_id) || empty($campaign_id)) continue;
        $this->insert($smtp_key, $contact_id, $campaign_id, $sending_id, $sender_url, $smtp_date);
      }
      elseif (preg_match('/ to=</',$line)) {
        $smtp_data = str_replace($this->from1, $this->to1, $smtp_data);
        list ($email,,,,, $hostip, $dsn, $stat) = explode(',', $smtp_data);
        if (empty($hostip) || empty($dsn) || empty($stat)) continue ;
        list ($host, $ip) = explode(' ', str_replace($this->from2, $this->to2, trim($hostip)));
        if (empty($host) || empty($ip)) continue;
        $dsn = trim(str_replace('dsn=', '', $dsn));
        if ($dsn == '2.0.1' || $dsn == '2.0.2') $dsn = '2.0.0';
        $stat = trim(str_replace('stat=', '', $stat));
        $this->update($email, $smtp_key, $smtp_date, $host, $ip, $dsn, $stat, $smtp_data);
      }
    }
    $this->dbo->exec('DELETE FROM maillog WHERE dsn="2.0.0"');
    $this->dbo->exec('DELETE FROM maillog WHERE dsn LIKE "_._._"');
    if ($fd) {
      fclose($fd);
    }
  }
  
  private function insert($smtp_key, $contact_id, $campaign_id, $sending_id, $sender_url, $smtp_date) {
    $this->dbo->query('SELECT * FROM maillog WHERE smtp_key="'.$this->dbo->protect($smtp_key).'"');
    if ($this->dbo->numRows() != 0) return;
    $row = array('smtp_key' => '"'.$this->dbo->protect($smtp_key).'"',
                 'contact_id' => $contact_id,
                 'campaign_id' => $campaign_id,
                 'sending_id' => $sending_id,
                 'sender_url' => '"'.$this->dbo->protect($sender_url).'"',
                 'from_time' => '"'.$this->dbo->protect($smtp_date).'"');
    $this->dbo->exec('INSERT INTO maillog ('.join(',', array_keys($row)).')'
                     .           ' VALUES ('.join(',', array_values($row)).')');
  }
  
  private function update($email, $smtp_key, $smtp_date, $host, $ip, $dsn, $stat, $smtp_data) {
    $this->dbo->query('SELECT * FROM maillog WHERE smtp_key="'.$this->dbo->protect($smtp_key).'"');
    if ($this->dbo->numRows() != 1) return ;
    $this->dbo->exec('UPDATE maillog SET email="'.$this->dbo->protect($email).'",'
                     .                 ' relay_host="'.$this->dbo->protect($host).'",'
                     .                 ' relay_ip="'.$this->dbo->protect($ip).'",'
                     .                 ' dsn="'.$this->dbo->protect($dsn).'",'
                     .                 ' stat="'.$this->dbo->protect($stat).'",'
                     .                 ' to_time="'.$this->dbo->protect($smtp_date).'"'
                     .           ' WHERE smtp_key="'.$this->dbo->protect($smtp_key).'"');
    $this->dbo->query('SELECT * FROM maillog WHERE smtp_key="'.$this->dbo->protect($smtp_key).'"');
    $h = $this->dbo->assoc();
    $h['proof'] = (int) !$h['sending_id'];
    $h['email_id'] = $h['contact_id'];
    $url = 'http://'.$h['sender_url'].'/push/smtp?'.http_build_query($h);
//    echo $url;
    $val = (int) @file_get_contents($url);
    if ($val != 1) {
      $this->dbo->exec('INSERT INTO error VALUES ("'.$this->dbo->protect($url).'")');
    }
  }
  
  private function clean($date) {
    $day = str_pad($date[1], 2, '00', STR_PAD_LEFT);
    $month = $this->months[$date[0]];
    $time = $date[2];
    $year = date('Y');
    if (date('m') < $day && date('d') < $month) {
      $year--;
    }
    return $year.'-'.$month.'-'.$day.' '.$time;
  }
 
  public function __destruct () {
    system('find /var/tmp/ -maxdepth 1 -type f -mtime +31 -exec rm {} \;');
  }

}

class DataBaseObject {

  private $link;
  private $rs;
  
  public function hasTable($table) {
    $this->query('SELECT * FROM sqlite_master WHERE type="table" AND name="'.$table.'"');
    return $this->numRows() == 1;
  }

  public static function getInstance() {
    $dir = '/var/lib/sqlite';
    if (!is_dir($dir)) {
      mkdir($dir, 0700, true);
    }
    $file = $dir.'/maillog.sqlite';
    if (class_exists(SQLite3)) {
      $version = 3;
    }
    elseif (function_exists('sqlite_open')) {
      $version = 1;
    }
    else {
      throw new Exception('no sqlite driver available');
    }
    $file .= $version;
    if (!file_exists($file) && file_exists(BLOCKER) && strlen(BLOCKER) >= 5) {
      unlink(BLOCKER);
    }
    if ($version == 1) {
      $dbo = new SqliteObject($file);
    }
    elseif ($version == 3) {
      $dbo = new Sqlite3Object($file);
    }
    if (!$dbo->hasTable('maillog')) {
      $sql  = 'CREATE TABLE maillog (';
      $sql .= ' smtp_key VARCHAR (16) NOT NULL PRIMARY KEY,';
      $sql .= ' email VARCHAR (255) NULL,';
      $sql .= ' contact_id INT (10) NULL,';
      $sql .= ' campaign_id SMALLINT (5) NULL,';
      $sql .= ' sending_id SMALLINT (5) NULL,';
      $sql .= ' sender_url VARCHAR (255) NULL,';
      $sql .= ' relay_host VARCHAR (255) NULL,';
      $sql .= ' relay_ip VARCHAR (16) NULL,';
      $sql .= ' stat VARCHAR (255) NULL,';
      $sql .= ' dsn VARCHAR (5) NULL,';
      $sql .= ' from_time DATETIME NULL,';
      $sql .= ' to_time DATETIME NULL)';
      $dbo->exec($sql);
      $dbo->exec('CREATE INDEX maillog_contact_id ON maillog (contact_id)');
      $dbo->exec('CREATE INDEX maillog_campaign_id ON maillog (campaign_id)');
      $dbo->exec('CREATE INDEX maillog_sending_id ON maillog (sending_id)');
      $dbo->exec('CREATE INDEX maillog_dsn ON maillog (dsn)');
      $dbo->exec('CREATE INDEX maillog_from_time ON maillog (from_time)');
      print('table creation: maillog'."\n");
    }
    if (!$dbo->hasTable('error')) {
      $dbo->exec('CREATE TABLE error ( url VARCHAR (255) NOT NULL PRIMARY KEY )');
      print('table creation: error'."\n");
    }
    
    $dbo->test();

    return $dbo;
  }

  public function test() {
    $prefix = 'xxxtest';
    for ($i = 1; $i <= 2; $i++) {
      $this->exec('INSERT INTO maillog (smtp_key) VALUES ("'.$prefix.$i.'")');
    }
    $this->query('SELECT smtp_key FROM maillog WHERE smtp_key LIKE "'.$prefix.'_"');
    if ($this->numRows() < 2) throw new Exception('dbo test failure 1');
    $h = $this->assoc(); 
//    print_r($h);
    if (!$h || substr($h['smtp_key'], 0, strlen($prefix)) !== $prefix) { 
      throw new Exception('dbo test failure 2');
    }
    $this->exec('DELETE FROM maillog WHERE smtp_key LIKE "'.$prefix.'1"');
    $this->query('SELECT smtp_key FROM maillog WHERE smtp_key LIKE "'.$prefix.'_"');
    if ($this->numRows() < 1) throw new Exception('dbo test failure 2');
    $this->exec('DELETE FROM maillog WHERE smtp_key LIKE "'.$prefix.'_"');
  }

}

class Sqlite3Object extends DataBaseObject {
    
  public function __construct($file) {
    try {
      $this->link = new SQLite3($file);
    }
    catch (Exception $e) {
      unlink($file);
      throw $e;
    }
  }
  
  public function query($sql) {
    $this->rs = $this->link->query($sql);
    if ($this->rs === false) {
//      throw new Exception($sql.' ('.$this->link->lastErrorMsg().')');
    }
  }
  
  public function exec($sql) {
    $ret = $this->link->exec($sql);
    if ($ret === false) {
//      throw new Exception($sql.' ('.$this->link->lastErrorMsg().')');
    }
    return $ret;
  }
  
  public function assoc() {
    return $this->rs->fetchArray(SQLITE3_ASSOC);
  }

  public function object() {
    $ret = $this->rs->fetchArray(SQLITE3_ASSOC);
    $ret = (object) $ret;
    return $ret;
  }
  
  public function numRows() {
    $n = 0;
    while ($this->rs->fetchArray(SQLITE3_ASSOC)) {
      $n++;
      if ($n > 3) break ;
    }
    $this->rs->reset();
    return $n;
  }
  
  public function protect($str) {
    return $this->link->escapeString($str);
  }

}

class SqliteObject extends DataBaseObject {
  
  public function __construct($file) {
    try {
      $this->link = sqlite_open($file, 0660, $error);
    }
    catch (Exception $e) {
      unlink($file);
      throw $e;
    }
  }
    
  public function query($sql) {
    $this->rs = sqlite_query($this->link, $sql, SQLITE_ASSOC, $error);
  }
  
  public function exec($sql) {
    $this->rs = sqlite_exec($this->link, $sql, $error);
  }
  
  public function assoc() {
    return sqlite_fetch_array($this->rs, SQLITE_ASSOC);
  }

  public function object() {
    return sqlite_fetch_object($this->rs);
  }
  
  public function numRows() {
    return sqlite_num_rows($this->rs);
  }
  
  public function protect($str) {
    return sqlite_escape_string($str);
  }
  
}

