<?php

class Storage {

  private $support = null;
  private $trace = false;

  const TTL = 600;

  public function __construct() {
    $this->support = new FileStorage();
  }

  public function get($key) {
    $value = $this->support->get($key);
    //    S('log')->debug($value === false, 'hit ('.$key.')');
    if ($this->trace && $value !== false) {
      S('log')->debug($value, 'hit ('.$key.')');
    }
    //    error_log($value);
    return $value;
  }

  public function set($key, $value, $ttl) {
    //    if ($ttl === null || $ttl === true) {
    if ($ttl === true) {
      $ttl = is_null(P('cache.ttl')) ? self::TTL : P('cache.ttl');
    }
    $ttl = (int) $ttl;
    if ($ttl < 1) return ;
    return $this->support->set($key, $value, $ttl);
  }

  public function delete($key) {
    return $this->support->delete($key);
  }

  public function flush() {
    return $this->support->flush();
  }

  public static function getInstance($type=null) {
    if ($type === null) {
      if (Platform::hasSingleton('stg')) return Platform::getSingleton('stg');
      Platform::addSingleton('stg', new Storage());
      return Platform::getSingleton('stg');
    }
    return new Storage($type);
  }

}

class FileStorage {

  public function __construct() {}

  public function get($key) {
    //    S('log')->debug($key);
    $path = $this->asPath($key);
    //S('log')->debug($path);
    if (!file_exists($path)) {
      return false;
    }
    $value = @file_get_contents($path);
    if ($value === false) {
      S('log')->trace('cache get failure ['.$path.']', __METHOD__);
      return false;
    }
    $struct = unserialize($value);
    if (time() > $struct['expire']) {
      return false;
    }
    return $struct['value'];
  }

  public function set($key, $value, $ttl) {
    //S('log')->debug($key);
    if ($ttl < 1) return false;
    $path = $this->asPath($key);
    $tmpf = @tempnam(TMP_PATH, '__KWO');
    if ($tmpf === false) {
      S('log')->error('cache set failure ['.$path.']', __METHOD__);
      return false;
    }
    $hash = array('expire' => (time() + $ttl),
                  'value' => $value);
    $fd = @fopen($tmpf, 'w');
    if ($fd === false) return false;
    //    flock($fd, LOCK_EX);
    fwrite($fd, serialize($hash));
    fclose($fd);
    rename($tmpf, $path);
    return true;
  }

  public function delete($key) {
    $path = $this->asPath($key);
    if (!file_exists($path)) return false;
    if (@unlink($path) === false) {
      S('log')->error($path, __METHOD__);
    }
  }

  public function flush() {
    if (strlen(self::PATH) > 5 && is_dir(self::PATH)) {
      $files = glob(self::PATH.'/*/*.cache');
      $files = $files ? $files : array();
      foreach ($files as $file) {
        if (!file_exists($file)) continue ;
        unlink($file);
      }
    }
    if (strlen(TMP_PATH) > 11 && is_dir(TMP_PATH)) {
      $files = glob(TMP_PATH.'/*');
      $files = $files ? $files : array();
      foreach ($files as $file) {
        if (!file_exists($file)) continue ;
        unlink($file);
      }
    }
  }

  public function asPath($key) {
    if (!preg_match('/^[0-9a-z][0-9a-z:._-]+$/i', $key)) {
      S('log')->error('invalid cache key ['.$key.']', __METHOD__);
      throw new Exception('invalid cache key');
    }
    $path = self::PATH.'/'.$key[0].'/'.$key.'.cache';
//    error_log($path);
    return $path;
  }

}

class MemcachedStorage {}
