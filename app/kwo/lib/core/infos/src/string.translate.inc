<?php

return ;

$reference_locale = strlen($req->reference) === 2 ? $req->reference : 'fr';

$locales = array();
if (strlen($req->locales.$req->locale) >= 2) {
  $locales = (array) explode(',', $req->locales.$req->locale);
}
$locales = empty($locales) ? array('en') : $locales;

if (in_array($reference_locale, $locales)) {
  throw new Exception('bad args');
}

$extensions = null;
if ($req->hasAttribute('extensions') || $req->hasAttribute('extension')) {
  $extensions = $req->hasAttribute('extensions')
              ? explode(',', $req->extensions)
              : (array) $req->extension; 
}

foreach (glob(WEB_PATH.'/*/locales/*_'.$reference_locale.'.inc') as $from_path) {
  list (, $extension, ) = explode('/', $from_path);
  if (is_array($extensions) && !in_array($extension, $extensions)) continue ;
  $words = null;
  include($from_path);
  if (empty($words)) continue;
  if (!Shell::confirm("\n".'Traduire <'.$from_path.'>', true)) continue;
  ksort($words);
  $src = $words;
  foreach ($locales as $locale) {
    $to_path = str_replace('_fr', '_'.$locale, $from_path);
    if (!file_exists($to_path)) touch($to_path);
    include($to_path);
    $words = (array) $words;
    foreach ($src as $key => $value) {
      if (empty($words[$key])) {
        $translation = Shell::prompt(' - traduire en '.strtoupper($locale).' <'.$key.'>', $value);
        $words[$key] = $translation;
      }
    }
    ksort($words);
    $extra_keys = array();
    foreach ($words as $key => $value) {
      if ($src[$key] === null) $extra_keys[] = $key;
    }
    if (count($extra_keys) >= 1 && 
        Shell::confirm("\n".' ! supprimer les clefs '.strtoupper($locale)
                       .' ('.join(',', $extra_keys).')')) {
      foreach ($extra_keys as $key) {
        unset($words[$key]);
      }
    }
    $php = '<?php'."\n".'$words = '.var_export($words, true).';';
    file_put_contents($to_path, $php);
  }
  $php = '<?php'."\n".'$words = '.var_export($src, true).';';
  file_put_contents($from_path, $php);
  $src = null;
}
