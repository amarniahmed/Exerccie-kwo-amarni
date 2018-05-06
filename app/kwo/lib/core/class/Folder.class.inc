<?php

class Folder implements Iterator
{
  const FILTER_IMG   = 1;
  const FILTER_REGEXP = 2;
  const FILTER_DIR   = 3;

  const TRASH_NAME = '.trash';

  private $fd = null;
  private $files = null;
  private $folders = null;
  private $name = null;
  private $path = null;

  private $current = null;
  private $index = 0;

  public function __construct($path=null, $mode=null) {
    $req = S('req');

    if ($req->isBack()) {
      if (empty($this->root_path)) $this->root_path = DOC_PATH;
    }
    elseif ($req->isFront() || $req->isAccount()) {
      $this->root_path = USR_PATH;
    }
    else {
      $this->root_path = '.';
    }

    $this->path = trim($path, '/ ');
    $this->base_name = basename($this->path);

    if ((empty($this->path) || $this->path === '.') && !$req->isCli()) {
      $this->path = $this->root_path;
    }

    if (!$req->isCli() && !preg_match('#^('.DOC_PATH.'|'.PUB_PATH.'|'.USR_PATH.')(/|$)#', $this->path)) {
      S('log')->error('path not allowed ['.$this->path.']', __METHOD__);
      throw new FileSystemException('path not allowed');
    }

    $this->fd = @opendir($this->path);
    if ($this->fd === false) {
      S('log')->error('can not open directory ['.$this->path.']', __METHOD__);
      throw new FileSystemException('can not open directory');
    }
    if ($mode === 'w' && !is_writable($this->path)) {
      S('log')->error('directory not writable ['.$this->path.']', __METHOD__);
      throw new FileSystemException('directory not writable');
    }
  }

  public function getUsedSpace($opts=null) {
    // en Mo
    $files = (array) glob($this->path.'/*');
    if (count($files) < 1) return 0;
    $result = 0;
    foreach ($files as $file) {
      $result += filesize($file);
    }
    return number_format($result / 1048576, 1, '.', '');
//    return NH::asFileSize($result, 'M');
  }

  public function compress() {
    ArchiveManager::compress($this);
  }

  public function hide() {
    if ($this->path === DOC_PATH) throw new Exception('operation prohibited');
    if (!is_dir(DOC_PATH.'/'.self::TRASH_NAME) &&
        mkdir(DOC_PATH.'/'.self::TRASH_NAME) === false) throw new Exception('no trash available');
    $ret = rename($this->path, DOC_PATH.'/'.self::TRASH_NAME.'/'.basename($this->path).'-'.time());
    if ($ret === false) {
      throw new Exception('can not move directory to trash');
    }
  }

  public function addChild($name) {
    $name = strtolower($name);
    $name = SH::stripAccents($name);
    if (!Valid::isAlphanum($name)) throw new Exception('alpha num');
    if (strpos($name, '.') !== false) throw new Exception('bad character [ . ]');
    if (!file_exists($this->path.'/'.$name)) mkdir($this->path.'/'.$name);
    return true;
  }

  private function cross() {
    $this->files = array();
    $this->folders = array();
    while (false !== ($file = readdir($this->fd))) {
      if (substr($file, 0, 1) === '.' ||
      	  substr($file, 0, 1) === '#' ||
      	  substr($file, -1, 1) === '~' ||
	    $file === self::TRASH_NAME) continue;
      if (is_dir($this->path.'/'.$file)) {
        $this->folders[] = array('name' => $file,
                                 'is_folder' => 1 ,
                                 'path' => $this->path.'/'.$file);
      }
      else {
        $this->files[] = array('name' => $file,
                               'size' => filesize($this->path.'/'.$file),
                               'time' => filectime($this->path.'/'.$file),
                               'path' => $this->path.'/'.$file);
      }
    }
    usort($this->folders, array($this, 'sortFiles'));
    usort($this->files, array($this, 'sortFiles'));
  }

  public function getFiles($filter=null, $data='') {
    if ($this->files === null) $this->cross();
    if ($filter === null) return $this->files;
    $results = array();
    foreach ($this->files as $file) {
      if ($filter === self::FILTER_IMG) {
	if (!preg_match('/('.File::REG_IMAGE.'$)/i', $file['name'])) continue;
      }
      elseif ($filter === self::FILTER_DIR) {
        if (!is_dir($file['name'])) continue;
      }
      elseif ($filter === self::FILTER_REGEXP) {
	if (!preg_match('/'.$data.'/i', $file['name'])) continue;
      }
      $results[] = $file;
    }
    return $results;
  }

  public function getChildren() {
    if ($this->folders === null) $this->cross();
    if (!$this->isRoot()) {
      $up = array('name' => '..',
                  'is_folder' => 1,
                  'path' => dirname($this->path));
      array_unshift($this->folders, $up);
    }
    return $this->folders;
  }

  public function getNbFiles() {
    return count($this->getFiles());
  }

  public function size() {
    return Shell::du($this->path);
  }

  public function isWritable() {
    return is_writable($this->path);
  }

  public function hasFile($filename) {
    return file_exists($this->path.'/'.$filename);
  }

  public function getHtmlPath() {
    $ret = '/ ';
    $folders = explode('/',$this->path);
    foreach ($folders as $folder) {
      $path .= '/'.$folder;
      if (!preg_match('#^'.$this->root_path.'(/|$)#', trim($path, '/'))) $ret .= $folder;
      else $ret .= '<a href="javascript:Kwo.Dir.view(\''.$path.'\')" class="color2">'.$folder.'</a>';
      $ret .= ' / ';
    }
    return $ret;
  }

  public function getPath() {
    // ne devrait on as retourner le dirname ?
    return $this->path;
  }

  public function getName() { return $this->path; }

  public function getBaseName() { return $this->base_name; }

  public function getParentPath() { return dirname($this->path); }

  public function isRoot() { return $this->path === $this->root_path; }

  private function sortFiles($a,$b) {
    return $a['name'] > $b['name'] ? 1 : -1;
  }

  /*** Iteration ***/
  public function next() {
    $this->current = readdir($this->fd);
    $this->index++;
  }
  public function rewind() {
    rewinddir($this->fd);
    $this->next();
//    $this->next();
//    $this->next();
    $this->index = 0;
  }
  public function current() { return $this->current; }
  public function key() { return $this->index; }
  public function valid() { return ($this->current !== false); }
  /*** Iteration ***/

  public function asHash() {
    $results = array();
    $path = $this->getPath();
    $stat = array();
    $fields = array('mode' => true, 'size' => true, 'atime' => true, 'ctime' => true, 'mtime' => true);
    foreach ($this as $filename) {
      $results[$filename]['name'] = $filename;
      $stat = stat($path.'/'.$filename);
      $results[$filename]['mode'] = $stat['mode'];
      $results[$filename]['size'] = $stat['size'];
      $results[$filename]['atime'] = $stat['atime'];
      $results[$filename]['ctime'] = $stat['ctime'];
      $results[$filename]['mtime'] = $stat['mtime'];
      $results[$filename]['is_folder'] = (($results[$filename]['mode'] & 0170000) === 040000);
      if ($results[$filename]['is_folder'] === false) {
        $results[$filename]['sizeh'] = SH::toSize($stat['size']);
      }
      $results[$filename]['is_image'] = File::isImage($filename);
    }
    uasort($results, array($this, 'sortHash'));
    return $results;
  }

  private function sortHash($a, $b) {
    if (($a['is_folder'] !== $b['is_folder'] &&
        $a['is_folder'] === true) ||
        $a['name'] < $b['name']) {
      return -1;
    }
    return 1;
  }

  public function __destruct() {
    if ($this->fd !== false) {
      $ret = @closedir($this->fd);
      if ($ret === false) {
        S('log')->error('closedir() failure ['.$this->path.']', __METHOD__);
      }
    }
  }

  public static function create($path, $recursive=false) {
    $path = trim($path, ' /');
    $paths = array(DOC_PATH, ETC_PATH, PUB_PATH, USR_PATH, VAR_PATH, WEB_PATH);
    if (!in_array(substr($path, 0, 3), $paths) ||
        strlen($path) <= 3 ||
        strpos($path, '..') !== false) {
      S('log')->error('invalid path ['.$path.']');
      return ;
    }
    if (empty($path)) {
      throw new Exception('invalid path');
    }
    $ret = @mkdir($path, 0777, $recursive);
    if ($ret === false) {
      S('log')->error('mkdir() failure ['.$path.']', __METHOD__);
    }
    if (System::isLinux()) {
      Shell::chown($path);
    }
    return $ret;
  }

  public static function getInstance($path, $mode='r') {
    return new Folder($path, $mode);
  }

  public static function PathClean($path) {
    $path = SH::stripAccents($path);
    $path = strtolower($path);
    $path = str_replace('../', '/', $path);
    $path = preg_replace('#[^(0-9|a-z|/|\-|_)]#i', '', $path);
    $path = ltrim($path, '/- ');
    return $path;
  }

  public static function NameClean($name) {
    $name = SH::stripAccents($name);
    $name = strtolower($name);
    $name = preg_replace('/[^(0-9|a-z|\-|_)]/i', '', $name);
    $name = ltrim($name, '/- ');
    return $name;
  }

  public static function exists($path) {
    return is_dir($path);
  }

}
