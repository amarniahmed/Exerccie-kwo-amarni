<?php

try {

  $app = Shell::prompt('App ?', S('app')->getName());

  $app = App::getInstance($app);

  $file = WEB_PATH.'/'.$app->getAttribute('app.extension').'/files/strings.log';
  if (!file_exists($file)) {
    return $res->write('string file not found');
  }

  $locales = array();
  foreach ($app->getAttribute('app.locales') as $locale) {
    $locales[] = Locale::toCode($locale);//I18N::locale2code($locale);
  }
  $locales = Shell::prompt('Locales ?', join(',', $locales));

  $set = SH::asSet($locales);
  $locales = array();
  foreach ($set as $locale) {
    $locales[] = Locale::toNum($locale);//$locales[] = I18N::locale2num($locale);
  }

//  print_r($locales);

  $set = $dbo->asSet('SELECT code FROM '.T('string'));
  $strings = array();
  $fd = fopen($file, 'r');
  while ($s = fgets($fd)) {
    $s = trim($s);
    list ($code, $locale) = explode(',', $s);
    if (in_array($code, $set)) continue ;
    $strings[$code]++;
  }
  fclose($fd);

  ksort($strings);

  print($file."\n");

  print_r($strings);

  if (!Shell::confirm('ceate those strings', true)) return ;

  $attrs = array('app' => $app->getName(),
                 'code' => '',
                 'name' => '',
                 'flags' => String::FLAG_BATCH,
                 'status' => String::STATUS_PENDING);
  foreach ($strings as $code => $n) {
    $string = String::getInstance();
    $attrs['code'] = $attrs['name'] = $code;
    print_r($attrs);
/*    $string->commit($attrs);
    foreach ($locales as $locale) {
      $string->setProperty('content', $code.'('.$locale.')');
    } */
  }

  if (Shell::confirm('reset file', true)) {
    unlink($file);
  }

}
catch (Exception $e) { $err->add($e); }
