<?php

class Imagick {

  const ICC_RGB_PATH = 'lib/core/opt/icc/sRGB.icc';
  const ICC_CMYK_PATH = 'lib/core/opt/icc/CoatedFOGRA27.icc';



  // Resolution ----------------------------------------------------------------

  public static function identify($image) {
    $image = is_object($image) ? $image : Img::getInstance($image);
    $res = Shell::exec('identify -format "%x x %y" '.$image->getName());
    return $res['last_line'];
  }

  public static function getDpi($image) {
    $format = self::identify($image);
    list($value, $unit) = explode(' ', $format);
    switch ($unit) {
      case 'PixelsPerInch': $dpi = ceil($value); break;
      case 'PixelsPerCentimeter': $dpi = round($value * 2.54); break;
      default : $dpi = round($value);
    }
    return $dpi;
  }

  public static function getResolutionUnit($image) {
    $format = self::identify($image);
    list($value, $unit) = explode(' ', $format);
    return trim($unit);
  }

  public static function setResolutionUnit($image, $unit='PixelsPerInch') {
    $image = is_object($image) ? $image : Img::getInstance($image);
    Shell::exec('convert -units '.$unit.' '.$image->getName().' '.$image->getName());
  }

  // Utilities -----------------------------------------------------------------

  public static function resize($image, $width, $height) {
    $image = is_object($image) ? $image : Img::getInstance($image);
    Shell::exec('mogrify -resize '.$width.'x'.$height.' '.$image->getName());
    $image->loadInfos();
    return $image;
  }

  public static function resample($image, $dpi) {
    $unit = self::getResolutionUnit($image);
    if ($unit == 'Undefined') {
       // resample en dpi en admettant le fait que l'unité est en dpi
      Shell::exec('mogrify -resample '.$dpi.'x'.$dpi.' '.$image->getName());
    }
    else {
      if ($unit == 'PixelsPerCentimeter') {
        self::setResolutionUnit($image, 'PixelsPerInch');
      }
      Shell::exec('mogrify -units PixelsPerInch -resample '.$dpi.'x'.$dpi.' '.$image->getName());
    }
    $image->loadInfos();
    return $image;
  }

  public static function crop($image, $width, $height, $x, $y) {
    $image = is_object($image) ? $image : Img::getInstance($image);
    $x = ($x >= 0 ? '+' : '-').abs($x);
    $y = ($y >= 0 ? '+' : '-').abs($y);
    Shell::exec('convert -crop '.$width.'x'.$height.$x.$y
                .' +repage '.$image->getName()
                .' '.$image->getName());
    $image->loadInfos();
    return $image;
  }

  public static function border($image, $h_width=0, $v_width=0, $color) {
    $image = is_object($image) ? $image : Img::getInstance($image);
    Shell::exec('convert -border '.$h_width.'x'.$v_width
                .' -bordercolor '.$color
                .' -background '.$color
                .' '.$image->getName()
                .' '.$image->getName());
    $image->loadInfos();
    return $image;
  }

  public static function convert($file, $ext) {
    $file = is_object($file) ? $file : Img::getInstance($file);
    $basename = basename($file->getBaseName(), '.'.$file->getExtension()).'.'.$ext;
    $output = $file->getPath().'/'.$basename;
    Shell::exec('convert '.$file->getName()
                .(strtolower($file->getExtension()) == 'pdf' ? '[0]' : '')
                .' '.$output);
    return $output;
  }

  public static function convertPdf($file, $ext) {
    $file = is_object($file) ? $file : Img::getInstance($file);
    $basename = basename($file->getBaseName(), '.'.$file->getExtension()).'.'.$ext;
    $output = $file->getPath().'/'.$basename;
    $cmd = 'convert -density 144 '.$file->getName().'[0]'.' -resize 50% '.$output;
    $ret = 0;
    system($cmd, $ret);
    return $output;
  }

  public static function fixUndefinedResolution($image) {
    $dpi = self::getDpi($image);
    if ($dpi >= 72) return;
    $canvas = imagecreatetruecolor($image->getWidth(), $image->getHeight());
    $bkg = imagecolorallocate($canvas, 255, 255, 255);
    imagefill($canvas, 0, 0, $bkg);

    switch (Img::imgType($image->getName())) {
      case IMAGETYPE_JPEG : $option_img = imagecreatefromjpeg($image->getName()); break;
      case IMAGETYPE_GIF : $option_img = imagecreatefromgif($image->getName()); break;
      case IMAGETYPE_PNG : $option_img = imagecreatefrompng($image->getName()); break;
    }
    imagecopy($canvas, $option_img, 0, 0, 0, 0, $image->getWidth(), $image->getHeight());
    imagejpeg($canvas, $image->getName(), 100);
    $out = Img::getInstance($image->getName());
    self::setResolutionUnit($out, 'PixelsPerInch');
    return $out;
  }

  // Colorimetry ---------------------------------------------------------------

  public static function isCmyk($image) {
    $image = is_object($image) ? $image : Img::getInstance($image);
    $res = Shell::exec('identify -format "%r" '.$image->getName());
    return preg_match('/CMYK/', $res['last_line']);
  }

  public static function rgb2cmyk($image) {
    $image = is_object($image) ? $image : Img::getInstance($image);
    $cmd = 'convert '.$image->getName()
           .' +profile icm'
           .' -profile '.self::ICC_RGB_PATH
           .' -profile '.self::ICC_CMYK_PATH
           .' '.$image->getName();
    Shell::exec($cmd);
    $image->loadInfos();
    return $image;
  }

  public static function cmyk2rgb($image) {
    $image = is_object($image) ? $image : Img::getInstance($image);
    Shell::exec('convert '.$image->getName()
                .' +profile icm'
                .' -profile '.self::ICC_CMYK_PATH
                .' -profile '.self::ICC_RGB_PATH
                .' '.$image->getName());
    $image->loadInfos();
    return $image;
  }

}