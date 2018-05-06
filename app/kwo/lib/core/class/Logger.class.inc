<?php

final class Logger
{
  private $disabled = false;
  private $microtime = 0;
  private $counter = 0;
  private $prefix = null;

  const MAX_MESSAGE = 30;

  const ERROR_LEVEL_LOW = 1;
  const ERROR_LEVEL_MEDIUM = 2;
  const ERROR_LEVEL_HIGH = 3;

  const PATH = 'var/log';

  const ERR_FILE = 'var/log/error';
  const URL_FILE = 'var/log/url';

  const LEVEL_FATAL = 1;
  const LEVEL_ERROR = 2;
  const LEVEL_WARN = 3;
  const LEVEL_TRACE = 4;
  const LEVEL_DEBUG = 5;

  public static $levels = array(self::LEVEL_FATAL => 'fatal',
                                self::LEVEL_ERROR => 'error',
                                self::LEVEL_WARN  => 'warn',
                                self::LEVEL_TRACE => 'trace',
                                self::LEVEL_DEBUG => 'debug');

  public function __construct() {
    $this->microtime = microtime(true);
    if (IS_CLI) {
      $this->setAppender('error');
    }
  }

  private function write($level, $msg, $prefix=null) {
    if (P('app.load') > 0) return ;
    if ($this->disabled === true && $level != self::LEVEL_DEBUG) return ;
    if ($this->prefix == null) {
      if (substr(P('app.domain'), -4) == '.kwo') {
        $this->prefix = S('app')->getName().'.';
      }
      else {
        $this->prefix = '';
      }
    }
    //error_log(var_export(debug_backtrace(), true));

    if (!IS_CLI) {
      $this->counter++;
      if ($level != self::LEVEL_DEBUG) {
        if ($this->counter > self::MAX_MESSAGE) {
          return ;
        }
        if ($this->counter == self::MAX_MESSAGE) {
          $level = self::LEVEL_ERROR;
          $msg = '** TOO MANY ERRORS **';
          $prefix = 'Logger';
        }
      }
    }
    if (is_object($msg)) {
      if ($msg instanceof Exception) {
        if (empty($prefix)) {
          $prefix = '#'.$msg->getFile().':'.$msg->getLine();
        }
        $msg = $msg->getMessage();
        $msg = empty($msg) ? 'Exception' : $msg;
      }
      if ($msg instanceof ActiveRecord) {
        $msg = $msg->getName().' ['.$msg->getModel().':'.$msg->getModelId().':'.$msg->getId().']';
      }
    }
    elseif (is_array($msg)) {
      $msg = json_encode($msg);
    }
    if (!Platform::hasSingleton('req')) {
      if ($prefix !== null) {
        $msg = '{'.$prefix.'} '.$msg;
      }
      $ret = 'LOGGER 0 [startup] '.strtoupper(self::$levels[$level]).' init '.$msg;
      error_log($ret);
      return ;
    }
    $req = S('req');
    $ret = round(microtime(true) - $this->microtime, 3);
    $ret = str_pad($ret, 5, '0');
    $ret .= ' ';
    $ret .= strtoupper(self::$levels[$level]).' ';
    $ret .= '[';
    if ($req->isCli()) {
      $ret .= get_current_user().':'.getmypid();
    }
    else {
      $ret .= $req->getClient('address');
      if (Platform::hasSingleton('admin') === true) {
	      $ret .= ':admin('.S('admin')->login.')';
      }
      elseif (S('ctx')->_user['id'] >= 1) {
        $ret .= ':user('.S('ctx')->_user['id'].')';
      }
    }
    $ret .= '] ';
    $ret .= $this->prefix;
    $ret .= $req->getScope().'.';
    $ret .= $req->getExtension().'.';
    $ret .= $req->getAction();
    $ret .= ' - ';
    if ($prefix !== null && $prefix[0] != '/') {
      $msg = '{'.$prefix.'} '.$msg;
    }
    $ret .= $msg;
    if ($prefix[0] === '/') { error_log($msg.' || '.$prefix);
      $prefix = str_replace(array('.', '/', ' '), array('', '', ''),
                            $prefix);
      $ret = '['.date(DH::DATE_MYSQL).'] '.$ret;
      file_put_contents(self::PATH.'/'.$prefix, $ret."\n",
                        FILE_APPEND|LOCK_EX);
      if (IS_CLI) {
        Shell::chown(self::PATH.'/'.$prefix);
      }
    }
    else {
      error_log($ret);
    }
  }

  private function setAppender($filename, $prefix=null) {
    if (preg_match('/(\.|\/)/', $filename)) return false;
    ini_set('display_errors', 'on');
    ini_set('log_errors', 'on');
    ini_set('error_log', self::PATH.'/'.$filename);
  }

  public function debug($msg, $prefix=null) {
    if (is_object($msg)) {
      if ($msg instanceof Collection) {
        $prefix = '[Collection] '.$prefix;
        $msg = $msg->asArray();
      }
      elseif ($msg instanceof ActiveRecord) {
        $prefix = '[ActiveRecord:'.$msg->getModel().'('.$msg->getId().')] '.$prefix;
        $msg = $msg->getAttributes();
      }
      else {
        $msg = $msg instanceof Exception ? $msg->getMessage() : (array) $msg;
      }
      $msg = $msg;
    }
    if (is_int($msg) && $msg === 0) {
      $msg = 'INT(0)';
    }
    elseif (is_null($msg)) {
      $msg = 'NULL';
    }
    elseif (is_bool($msg)) {
      $msg = 'BOOL('.intval($msg).')';
    }
    if (is_array($msg)) {
      $msg = "\n".var_export($msg, true);
    }
    if ($prefix !== null) {
      $msg = $prefix.' '.$msg;
    }
    $this->write(self::LEVEL_DEBUG, $msg);
  }

  public function error($msg, $prefix=null) { $this->write(self::LEVEL_ERROR, $msg, $prefix); return false; }
  public function fatal($msg, $prefix=null) { $this->write(self::LEVEL_FATAL, $msg, $prefix); exit (0); }
  public function trace($msg, $prefix=null) { $this->write(self::LEVEL_TRACE, $msg, $prefix); }
  public function warn($msg, $prefix=null)  { $this->write(self::LEVEL_WARN, $msg, $prefix); }

  public function bench($msg='') {
    $usage = getrusage();
    $msg = '[MEM='.memory_get_usage().'b|CPU='.$usage['ru_stime.tv_usec'].'] '.$msg;
    $this->write(self::LEVEL_TRACE, $msg, $bindings);
  }

  public static function getInstance() {
    if (Platform::hasSingleton('log')) return ;
    Platform::addSingleton('log', new Logger());
  }

  public static function clean() {
    $files = array(self::ERR_FILE, self::URL_FILE);
    foreach ($files as $file) {
      if (!file_exists($file)) continue ;
      $fp = fopen($file, 'w');
      ftruncate($fp, 0);
      fclose($fp);
    }
  }

  public static function archive($force=false) {
    if (date('H') == 5) {
      $files = glob(self::PATH.'/*.gz');
      $files = $files ? $files : array();
      $ts = time();
      $ttl = 6 * 31 * 24 * 60 * 60;
      foreach ($files as $file) {
        if (!file_exists($file)) continue ;
        $fts = filemtime($file);
        if (($ts - $fts) > $ttl) {
          unlink($file);
        }
      }
    }
    $files = array(self::ERR_FILE, self::URL_FILE);
    $time = date('Y-m-d-H-i-s');
    foreach ($files as $file) {
      if (!file_exists($file)) continue ;
      if ($force === false && filesize($file) < (2 * 1048576)) continue ;
      $file_in = $file;
      $file_out = $file . '_' . $time . '.gz';
      error_log($file_out);
      if ($file_out = gzopen($file_out, 'w9')) {
        if ($file_in = fopen($file_in, 'r+b')) {
          while (!feof($file_in)) {
            $data = fread($file_in, 1024 * 512);
            gzwrite($file_out, $data);
          }
          $data = null;
          ftruncate($file_in, 0);
          fclose($file_in);
        }
        else {
          S('log')->error('file error', __METHOD__);
        }
        gzclose($file_out);
      }
    }
    return $time;
  }

  public function disable() {
    $this->disabled = true;
  }

  public function enable() {
    $this->disabled = false;
  }

  public function backtrace($opts=null) {
    $opts = (array) $opts;
    $results = array('kwo stack trace');
    if ($opts['msg']) {
      $results[0] .= '['.$opts['msg'].']';
    }
    $trace = debug_backtrace(false);
    foreach ($trace as $index => $h) {
      if ($index == 0) continue ;
/*      $results[$index] = array('file' => $h['file'],
                              'line' => $h['line'],
                              'function' => $h['function'],
                              'class' => $h['class'],
                              'type' => $h['type']); */
      $results[] = $index.'. '.$h['file'].':'.$h['line'].' '.$h['class'].$h['type'].$h['function'].'()';
    }
    if ($opts['limit'] > 0) {
      $results = array_slice($results, (int) $opts['offset'], $opts['limit']);
    }
    $results = join("\n", $results);
    $this->debug("\n".$results);
  }

}
