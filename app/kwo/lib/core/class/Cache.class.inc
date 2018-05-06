<?php

class Cache
{
  const PATH = 'var/cache';

  public static function get($key) {
    return Storage::getInstance()->get($key);
  }

  public static function set($key, $value, $ttl) {
    return Storage::getInstance()->set($key, $value, $ttl);
  }

  public static function clean() {
    Storage::getInstance()->flush();
  }
}
