<?php

// extensions autorisées :
// jpg, jpeg, png, gif, pdf, doc, ppt, odt, mp3,
// mp4, m4a, wav, ogg, mov, wmv, avi, mpg, txt

class UploadManager
{
  const ERROR_FILE_EXISTS = 2;
  const ERROR_EXTENSION = 3;

  public static function anonymous($name, $opts=null) {
    $opts = (array) $opts;
    $filename = $_FILES[$name]['name'];
    $chunks = explode('/', $filename);
    $filename = array_pop($chunks);
    if (P('app.upload.anonymous.size') && $_FILES[$name]['size'] > P('app.upload.anonymous.size')) {
      S('log')->error('file size ('.$_FILES[$name]['size'].')', __METHOD__);
      throw new Exception(l('error.upload.filesize'));
    }

    $opts['desination'] = $opts['desination']
                        ? trim($opts['desination'], '/')
                        : self::path();

    if (!is_writable($opts['desination'])) {
       S('log')->error('directory not writable ['.$opts['desination'].']',
                       __METHOD__);
      throw new Exception(l('error.upload.move'));
    }

    $filename = File::NameClean($filename);

    if (!File::isAllowedName($filename)) {
      if (file_exists($_FILES[$name]['tmp_name']) &&
          strlen($_FILES[$name]['tmp_name']) >= 6) {
        unlink($_FILES[$name]['tmp_name']);
      }
      S('log')->error('invalid file name ['.$filename.']', __METHOD__);
      throw new Exception(l('error.upload.extension'));
    }

    if (!is_uploaded_file($_FILES[$name]['tmp_name']) ||
        strpos($_FILES[$name]['tmp_name'], self::path()) === false ||
        $_FILES[$name]['error'] != UPLOAD_ERR_OK) {
      S('log')->error('upload failure ['.$filename.']'
                      .' ('.$_FILES[$name]['error'].')',
                      __METHOD__);
      throw new Exception(l('error.upload.transfer'));
    }

    $dest = $opts['desination'].'/'.$filename;
    if (file_exists($dest)) {
      $dest = $opts['desination'].'/'.time().'-'.$filename;
    }

    if (@move_uploaded_file($_FILES[$name]['tmp_name'], $dest) === false) {
      S('log')->error('move failure ['.$filename.']', __METHOD__);
      throw new Exception(l('error.upload.move'));
    }

    $file = File::getInstance($dest);

    S('app')->hook('onUpload', array('file' => $file));
    S('app')->hook('onAnonymousUpload', array('file' => $file));

    if (P('app.upload.anonymous.box') && File::isImage($file)) {
      list ($width, $height) = explode('x', P('app.upload.anonymous.box'));
      $height = $height ?: $width;
      try {
        $file->getImg()->setBox($width, $height);
      }
      catch (Exception $e) {
        $file->unlink();
        throw new Exception(l('error.image'));
      }
    }
    return $file;
  }

  public static function user($user, $name, $opts=null) {
    $path = $user->getPath();
    $filename = $_FILES[$name]['name'];
    $path = rtrim($path, '/');

    $filename = File::NameClean($filename);

    if (!File::isAllowedName($filename)) {
      if (file_exists($_FILES[$name]['tmp_name']) &&
          strlen($_FILES[$name]['tmp_name']) >= 6) {
        unlink($_FILES[$name]['tmp_name']);
      }
      S('log')->error('invalid file name ['.$filename.']', __METHOD__);
      throw new Exception(l('error.upload.extension'));
    }

    if (P('app.upload.user.size') && $_FILES[$name]['size'] > P('app.upload.user.size')) {
      S('log')->error('file size ('.$_FILES[$name]['size'].')', __METHOD__);
      throw new Exception(l('error.upload.filesize'));
    }

    if (!is_uploaded_file($_FILES[$name]['tmp_name']) ||
        $_FILES[$name]['error'] != UPLOAD_ERR_OK) {
      S('log')->error('upload failure ['.$filename.']'
                      .' ('.$_FILES[$name]['error'].')',
                      __METHOD__);
      throw new Exception(l('error.upload.transfer'));
    }

    $dest = $path.'/'.$filename;
    if (!move_uploaded_file($_FILES[$name]['tmp_name'], $dest)) {
      S('log')->error('move failure ['.$filename.']', __METHOD__);
      throw new Exception(l('error.upload.move'));
    }

    S('app')->hook('onUserUpload', array('filename' => $dest));

    $file = File::getInstance($dest);

    S('app')->hook('onUpload', array('file' => $file));
    S('app')->hook('onUserUpload', array('file' => $file));

    if (P('app.upload.user.box') && File::isImage($file)) {
      list ($width, $height) = explode('x', P('app.upload.user.box'));
      $height = $height ?: $width;
      try {
        $file->getImg()->setBox($width, $height);
      }
      catch (Exception $e) {
        $file->unlink();
        throw new Exception(l('error.image'));
      }
    }
    return $file;
  }

  public static function back($dir) {
    $name = 'remote_file_path';
    $filename = $_FILES[$name]['name'];
    $filename = File::NameClean($filename);
    if (File::isMaliciousName($filename)) {
      throw new Exception("extension de fichier non authorisée");
    }
    if (!is_uploaded_file($_FILES[$name]['tmp_name']) ||
        $_FILES[$name]['error'] != UPLOAD_ERR_OK) {
      throw new Exception('le fichier n’a pu être envoyé');
    }
    $filepath = $dir->getPath().'/'.$filename;
    if (!move_uploaded_file($_FILES[$name]['tmp_name'], $filepath)) {
      throw new Exception('déplacement du fichier impossible');
    }
    return $filepath;
  }

  public static function path() {
    return TMP_PATH;
  }

  public static function clean() {
    if (strlen(self::path()) <= 5 || !is_dir(self::path())) return false;
    $files = glob(self::path().'/*');
    $files = $files ?: array();
    $ts = time();
    $ttl = 4 * 24 * 60 * 60;
    foreach ($files as $file) {
      if (!file_exists($file) || $file[0] === '.') continue ;
      $fts = filemtime($file);
      if (($ts - $fts) > $ttl) {
        unlink($file);
      }
    }
  }

}