<?php

class System
{
  public static function pwd() {
    return realpath($_SERVER['PWD']);
  }

  public static function fork($cmd) {
    exec('nice '.$cmd.' > /dev/null 2>&1 &');
  }

  public static function example() {
    exec('mycommand 2>&1', $out, $ret);
    if ($ret == Platform::EXIT_STATUS_SUCCESS) {
    }
  }

  public static function getApacheUser() {
    if (self::getOsType() === 'windows') return null;
    $users = array('apache', '_www', 'www-data', 'www',
                   'wwwrun', 'httpd');
    foreach ($users as $user) {
      $h = posix_getpwnam($user);
      if ($h === false) continue ;
      return array('uid' => $h['uid'],
                   'gid' => $h['gid'],
                   'user' => $user,
                   'name' => $user);
    }
    return null;
  }

  public static function is64bits() {
    // http://stackoverflow.com/questions/2353473/can-php-tell-if-the-server-os-it-64-bit
    // PHP_INT_SIZE === 4 => 32bits
    return PHP_INT_SIZE === 8;
  }

  public static function getOsType() {
    // windows, linux, osx
    // voir : if(substr(PHP_OS, 0, 3) == 'WIN' ) {
    $system = php_uname('s');
    $system = strtolower($system);
    if ($system == 'linux') return 'linux';
    if (strpos($system, 'osx') !== false ||
        strpos($system, 'darwin') !== false) return 'osx';
    if (strpos($system, 'windows') !== false) return 'windows';
    S('log')->trace($system, __METHOD__);
    return 'unknown';
  }

  public static function isOsx() {
    return self::getOsType() === 'osx';
  }

  public static function isLinux() {
    return self::getOsType() === 'linux';
  }

  public static function getMysqlUser() {
    if (self::getOsType() === 'windows') return null;
    $users = array('mysql', '_mysql');
    foreach ($users as $user) {
      $h = posix_getpwnam($user);
      if ($h === false) continue ;
      return array('uid' => $h['uid'],
                   'gid' => $h['gid'],
                   'user' => $user,
                   'name' => $user);
    }
    return null;
  }

  public static function getMysqlRootPassword() {
    $file = '/etc/my.cnf';
    if (!is_readable($file)) {
      $file = '/root/.my.cnf';
      if (is_readable($file)) {
        return false;
      }
    }
    $h = @parse_ini_file($file, false);
    if (!isset($h['password'])) return false;
    return (string) $h['password'];
  }

  public static function getLoadAvg() {
    $data = shell_exec('cat /proc/loadavg');
    $arr = explode(' ',$data);
    $arr[0] = $arr[0] * 100;
    return $arr[0];
  }

  public static function getUsedMemory() {
  }

}