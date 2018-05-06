<?php

/***

IMAGETYPE_GIF
IMAGETYPE_JPEG
IMAGETYPE_PNG
IMAGETYPE_SWF
IMAGETYPE_PSD
IMAGETYPE_BMP
IMAGETYPE_TIFF_II
IMAGETYPE_TIFF_MM
IMAGETYPE_JPC
IMAGETYPE_JP2
IMAGETYPE_JPX
IMAGETYPE_JB2
IMAGETYPE_SWC
IMAGETYPE_IFF
IMAGETYPE_WBMP
IMAGETYPE_XBM

***/

class Img extends File
{

  /* qualtiy png : 0 -> 9 */
//  const QUALITY = 90;
  const QUALITY = 90;

  protected $height = null;
  protected $mime   = null;
  protected $width  = null;

  public function __construct($filename, $mode='r') {
    $this->img_flag = true;
    parent::__construct($filename, $mode);
    $this->loadInfos();
  }

  public function loadInfos() {
    $h = getimagesize($this->getName());
    if ($h === false) {
      S('log')->error($this->getName(), __METHOD__);
      throw new FileSystemException('invalid image');
    }
    $this->width = $h[0];
    $this->height = $h[1];
    $this->type = $h[2];
    $this->mime = $h['mime'];
    if ($this->width <= 0 || $this->height <= 0) {
      S('log')->error('invalid dimensions ['.$this->getName().']', __METHOD__);
      throw new FileSystemException('invalid image');
    }
  }

  public function getWidth() {
    return $this->width;
  }

  public function getHeight() {
    return $this->height;
  }

  public function setWidth($width, $preserve_ratio_flag=false) {
    $height = $this->getHeight();
    if ($preserve_ratio_flag === true) {
      $height = ceil($height * ($width / $this->getWidth()));
    }
    return $this->setSize($width, $height);
  }

  public function setHeight($height, $preserve_ratio_flag=false) {
    $width = $this->getWidth();
    if ($preserve_ratio_flag === true) {
      $width = ceil($width * ($height / $this->getHeight()));
    }
    return $this->setSize($width, $height);
  }

  public function setCropBox($width, $height) {
    $w = $this->getWidth();
    $h = $this->getHeight();
    if ($w <= $width && $h <= $height) {
      return $this;
    }
    if ($w < $width || $h < $height) {
      return $this->setBox($width, $height);
    }
    if ($w > $h) {
      $w = ceil($width * ($w / $h));
      $h = $height;
      return $this->setSize($w, $h)->crop(ceil(($w-$h)/2), 0, $width, $height);
    }
    $h = ceil($height * ($h / $w));
    $w = $width;
    return $this->setSize($w, $h)->crop(0, ceil(($h-$w)/2), $width, $height);
  }

  public function setBox($width, $height=0) {
    $height = $height < 1 ? $width : $height;
    $w = $this->getWidth();
    $h = $this->getHeight();
    if ($w <= $width && $h <= $height) {
      return $this;
    }
    $ratio = $w >= $h ? ($width / $w) : ($height / $h);
    $width = ceil($w * $ratio);
    $height = ceil($h * $ratio);
    return $this->setSize($width, $height);
  }

  public function setSize($width, $height=0) {
    $width = (int) $width;
    $height = (int) $height;
    $height = $height < 1 ? $width : $height;
    if ($width < 1 || $height < 1) {
      S('log')->error($this->getName().' ['.$width.'x'.$height.']', __METHOD__);
      throw new FileSystemException('invalid dimensions');
    }
    $img = $this->load();
    $dest = @imagecreatetruecolor($width, $height);
    if ($dest === false) {
      S('log')->error('imagecreatetruecolor ['.$this->getName().']', __METHOD__);
      throw new FileSystemException('setSize failure (1)');
    }
    if ($this->mayHaveAlpha()) {
      $index = imagecolortransparent($img);
      if ($index >= 0) {
        $color = imagecolorsforindex($img, $index);
        imagecolortransparent($dest,
                              imagecolorallocate($dest,
                                                 $color['red'],
                                                 $color['green'],
                                                 $color['blue']));
      }
      imagealphablending($dest, false);
      imagesavealpha($dest, true);
    }
    $ret = imagecopyresampled($dest, $img,
                              0, 0, 0, 0,
                              $width, $height,
                              $this->getWidth(), $this->getHeight());
    if ($ret === false) {
      S('log')->error($this->getName(), __METHOD__);
      throw new FileSystemException('setSize failure (2)');
    }
    $ret = $this->_convert($dest);
    $ret = imagedestroy($img);
    if ($ret === false) {
      S('log')->error('destroy (1) ['.$this->getName().']', __METHOD__);
      throw new FileSystemException('setSize failure (3)');
    }
    $ret = imagedestroy($dest);
    if ($ret === false) {
      S('log')->error('destroy (2) ['.$this->getName().']', __METHOD__);
      throw new FileSystemException('setSize failure (4)');
    }
    $this->loadInfos();
    return $this;
  }

  public function rename($name) {
    $type = self::imgType($name);
    if ($type === null) {
      S('log')->error('bad extension type', __METHOD__);
      throw new FileSystemException('bad extension type');
    }
    $this->checkMemory();
    parent::rename($name);
    if ($type != $this->type) {
      switch ($this->type) {
      case IMAGETYPE_GIF: $img = imagecreatefromgif($this->getName()); break ;
      case IMAGETYPE_JPEG: $img = imagecreatefromjpeg($this->getName()); break ;
      case IMAGETYPE_PNG: $img = imagecreatefrompng($this->getName()); break ;
      }
      if ($img === false) {
        S('log')->error('failure ['.$this->getName().']', __METHOD__);
        throw new FileSystemException('unable to load image');
      }
      if ($this->mayHaveAlpha()) {
        imagealphablending($img, false);
        imagesavealpha($img, true);
      }

      $dest = imagecreatetruecolor($this->width, $this->height);
      $white = imagecolorresolve($img, 255, 255, 255);
      imagefill($dest, 0, 0, $white);
      imagecopy($dest, $img, 0, 0, 0, 0, $this->width, $this->height);

      switch ($type) {
        case IMAGETYPE_GIF: $ret = imagegif($dest, $this->getName()); break ;
        case IMAGETYPE_JPEG: $ret = imagejpeg($dest, $this->getName(), self::QUALITY); break ;
        case IMAGETYPE_PNG: $ret = imagepng($dest, $this->getName()); break ;
      }
      if ($ret === false) {
        S('log')->error('unable to store img data', __METHOD__);
        throw new FileSystemException('unable to store img data');
      }
      $this->loadInfos();
    }
    return $this;
  }

  public function setType($type, $quality=self::QUALITY) {
    $this->_convert($this->load(), $quality, $type);
    return $this;
  }

  private function checkMemory() {
    $h = getimagesize($this->getName());
    $mem_req = ceil(($h[0] * $h[1] * ($h['bits'] / 8) * $h['channels'] * 2.5) / 1024);
    $mem_lim = substr(ini_get('memory_limit'), 0, -1) * 1048576;
    if ($mem_req > $mem_lim) {
      S('log')->trace('['.$this->getName().'] memory_limit is too low. '.
                      'Preferred value: '.ceil($mem_req / 1048576).'M');
    }
  }

  private function load() {

    $this->checkMemory();

    $img = false;
    switch ($this->type) {
      case IMAGETYPE_GIF:  $img = imagecreatefromgif($this->getName()); break ;
      case IMAGETYPE_JPEG: $img = imagecreatefromjpeg($this->getName()); break ;
      case IMAGETYPE_PNG: $img = imagecreatefrompng($this->getName()); break ;
    }
    if ($img === false) {
      S('log')->error('failure ['.$this->getName().']', __METHOD__);
      throw new FileSystemException('unable to load image');
    }
    if ($this->mayHaveAlpha()) {
      imagealphablending($img, false);
      imagesavealpha($img, true);
    }
    return $img;
  }

  public function mayHaveAlpha() {
    return in_array($this->type, array(IMAGETYPE_GIF, IMAGETYPE_PNG));
  }

  public function _convert($img, $quality=self::QUALITY, $type=null) {
    $type = $type ? $type : $this->type;
    $ret = false;
    switch ($type) {
      case IMAGETYPE_GIF:  $ret = imagegif($img, $this->getName()); break;
      case IMAGETYPE_JPEG: $ret = imagejpeg($img, $this->getName(), $quality); break;
      case IMAGETYPE_PNG:  $ret = imagepng($img, $this->getName()); break;
        /*      $ret = imagepng($img, $this->getName());
                imagealphablending($img, false);
      imagesavealpha($img, true);
      break;*/
    }
    if ($ret !== false) $this->loadInfos();
    return $ret;
  }

  public function rotate($angle, $quality=self::QUALITY) {
    $img = $this->load();
    $img = imagerotate($img, $angle, 0);
    if ($img === false) {
      S('log')->error('rotate error ['.$this->getName().']',
                      __METHOD__);
    }
    $ret = $this->_convert($img, $quality);
    $ret = imagedestroy($img);
    if ($ret === false) {
      S('log')->error('destroy error ['.$this->getName().']',
                      __METHOD__);
    }
    return $this;
  }

  public function setCropRatio($ratio) {
    if (is_string($ratio) && strpos($ratio, '/') > 0) {
      list ($w, $h) = explode('/', $ratio);
      $ratio = $w / $h;
    }
    $ratio = (float) $ratio;
    $w = $this->getWidth();
    $h = $this->getHeight();
    $ar = $w / $h;
    if ($ar == $ratio) return $this;
    if ($ratio >= 1) {
      if ($ar > $ratio) {
        $y = 0; $height = $h;
        $width = $height * $ratio;
        $x = floor(($w - $width) / 2);
      }
      else {
        $x = 0; $width = $w;
        $height = $width / $ratio;
        $y = floor(($h - $height) / 2);
      }
    }
    else {
      if ($ar > $ratio) {
        $y = 0; $height = $h;
        $width = $height / $ratio;
        $x = floor(($w - $width) / 2);
      }
      else {
        $x = 0; $width = $w;
        $height = $width * $ratio;
        $y = floor(($h - $height) / 2);
      }
    }
    return $this->crop($x, $y, $width, $height);
  }

  public function crop($x, $y, $width, $height) {
    $img = $this->load();
    /*    if ($x1 > $x2) {
      $width = $x1 - $x2;
      $x = $x2;
    }
    else {
      $width = $x2 - $x1;
      $x = $x1;
    }
    if ($y1 > $y2) {
      $height = $y1 - $y2;
      $y = $y2;
    }
    else {
      $height = $y2 - $y1;
      $y = $y1;
      }*/
    if ($width <= 0 || $height <= 0) return $this;
    $dest = imagecreatetruecolor($width, $height);
    imagecopy($dest, $img, 0, 0, $x, $y, $width, $height);
    $ret = $this->_convert($dest);
    imagedestroy($img);
    imagedestroy($dest);
    $this->loadInfos();
    return $this;
  }

  public static function imgType($name) {
    $types = array('JPG' => IMAGETYPE_JPEG,
                   'JPEG' => IMAGETYPE_JPEG,
                   'GIF' => IMAGETYPE_GIF,
                   'PNG' => IMAGETYPE_PNG);
    $parts = explode('.', basename($name));
    $extension = array_pop($parts);
    return $types[strtoupper($extension)];
  }

  public static function getInstance($path, $mode='r') {
    return new Img($path, $mode);
  }

}
