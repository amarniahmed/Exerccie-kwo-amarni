<?php

/**
Verif des : Letter, Snippet, String
**/

class Repository
{
  private $debug = true;
  private $server = null;
  private $snapshot = null;

  const SNAPSHOT = 'var/repository/snapshot.inc';
  const MANIFEST = 'manifest';
  
  const PATH = 'var/repository';
  const BACKUP_PATH = 'var/repository/backup';
  const DIFF_PATH = 'var/repository/diff';
  
  const FILE_STATUS_NONEXISTENT = 1;
  const FILE_STATUS_OUTDATED = 2;
  
  const SERVER_ENDPOINT = '/repository.server';
  
  public function __construct($server) {
    $this->server = $server;
    $this->server = 'http://www.test.kwo';
  }
  
  public function getHttpResponse($command, $body=null) {
    $uri = $this->server . self::SERVER_ENDPOINT;
    $req = HttpRequest::getInstance($uri);
    $req->setHeaders(array('Kwo-Repo-Command' => $command, 
                           'Kwo-Repo-Token' => 'toto'));
    if (is_array($body)) { 
      $req->setParams($body);
    }
    elseif ($body) {
      $req->setBody($body);
    }
    return $req->send();
  }
  
  public function isValidToken($token, $command) {
    return $token == 'toto';
  }
  
  public function getLocalSnapshot() {
    $snapshot = $this->snapshot();
    return $snapshot;
  }
  
  public function getRemoteSnapshot() {
    $snapshot = $this->getHttpResponse('snapshot')->getContent(); 
    $snapshot = gzuncompress($snapshot); 
    $snapshot = unserialize($snapshot); 
    if ($this->debug === true) {
      unset($snapshot['files']['lib/'.DEVEL.'/class/Marchand.class.inc']);
      $snapshot['files']['lib/'.DEVEL.'/class/Marchandise.class.inc']['md5'] = 'md5toto';
    }
    if (!is_array($snapshot) || empty($snapshot)) {
      throw new Exception('invalid remote snapshot ['.$this->server.']');
    }
    return $snapshot;
  }

  /**
     TODO : en fonction de certains pathes, faire un build (etc/inc/)
   **/
  private function extract($pack) {
    if (!is_dir(self::BACKUP_PATH)) {
      Folder::create(self::BACKUP_PATH);
    }
    $zip = new ZipArchive();
    if (!$zip->open($pack)) {
      throw new Exception('invalid pack ['.$pack.']');
    }
    $errors = array();
    $manifest = $zip->getFromIndex(0);
    $manifest = @unserialize($manifest);
    if (!is_array($manifest)) {
      $errors['manifest : invalid'];
    }
    
    for ($i = 0; $i < $zip->numFiles; $i++) {
      $entry = $zip->getNameIndex($i);
      if ($entry === self::MANIFEST) continue ;
      $file = str_replace('|', '/', $entry);
      if (file_exists($file) && !is_writable($file)) {
        $errors[] = $file.' : not writable';
      }
      $dir = dirname($file);
      if (!is_dir($dir)) {
        if (!Folder::create($dir, true)) {
          $errors[] = $dir.' : folder creation failure';
        }
        S('log')->trace('creating ['.$dir.']', __METHOD__);
      }
    }
    if (count($errors) > 0) {
      S('log')->debug($errors, __METHOD__);
      throw new Exception('errors');
    }
    $t = date('Y-m-d-H-i-s', time());
    for ($i = 0; $i < $zip->numFiles; $i++) {
      $entry = $zip->getNameIndex($i);
      if ($entry == self::MANIFEST) continue ;
      $file = str_replace('|', '/', $entry);
      if (file_exists($file)) {
        $dest = self::BACKUP_PATH . '/' . $t . '-' . $entry;
        copy($file, $dest);
      }
      if (false) {
        rename(self::PATH . '/' . $entry, $file);
        if (IS_CLI) {
          Shell::chown($file);
        }
        if ($manifest['files'][$file]['mtime'] > 1000000000) {
          touch($file, $manifest['files'][$file]['mtime']);
        }
      }
    }
    $zip->close();
    return $manifest;
  }

  public function sync($opts=null) {
    $opts = (array) $opts;

    if (is_array($opts['push']) || is_array($opts['pull'])) {
      $push = (array) $opts['push'];
      $pull = (array) $opts['pull'];
    }
    else {
      $diff = $this->compare();
      $push = $pull = array();
      foreach ($diff['files'] as $file => $h) {
        if ($h['remote']['status']) {
          $push[] = $file;
        }
        else {
          $pull[] = $file;
        }
      }
    }

    if (count($push) > 0) {
      $body = $this->createPack($push);
      $this->getHttpResponse('push', $body);
    }

    if (count($pull) > 0) {
      $body = serialize($pull);
      $stream = $this->getHttpResponse('pull', $body)->getStream();
//      S('log')->debug($stream);
//      S('log')->debug(stream_get_contents($stream));
      $this->receive($stream);
    }

  }

  public function createPack($files, $opts=null) {
    $opts = (array) $opts;
    $struct = array('metas' => array('sent_at' => DH::now(),
                                     'comment' => $opts['comment']),
                    'files' => array());

    $manifest = self::PATH . '/' . self::MANIFEST;
    $entries = array($manifest);

    foreach ($files as $file) {
      $stat = stat($file);
      $struct['files'][$file] = array('file' => $file,
                                      'mtime' => $stat['mtime'],
                                      'ctime' => $stat['ctime']);
      $dest = self::PATH . '/' . str_replace('/', '|', $file);
      $entries[] = $dest;
      copy($file, $dest);
    }

    File::put($manifest, $struct);

    $time = date('Y-m-d-H-i-s', time());
    $pack = self::PATH . '/' . 'push-' . $time . '.zip';

    $zip = new ZipArchive();
    $zip->open($pack, ZipArchive::CREATE);
    foreach ($entries as $file) {
      $zip->addFile($file, basename($file));
    }
    $zip->close();

    foreach ($entries as $file) {
      unlink($file);
    }
    Shell::chown($pack);

    return File::getInstance($pack);

  }

  public function compare() {
    $changes = array('files' => array(),
                     'tables' => array(),
                     'properties' => array());
    $lsnapshot = $this->getLocalSnapshot();
    unset($lsnapshot['files']['lib/'.DEVEL.'/class/Toto.class.inc']);
    $rsnapshot = $this->getRemoteSnapshot(); 
    foreach ($lsnapshot['files'] as $file => $h) {
      if (!isset($rsnapshot['files'][$file])) { 
        $changes['files'][$file] = array('local' => $h,
                                         'remote' => array('status' => self::FILE_STATUS_NONEXISTENT));
        continue ;
      }
      if ($h['md5'] != $rsnapshot['files'][$file]['md5']) {
        $changes['files'][$file] = array('local' => $h,
                                         'remote' => $rsnapshot['files'][$file]);
        $key = ($h['mtime'] > $rsnapshot['files'][$file]['mtime']) ? 'remote' : 'local';
        $changes['files'][$file][$key]['status'] = self::FILE_STATUS_OUTDATED;
      }
    }
    foreach ($rsnapshot['files'] as $file => $h) {
      if (!isset($lsnapshot['files'][$file])) {
        $changes['files'][$file] = array('local' => array('status' => self::FILE_STATUS_NONEXISTENT),
                                         'remote' => $h);
      }
    }
    return $changes;
  }

  public function snapshot() {
    $this->snapshot = array();
    if ($this->debug === true && file_exists(self::SNAPSHOT)) {
      include self::SNAPSHOT;
      return $snapshot;
    }
    $beg = time();
    $this->snapshot = array('metas' => array('size' => 0,
                                             'files' => 0,
                                             'tables' => 0,
                                             'properties' => 0,
                                             'last_update' => null,
                                             'updated_at' => null,
                                             'built_at' => DH::now(),
                                             'build_time' => DH::now()),
                            'files' => array(),
                            'tables' => array(),
                            'properties' => array());
    $this->createFileSystemSnapshot('.');
    $this->snapshot['metas']['updated_at'] = date(DH::DATE_MYSQL,
                                                  $this->snapshot['metas']['updated_at']);
    $this->createDataBaseSnapshot();
    $this->createPropertiesSnapshot();
    $this->snapshot['metas']['built_at'] = DH::now();
    $this->snapshot['metas']['build_time'] = time() - $beg;
    $ret  = '<?php'."\n\n";
    $ret .= '$snapshot = '.var_export($this->snapshot, true).';'."\n";
    if (!is_dir(self::PATH)) {
      Folder::create(self::PATH);
    }
    File::put(self::SNAPSHOT, $ret);
    return $this->snapshot;
  }
  
  public function createFileSystemSnapshot($dir) {
    if ($handle = opendir($dir)) {
      while ($file = readdir($handle)) {
        if ($file[0] === '.') continue ;
        if (is_dir($dir.'/'.$file)) {
          if ($file === 'infos') continue ;
          $child = $dir.'/'.$file;
          if ($child === './etc/app' || $child === './etc/inc') continue ;
          if ($dir === '.' && !in_array($file, array(BIN_PATH, ETC_PATH, LIB_PATH, WEB_PATH))) continue ;
          $this->createFileSystemSnapshot($child);
        }
        else {
          if ($dir == './etc' && $file === 'platform.conf.inc') continue ;
          if (strpos($dir, '/lib/'.DEVEL) === false) continue ; // test
          if (preg_match('/(\.lck|~|\.bak|\.safe|\.old|#|\.gz|\.zip)$/', $file)) continue ;
          $this->addFile($dir.'/'.$file);
        }
      }
      closedir($handle);
    }
  } 
  
  private function addFile($file) {
    $file = substr($file, 2);
    $this->snapshot['metas']['files']++;
    $h = stat($file);
    $this->snapshot['metas']['size'] += $h['size'];
    if ($h['mtime'] > $this->snapshot['metas']['updated_at']) {
      $this->snapshot['metas']['updated_at'] = $h['mtime'];
      $this->snapshot['metas']['last_update'] = $file;
    }
    $this->snapshot['files'][$file] = array('mtime' => $h['mtime'], 
                                            'size' => $h['size'],
                                            'md5' => md5_file($file));
  }

  private function createPropertiesSnapshot() {
    foreach (Property::collect() as $prop) {
      $this->snapshot['metas']['properties']++;
      $this->snapshot['properties'][] = array('model' => Model::name($prop->model_id),
                                              'model_id' => $prop->model_id,
                                              'name' => $prop->name);
    }
  }

  /** Attention à la sync de etc/inc si property_id est différent **/
  /** TODO : indexes **/
  private function createDataBaseSnapshot() {
    $dbo = S('dbo');
    foreach ($dbo as $table) {
      $this->snapshot['metas']['tables']++;
      $name = $table->getName();
      $this->snapshot['tables'][$name] = array('fields' => array(),
                                               'indexes' => array());
      foreach ($table->getColumns() as $column) {
        $field = $column['Field'];
        $type = $column['Type'];
        $this->snapshot['tables'][$name]['fields'][$field] = $type;
      }
    }
  }

  public function diff($filename) {
    $path = self::DIFF_PATH;
    if (!is_dir($path)) {
      Folder::create($path);
    }
    $params = array('file' => $filename);
    $content = $this->getHttpResponse('file', $params)->getContent(); 
    $content = gzuncompress($content); 
    $from = $filename;
    $to = $path . '/' . 'diff-' . str_replace('/', '-', $filename);
    File::put($to, $content);
    $cmd = array('diff', 
                 '-U 5', 
                 '--ignore-all-space',
                 '--ignore-blank-lines',
                 $from, $to);
    $h = Shell::exec(join(' ',$cmd));
    return $h['lines'];
  }

  public function receive($stream) {
    $time = date('Y-m-d-H-i-s', time());
    $pack = self::PATH . '/' . 'pull-' . $time . '.zip';
    if (!is_resource($stream)) {
      throw new Exception('receive: invalid input');
    }
    $out = @fopen($pack, 'w');
    if (!is_resource($out)) {
      throw new Exception('receive: invalid output');
    }
    while (!feof($stream)) { 
      $chunk = fread($stream, 8192);
//      error_log($pack.' : '.strlen($chunk));
      fwrite($out, $chunk);
    }
    fclose($out);
    fclose($stream); 
    $this->extract($pack);
  }
  
  public static function clean() {
    if (strlen(self::PATH) < 9 || !is_dir(self::PATH)) return ;
    $paths = array(self::BACKUP_PATH , self::DIFF_PATH , self::PATH);
    foreach ($paths as $path) {
      $files = glob($path.'/*.*');
      $files = $files ?: array();
      foreach ($files as $file) {
        if (!file_exists($file)) continue ;
        unlink($file);
      }
    }
  }  

  private static $singleton = null;
  
  public static function getInstance($server=null) {
    if (!self::$singleton) {
      self::$singleton = new Repository($server);
    } 
    return self::$singleton;
  }
  
}
