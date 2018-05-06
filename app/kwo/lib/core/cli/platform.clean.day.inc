<?php

Cache::clean();
UploadManager::clean();
HtmlDocument::clean();

$dbo->optimize();

$files = glob('var/apache/*/*/*/*.*');
$files = $files ?: array();
$now = time();
$ttl = 6 * 60 * 60;
foreach ($files as $file) {
  if (!file_exists($file)) continue ;
  $ts = filemtime($file);
  if (($now - $ts) > $ttl) {
    unlink($file);
  }
}