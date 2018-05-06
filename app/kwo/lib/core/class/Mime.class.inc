<?php

class Mime
{
  public static function getIcon($filename) {
    $parts = explode('.', $filename);
    $type = self::$registry[array_pop($parts)];
    return $type === null ? 'file.png' : $type['icon'];
  }

  public static function getContentType($filename) {
    $parts_arr = explode('.', $filename);
    $type = self::$registry[array_pop($parts_arr)];
    return $type === null ? 'application/octet-stream' : $type['mime'];
  }

  public static $registry = 
  array(
    'aac' => array('mime' => 'video/mp4', 'icon' => 'video.png'),
    'aiff' => array('mime' => 'audio/x-aiff', 'icon' => 'video.png'),
    'avi' => array('mime' => 'video/x-msvideo', 'icon' => 'video.png'),
    'bmp' => array('mime' => 'image/bmp', 'icon' => 'img.png'),
    'class' => array('mime' => 'application/octet-stream', 'icon' => 'java.png'),
    'css' => array('mime' => 'text/css', 'icon' => 'html.png'),
    'csv' => array('mime' => 'text/csv', 'icon' => 'csv.png'),
    'doc' => array('mime' => 'application/vnd.ms-word', 'icon' => 'doc.png'),
    'docx' => array('mime' => 'application/vnd.openxmlformats-officedocument.wordprocessingml.document', 
                    'icon' => 'doc.png'),
    'emf' => array('mime' => 'image/x-emf', 'icon' => 'image.png'),
    'eml' => array('mime' => 'message/rfc822', 'icon' => 'email.png'),
    'emlx' => array('mime' => 'message/rfc822', 'icon' => 'email.png'),
    'fla' => array('mime' => 'text/plain', 'icon' => 'flash.png'),
    'flac' => array('mime' => 'audio/flac', 'icon' => 'video.png'),
    'gif' => array('mime' => 'image/gif', 'icon' => 'img.png'),
    'gz'  => array('mime' => 'application/x-gzip', 'icon' => 'compress.png'),
    'html' => array('mime' => 'text/html', 'icon' => 'html.png'),
    'ico' => array('mime' => 'image/vnd.microsoft.icon', 'icon' => 'img.png'),
    'java' => array('mime' => 'text/plain', 'icon' => 'java.png'),
    'jpeg' => array('mime' => 'image/jpeg', 'icon' => 'img.png'),
    'jpg' => array('mime' => 'image/jpg', 'icon' => 'img.png'),
    'js'  => array('mime' => 'application/javascript', 'icon' => 'html.png'),
    'json' => array('mime' => 'application/json', 'icon' => 'html.png'),
    'mov' => array('mime' => 'video/quicktime', 'icon' => 'video.png'),
    'mp3' => array('mime' => 'audio/mpeg', 'icon' => 'video.png'),
    'mpeg' => array('mime' => 'video/mpeg', 'icon' => 'video.png'),
    'mpg' => array('mime' => 'video/mpeg', 'icon' => 'video.png'),
    'msg' => array('mime' => 'message/rfc822', 'icon' => 'email.png'),
    'ogg' => array('mime' => 'audio/ogg', 'icon' => 'video.png'),
    'pdf' => array('mime' => 'application/pdf', 'icon' => 'pdf.png', 'magic' => "\\x25\\x50\\x44\\x46\\x2D"),
    'php' => array('mime' => 'text/plain', 'icon' => 'psp.png'),
    'png' => array('mime' => 'image/png', 'icon' => 'img.png'),
    'ppt' => array('mime' => 'application/ms-powerpoint', 'icon' => 'doc.png'),
    'pptx' => array('mime' => 'application/vnd.openxmlformats-officedocument.presentationml.presentation',
                    'icon' => 'doc.png'),
    'psp' => array('mime' => 'text/plain', 'icon' => 'psp.png'),
    'rar' => array('mime' => 'application/x-rar-compressed', 'icon' => 'compress.png'),
    'rss' => array('mime' => 'text/xml', 'icon' => 'xml.png'),
    'rtf' => array('mime' => 'text/rtf', 'icon' => 'doc.png'),
    'svg' => array('mime' => 'image/svg+xml', 'icon' => 'xml.png'),
    'swf' => array('mime' => 'application/x-shockwave-flash', 'icon' => 'flash.png'),
    'tgz' => array('mime' => 'application/x-gzip', 'icon' => 'compress.png'),
    'tiff' => array('mime' => 'image/tiff', 'icon' => 'img.png'),
    'txt' => array('mime' => 'text/plain', 'icon' => 'file.png'),
    'wav' => array('mime' => 'audio/x-wav', 'icon' => 'video.png'),
    'wmf' => array('mime' => 'image/x-wmf', 'icon' => 'image.png'),
    'xbl' => array('mime' => 'text/xml', 'icon' => 'xml.png'),
    'xls' => array('mime' => 'application/vnd.ms-excel', 'icon' => 'csv.png'),
    'xlsx' => array('mime' => 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet', 'icon' => 'csv.png'),
    'xml' => array('mime' => 'text/xml', 'icon' => 'xml.png'),
    'xul' => array('mime' => 'application/vnd.mozilla.xul+xml', 'icon' => 'xml.png'),
    'zip' => array('mime' => 'application/zip', 'icon' => 'compress.png')
  );



}