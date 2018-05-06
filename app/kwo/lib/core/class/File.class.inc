<?php

/***   [STDIN] [STDOUT] [STDERR] ***/

/**

getName() : /path/file.txt
getPath() : /path
getBaseName() : file.txt

**/

class File
{
  const REG_BAD_EXT = 'php|php.|inc|cgi|pl|psp|exe';
  const REG_COMPRESS = '\.zip|\.tgz|\.tar|\.rar|\.z';
  const REG_IMAGE = '\.jpg|\.jpeg|\.gif|\.png|\.tiff|\.bmp';
  const REG_VIDEO = '\.avi|\.mov|\.mp4|\.ogv|\.mpg|\.f4v';
  const REG_AUDIO = '\.wav|\.ogg|\.mp3';
  const WHITE_LIST = 'csv|jpg|jpeg|gif|png|pdf|txt|rtf|doc|docx|ppt|pptx|xls|xlsx|webp|webm|wmv|flv|avi|mov|mp4|f4v|ogv|ogg|wav|mp3';

  protected $_folder   = null;
  protected $base_name = false;
  protected $name      = false;
  protected $fd        = null;
  protected $img_flag  = false;
  protected $mode      = 'r';
  protected $path      = false;
  protected $stat_hash = null;

  public function __construct($filename, $mode='r') {
    $this->name = trim($filename, '/');
    $this->base_name = basename($this->name);
    $this->path = dirname($this->name);
    $this->mode = strtolower($mode);

    if ($filename === 'php://output') {
      $this->name = $filename;
      return;
    }

    $req = S('req');

    $allowed = array(DOC_PATH, PUB_PATH, USR_PATH);
    if ($req->isBack() || $req->isCli()) {
      array_push($allowed, VAR_PATH);
      if ($req->isCli()) {
        array_push($allowed, WEB_PATH);
      }
    }

    if (strpos($this->path, '.') !== false ||
        !in_array(substr($this->name, 0, 3), $allowed)) {
      S('log')->error('path not allowed ['.$this->name.']');
        throw new FileSystemException('path not allowed');
    }

    if ($mode === 'r' && !is_readable($this->getName())) {
      S('log')->error('file not readable ['.$this->name.']', __METHOD__);
      throw new FileSystemException('file not readable');
    }
    elseif ($mode === 'd' && !$this->isDownloadable($this->getName())) {
      S('log')->error('file not downloadable ['.$this->name.']', __METHOD__);
      throw new FileSystemException('file not downloadable');
    }
    elseif ($mode === 'w') {
      if (!is_writable($this->path)) {
        S('log')->error('directory not writable ['.$this->path.']', __METHOD__);
        throw new FileSystemException('directory not writable');
      }
      if (file_exists($this->getName()) && !self::isWritable($this->getName())) {
        S('log')->error('file not writable ['.$this->name.']', __METHOD__);
        throw new FileSystemException('file not writable');
      }
    }

    // FIX Hugues : add "&& filesize($this->name) !== false"  SI le fichier > 2Go, return false
    // voir http://www.php.net/manual/fr/function.stat.php
    // une méthode avec curl permet de connaitre la taille du fichier
    if (file_exists($this->name) && filesize($this->name) !== false) {
      $this->stat($this->getName());
    }

  }

  public function close() {
    if ($this->fd === null) return;
    fclose($this->fd);
  }

  public function getFolder() {
    $this->_folder = Folder::getInstance($this->getPath());
    return $this->_folder;
  }

  public function getDetails() {
    $h = array('name' => $this->getBaseName(),
               'path' => $this->getPath(),
               'size' => $this->getSize(),
               'ctime' => $this->getCtime(),
               'mtime' => $this->getMtime());
    if (self::isImage($this->getBaseName())) {
      try {
        $img = $this->getImg();
        $h['width'] = $img->getWidth();
        $h['height'] = $img->getHeight();
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    return $h;
  }

  public function getBaseName() { return $this->base_name; }

  public function getName() { return $this->name; }

  public function getUrl() { return '/'.$this->name; }

  public function getPath() { return $this->path; }

  public function moveTo($filename) {
    $folder = Folder::getInstance(dirname($filename));
    if (!$folder->isWritable()) {
      S('log')->error('dest folder not writable ['.$folder->getPath().']', __METHOD__);
      throw new FileSystemException('moveTo failure (1)');
    }
    if ($this->getName() == $filename) {
      return $this;
    }
    if (@rename($this->getName(), $filename) === false) {
      S('log')->error('from ['.$this->getName().'] to ['.$filename.']',
                        __METHOD__);
      throw new FileSystemException('moveTo failure (2)');
    }
    $this->name = $filename;
    $this->path = $folder->getPath();
    return $this;
  }

  public function moveIn($folder) {
    $folder = $folder instanceof Folder ? $folder : Folder::getInstance($folder);
    if (!$folder->isWritable()) {
      S('log')->error('dest folder not writable ['.$folder->getPath().']', __METHOD__);
      throw new FileSystemException('move failed');
    }
    $filename = $folder->getPath().'/'.$this->getBaseName();
    if (@rename($this->getName(), $filename) === false) {
      S('log')->error('from ['.$this->getName().'] to ['.$path.']',
                        __METHOD__);
      throw new FileSystemException('move failed (2)');
    }
    $this->name = $filename;
    $this->path = $folder->getPath();
    return $this;
  }

  public function copyTo($filename, $same_path=false) {
    $base_name = basename($filename);
    if ($same_path === true) {
      $path = $this->getPath();
      $dest = $path.'/'.$base_name;
    }
    else {
      $path = dirname($filename);
      $dest = $filename;
    }
    Folder::getInstance($path, 'w');
    if (strlen($dest) < 3) {
      S('log')->error('bad destination file ['.$dest.']', __METHOD__);
      throw new FileSystemException('copy (1)');
    }
    if ($this->getName() === $dest) {
      return $this;
    }
    if (@copy($this->getName(), $dest) === false) {
      S('log')->error('from ['.$this->getName().'] to ['.$dest.']', __METHOD__);
      throw new FileSystemException('copy (2)');
    }
    return $this instanceof Img
         ? Img::getInstance($dest)
         : File::getInstance($dest);
  }

  public function copyIn($folder) {
    $folder = $folder instanceof Folder ? $folder : Folder::getInstance($folder);
    if (!$folder->isWritable()) {
      S('log')->error('dest folder not writable ['.$folder->getPath().']', __METHOD__);
      throw new FileSystemException('move failed');
    }
    $filename = $folder->getPath().'/'.$this->getBaseName();
    if ($this->getName() == $filename) {
      return $this;
    }
    if (@copy($this->getName(), $filename) === false) {
      S('log')->error('from ['.$this->getName().'] to ['.$filename.']',
                        __METHOD__);
      throw new FileSystemException('move failed (2)');
    }
    return $this instanceof Img
         ? Img::getInstance($this->name)
         : File::getInstance($this->name);
  }

  public function getNext() {
    if ($this->fd === null) $this->open();
    if (feof($this->fd)) {
      return false;
    }
    $line = fgets($this->fd);
    if ($line === false) {
      return false;
    }
    return trim($line);
  }

  public function getSize() {
    return $this->stat_hash['size'];
  }

  public function rewind() {
    return rewind($this->fd);
  }

  public function getCtime() {
    return $this->stat_hash['ctime'];
  }

  public function getMtime() {
    return $this->stat_hash['mtime'];
  }

  public function isDownloadable() {
    return is_readable($this->getName());
  }

  public function getExtension() {
    $parts = explode('.', $this->getBaseName());
    if (count($parts) < 2) return null;
    $file_ext = $parts[count($parts)-1];
    return strtolower($file_ext);
  }

  //  public function isImg() { return $this->img_flag; }

  public function getImg() { return new Img($this->name); }

  public function isText() {
    $chunk = $this->getChunk(16);
    return preg_match('/ /', $chunk) || Valid::isWord($chunk);
  }

  public function getChunk($size=128) {
    $fd = fopen($this->getName(), 'r');
    $chunk = fread($fd, $size);
    fclose($fd);
    return $chunk;
  }

  public function isFolder() {
    return is_dir($this->getName());
  }

  public function asArray($line_count=0) {
    $fd = fopen($this->getName(), 'r');
    $lines = array();
    $i = 1;
    while ($line = fgets($fd)) {
      if ($line_count > 0 && $i > $line_count) break;
      $lines[] = trim($line);
      $i++;
    }
    fclose($fd);
    return $lines;
  }

  public function getContent() {
    $ret = @file_get_contents($this->getName());
    if ($ret === false) {
      S('log')->error($this->getName(), __METHOD__);
      throw new FileSystemException('content unreadable ['.$this->getBaseName().']');
    }
    return $ret;
  }

  public function getStream() {
    $this->fd = fopen($this->getName(), $this->mode);
    if ($this->fd === false) {
      S('log')->error($this->getName(), __METHOD__);
      throw new FileSystemException('open failure');
    }
    return $this->fd;
  }

  protected function open() {
    $this->fd = fopen($this->getName(), $this->mode);
    if ($this->fd === false) {
      S('log')->error($this->getName(), __METHOD__);
      throw new FileSystemException('open failure');
    }
    return $this;
  }

  public function rename($new_base_name) {
    $new_base_name = self::NameClean($new_base_name);
    if (self::isMaliciousName($new_base_name)) {
      throw new FileSystemException('nom de fichier interdit');
    }
    $ret = @rename($this->getName(),
                   $this->getPath().'/'.$new_base_name);
    if ($ret === false) {
      S('log')->error('from ['.$this->getName().']'
                        .' to ['.$new_base_name.']', __METHOD__);
      throw new FileSystemException('rename failure');
    }
    $this->name = $this->getPath().'/'.$new_base_name;
    $this->base_name = $new_base_name;
    return $this;
  }

  private function stat() {
    $ret = @stat($this->getName());
    if ($ret === false) {
      S('log')->error($this->getName(), __METHOD__);
      throw new FileSystemException('file stat failure');
    }
    $this->stat_hash = $ret;
  }

  public function unlink() {
    if (@unlink($this->getName()) === false) {
      S('log')->error($this->getName(), __METHOD__);
      throw new FileSystemException('file deletion failure');
    }
  }

  public function write($str) {
    if ($this->fd === null) $this->open('w');
    return fwrite($this->fd, $str);
  }

  public function writeln($str) {
    if ($this->fd === null) $this->open('w');
    return fwrite($this->fd, $str."\n");
  }

/*  public function __toString() {
//    S('log')->backtrace();
    S('log')->trace('** DEPRECATED - contact FXB', __METHOD__);
//    return $this->getName();
  } */

  public function asDataFileName() {
    return $this->getBaseName();
  }

  /*** STATIC ***/

  public static function getInstance($filename, $mode='r') {
    return new File($filename, $mode);
  }

  public static function exists($filename) {
    $filename = trim($filename, '/ ');
    if (strlen($filename) <= 1) return false;

    $path = dirname($filename);
    $base_name = basename($filename);
    if (preg_match('#[\x00-\x1F\x7F-\x9F/\\\\]#', $base_name)) {
      S('log')->error('dangerous path', __METHOD__);
      return false;
    }
    if (strpos($path, '.') !== false ||
        strpos($filename, '..') !== false ||
        $filename[0] === '.' ||
        !in_array(substr($filename, 0, 3), array(WEB_PATH, DOC_PATH, USR_PATH)) ||
        !file_exists($filename)) return false;
    return true;
  }

  public static function isImage($file) {
    $file = $file instanceof File ? $file->getBaseName() : $file;
    return preg_match('/('.self::REG_IMAGE.')$/i', $file) >= 1;
  }

  public static function isVideo($filename) {
    return preg_match('/('.self::REG_VIDEO.')$/i', $filename) >= 1;
  }

  public static function isAudio($filename) {
    return preg_match('/('.self::REG_AUDIO.')$/i', $filename) >= 1;
  }

  public static function isPublic($filename) {
    if (self::exists($filename) === false) return false;
    return is_readable($filename);
  }

  public static function isReadable($filename) {
    $filename = trim($filename, '/');
    if (self::exists($filename) === false) return false;
    return is_readable($filename);
  }

  public static function isMaliciousName($name) {
    $name = rtrim($name);
    if (preg_match('#[\x00-\x1F\x7F-\x9F/\\\\]#', $name)) {
      S('log')->error('dangerous path', __METHOD__);
      return true;
    }
    if ($name[0] == '.' || strpos($name, '..') !== false) {
      return true;
    }
    return (bool) preg_match('/\.('.self::REG_BAD_EXT.')$/i', $name);
  }

  public static function isAllowedName($name) {
    $name = trim($name);
    if (preg_match('#[\x00-\x1F\x7F-\x9F/\\\\]#', $name)) {
      S('log')->error('security - malicious characters', __METHOD__);
      return false;
    }
    if ($name[0] == '.' || strpos($name, '..') !== false) {
      return false;
    }
    if (strpos($name, '.') === false) {
      return false;
    }
    return (bool) preg_match('/\.('.self::WHITE_LIST.')$/i', $name);
  }

  public static function isWritable($filename) {
    $filename = trim($filename, '/');
    return self::exists($filename) && is_writable($filename);
  }

  public static function hasBom($filepath) {
    $str = file_get_contents($filepath);
    $bom = pack('CCC', 0xef, 0xbb, 0xbf);
    if (strncmp($str, $bom, 3) == 0) {
      //      echo "BOM detected - file is UTF-8\n";
      $str = substr($str, 3);
    }
    return false;
  }

  public static function hasMagicNumber($filepath) {
    $numbers = array('doc'  => array('\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1',
                                     '\x0D\x44\x4F\x43',
                                     '\xCF\x11\xE0\xA1\xB1\x1A\xE1\x00',
                                     '\xDB\xA5\x2D\x00',
                                     '\xEC\xA5\xC1\x00'),
                     'docx' => array('\x50\x4B\x03\x04',
                                     '\x50\x4B\x03\x04\x14\x00\x06\x00'),
                     'gif'  => array('\x47\x49\x46\x38\x37\x61',
                                     '\x47\x49\x46\x38\x39\x61'),
                     'jpg'  => array('\xFF\xD8\xFF\xE0',
                                     '\xFF\xD8\xFF\xE1',
                                     '\xFF\xD8\xFF\xE8'),
                     'odf'  => array('\x50\x4B\x03\x04'),
                     'pdf'  => array('\x25\x50\x44\x46'),
                     'png'  => array('\x89\x50\x4E\x47\x0D\x0A\x1A\x0A'),
                     'rtf'  => array('\x7B\x5C\x72\x74\x66\x31'));

    $parts = pathinfo($filepath);
    $extension = strtolower($parts['extension']);
    if ($extension == 'jpeg') {
      $extension = 'jpg';
    }
    if (!array_key_exists($extension, $numbers)) {
      return false;
    }
    $chunk = file_get_contents($filepath, false, null, 0, 16);
    foreach ($numbers[$extension] as $number) {
      $str = pack('H*', str_replace('\x', '', $number));
      if (substr($chunk, 0, strlen($str)) === $str) {
        return true;
      }
    }
    return false;
  }
  
  public static function NameUniquify($name) {
    $parts = explode('.', $name);
    $ext = '';
    if (count($parts) >= 2) {
      $ext = array_pop($parts);
    }
    $name = join('.', $parts);
    $name .= '-'.time();
    if (strlen($ext) > 0) { 
      $name .= '.'.$ext;
    }
    $name = ltrim($name, '/.- ');
    return $name;
  }

  public static function NameClean($name) {
    $name = SH::stripAccents($name);
    $name = strtolower($name);
    $name = preg_replace('/[^(0-9|a-z|_|\-|\.)]/i', '', $name);
    $name = trim($name);
    $name = ltrim($name, '/.- ');
    return $name;
  }

  public static function put($file, $content) {
    $file = trim($file, ' /');
    $paths = array(DOC_PATH, ETC_PATH, LIB_PATH, USR_PATH, VAR_PATH, WEB_PATH);
    if (!in_array(substr($file, 0, 3), $paths) ||
        strlen($file) <= 6 ||
        strpos($file, '..') !== false) {
      S('log')->error('invalid path ['.$file.']', __METHOD__);
      return ;
    }
    if (is_array($content)) {
      $content = serialize($content);
    }
    $ret = @file_put_contents($file, $content);
    if ($ret === false) {
      S('log')->error('write error ['.$file.']', __METHOD__);
      return ;
    }
    Shell::chown($file);
    return $ret;
  }

  public static function get($file) {
    $file = trim($file, ' /');
    $paths = array(DOC_PATH, ETC_PATH, LIB_PATH, USR_PATH, VAR_PATH, WEB_PATH);
    if (!in_array(substr($file, 0, 3), $paths) ||
        strlen($file) <= 6 ||
        strpos($file, '..') !== false) {
      S('log')->error('invalid path ['.$file.']', __METHOD__);
      return ;
    }
    $content = @file_get_contents($file);
    if ($content === false) {
      S('log')->error('read error ['.$file.']', __METHOD__);
      return ;
    }
    return $content;
  }

}
