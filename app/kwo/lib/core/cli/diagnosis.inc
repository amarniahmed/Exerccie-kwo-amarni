<?php

/***

Todo : vérfier si le cron est paramétré dans le cron.hourly

si push ... vérifier smtpcrawler

***/

class Diagnosis {

  public static $warnings = array();

  public static function smtp_connection() {
    if (P('mail.smtp.default')) {
      if (!IH::isPortOpened(P('mail.smtp.default'), 25)) {
        throw new Exception('can not open port 25 on '.P('mail.smtp.default'));
      }
    }
    return P('mail.smtp.default');
  }

  public static function smtp_log() {
    $dbo = S('dbo');
    $n = $dbo->fetchValue('SELECT COUNT(*) FROM '.T('campaign')
                          .' WHERE status>='.Campaign::STATUS_SCHEDULED);
    if ($n > 0 && !file_exists('/var/log/smtp')) {
      throw new Exception('smtp_crawler.php and syslog should be configured');
    }
  }

  public static function dbms_connection() {
    S('dbo')->connect();
    if (!S('dbo')->isConnected()) {
      throw new Exception();
    }
  }

  public static function dbms_configuration() {
    if (!S('dbo')->isConnected()) {
      throw new Exception();
    }
    $errors = array();
    $h = S('dbo')->asHash('SHOW VARIABLES'); 
    if ($h['max_connections'] <= 100) {
      $errors[] = 'max_connections ('.$h['max_connections'].') too small';
    }
    if ($h['wait_timeout'] > 3600) {
      $errors[] = 'wait_timeout ('.$h['wait_timeout'].') too high';
    }
    if ($h['auto_increment_increment'] != 1) {
      $errors[] = 'auto_increment_increment ('.$h['auto_increment_increment'].') invalid';
    }
    if ($h['skip_networking'] != 'ON') {
      self::$warnings[] = 'skip_networking should be ON';
    }
    if ($h['sql_mode'] != '') {
      $errors[] = 'sql_mode should be empty (also check /usr/my.cnf)';
    }
    if (count($errors) >= 1) {
      throw new Exception('invalid params ['.join(', ', $errors).']');
    }
  }

  public static function php_extensions() {
    $required = array('calendar', 'ctype', 'curl', 'date', 'dom', 'gd', 'hash',
                      //'iconv', 
                      'json', 'mbstring', 'mysqli', 'mysqlnd',
                      'openssl',
                      'pcre', 'posix', 'session', 'simplexml', 'sqlite3', 
                      'tidy', 'zip', 'zlib');
    $extensions = get_loaded_extensions();
    foreach ($extensions as &$extension) {
      $extension = strtolower($extension);
    }
    $diff = array_diff($required, $extensions);
    if (count($diff) >= 1) {
      throw new Exception('['.join(', ', $diff).'] not found');
    }
  }

   public static function php_web_configuration() {
     $params = array('secret' => SH::crypt(date('Y-m-d')));
     $opts =  array('debug' => false);
     $h = HttpClient::get('/diagnosis', $params, $opts); 
     if (empty($h)) {
       throw new Exception('invalid input from /diagnosis');
     }
     $uh = System::getApacheUser();
     $uid = $uh['uid'];
     $vars = array('server_user_id' => $uh['uid'],
                   'php_display_errors' => 0,
                   'php_register_globals' => 0,
                   'mbstring.detect_order' => 'UTF-8',
                   'mbstring.func_overload' => 7,
                   'default_charset' => 'UTF-8',
                   'php_safe_mode' => 0,
                   'php_session_auto_start' => 0,
                   'php_magic_quotes_gpc' => 0,
                   'php_magic_quotes_runtime' => 0);
     $h = unserialize($h);
     $errors = array();
     foreach ($vars as $key => $value) {
       if ($h[$key] == $value) continue ;
       $errors[] = $key.' ('.$value.')';
     }
     if (!in_array('mod_rewrite', $h['server_modules'])) {
       $errors[] = 'apache mod_rewrite required';
     }
     if (count($errors) >= 1) {
       throw new Exception('invalid server vars ['.join(', ', $errors).']');
     }
   }

  public static function php_cli_configuration() {
    $errors = array();
    if (ini_get('mbstring.func_overload') != 7) {
      $errors[] = 'mbstring.func_overload';
    }
    if (version_compare(PHP_VERSION, '5.3.0') <= 0) {
      $errors[] = 'invalid php version ' . PHP_VERSION;
    }
    if (count($errors) >= 1) {
      throw new Exception('invalid params ['.join(', ', $errors).']');
    }
  }

  public static function fs_owner() {
    $uh = System::getApacheUser();
    $uid = $uh['uid'];
    $paths = array(DOC_PATH, ETC_PATH, INC_PATH, TMP_PATH, USR_PATH, VAR_PATH,
                   Logger::PATH, Scheduler::PATH, 
                   Session::PATH, Task::PATH);
    $errors = array();
    foreach ($paths as $path) {
      if (!file_exists($path)) {
        $errors[] = $path;
        continue ;
      }
      $h = stat($path);
      if ($h['uid'] == $uid) continue ;
      $errors[] = $path;
    }
    if (count($errors) >= 1) {
      throw new Exception('invalid owner ['.join(', ', $errors).']');
    }
  }

}

try {

  $tests = array('bdd : connexion' => 'dbms_connection',
                 'bdd : configuration' => 'dbms_configuration',
                 'smtp : relay' => 'smtp_connection',
                 'smtp : log' => 'smtp_log',
                 'php : extensions' => 'php_extensions',
                 'php : cli configuration' => 'php_cli_configuration',
                 'php : web configuration' => 'php_web_configuration',
                 'platform : owner' => 'fs_owner');
  
  $failures = 0;
  $results = array();

  foreach ($tests as $label => $method) {
    $result = 'ok';
    $message = '';
    try {
      $message = (string) call_user_func(array(Diagnosis, $method));
    }
    catch (Exception $e) {
      $failures++;
      $result = '*** ko ***';
      $message = $e->getMessage();
    }
    $results[] = array($label, $result, $message);
  }
  
  if (!$req->isCron()) {
    print_r($results);
    if (!empty(Diagnosis::$warnings)) {
      print('warnings -------------');
      print_r(Diagnosis::$warnings);
    }
  }
  elseif ($failures > 0) {
    $log->error($results);
  }

}
catch (Exception $e) { $err->add($e); }
