<?php

try {

  $files = glob(GetText::PATH.'/messages_*.inc');

  foreach ($files as $file) {
    if (strpos($file, '_fr.inc') > 1) $locale = Locale::FR;
    elseif (strpos($file, '_en.inc') > 1) $locale = Locale::EN;
    else continue ;
    $words = array();
    include $file;
    list (, $extension,,) = explode('/', $file);
    $app = '';
    if (Extension::id($extension) >= Extension::THRESHOLD) {
      $app = $extension;
    }
    if (empty($strings)) continue ;
    foreach ($strings as $word => $value) {
      //      if ($word != 'first_name') continue ;
      if (empty($value)) continue ;
      $string_id = String::id(array('name' => $word));
      if ($string_id > 0) {
        $string = String::getInstance($string_id);
      }
      else {
        $string = String::getInstance();
        try {
          $string->commit(array('name' => $word,
                                'status' => 1,
                                'app' => $app));
        }                      
        catch (Exception $e) { 
          $log->trace($e);
          continue;
        }
      }
      if (($string->locales & $locale) < 1) {
        $string->commit(array('locales' => $string->locales | $locale));
      }
      $string->setProperty('content', $value, $locale);
      //$string->syncAttributes();
    }
  }

}
catch (Exception $e) { $err->add($e); }
