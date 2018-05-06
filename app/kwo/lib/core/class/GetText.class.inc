<?php

class GetText
{
  const PATH = 'var/locales';

  private static $strings = array();
  private static $strings_back = array();

  public static function dict($code, $bindings=null, $locale=0) {
    $res = S('res');
    $locale = $locale >= 1 ? $locale : $res->getLocale();
    $prefix = substr($code, 0, 8);
    if ($prefix === 'snippet:') {
      $code = substr($code, 8);
      try {
        $snippet = Snippet::getInstance($code, array('ignore' => true));
      }
      catch (Exception $e) { return ''; }
      $content = $snippet->getProperty('content', $locale);
      if ($content === false) {
        S('log')->error('snippet ['.$code.']('.$locale.')', __METHOD__);
        $content = $code;
      }
    }
    else {
      if (self::$strings[$locale] === null) {
        $extension = P('app.extension');
        include self::PATH.'/messages_'.Locale::toCode($locale).'.inc';//include self::PATH.'/messages_'.I18N::locale2code($locale).'.inc';
        self::$strings[$locale] = $strings;
      }
      if (!isset(self::$strings[$locale][$code])) {
        $req = S('req');
        if (substr(P('app.domain'), -4) === '.kwo' && !$req->isCli()) {
          $string_id = String::id(array('code' => $code));
          if ($string_id) {
            $item = String::getInstance($string_id);
          }
          else {
            $item = String::getInstance(true);
            $about = $req->getScope().'/'.$req->getExtension().'/'.$req->getAction();
            $about .= ' ('.DH::now().')';
            $item->commit(array('code' => $code,
                                'app' => P('app.name'),
                                'locales' => $locale,
                                'type' => String::TYPE_FRONT,
                                'about' => $about,));
            $item->setFlag(String::FLAG_BATCH);
          }
          $content = $item->getProperty('content', $locale);
          if (empty($content)) {
            $content = $code.'('.Locale::toCode($locale).')';
            $item->setProperty('content', $content, $locale);
          }
          self::$strings[$locale][$code] = $content;
        }
        else {
          $path = WEB_PATH.'/'.S('app')->getAttribute('app.extension').'/files/strings.log';
          file_put_contents($path, $code.','.$locale."\n", FILE_APPEND|LOCK_EX);
        }
      }
      if ($session = S('req')->isTraced()) {
        $item = String::getInstance(array('code' => $code));
      }
      $content = self::$strings[$locale][$code];
      if ($content === null) {
        S('log')->error('unknown ['.$code.']('.$locale.')', __METHOD__);
        $content = '**'.$code.' ('.$locale.')**';
      }
    }
    if (is_array($bindings) && count($bindings) >= 1 && strlen($content) > 3) {
      $patterns = array();
      foreach ($bindings as $key => $value) {
        $patterns[] = '(('.$key.'))';
      }
      $content = str_replace($patterns, $bindings, $content);
    }
    return $content;
  }

  public static function backDict($code, $bindings=null, $locale=0) {
    $code = trim($code);
    if (empty($code)) return '';
    $res = S('res');
    $locale = BackManager::getLocale();

    $dict_file = self::PATH.'/messages_back_'.Locale::toCode($locale).'.inc';
    if (self::$strings_back[$locale] === null && file_exists($dict_file)) {
      include $dict_file;
      self::$strings_back[$locale] = $strings;
    }
    if (!isset(self::$strings_back[$locale][$code])) {
      $req = S('req');
      $args = array('code' => $code,
                    'type' => String::TYPE_BACK);
      $string_id = String::id($args);
      if ($string_id) {
        $item = String::getInstance($string_id);
      }
      else {
        $app = '';
        $ext = $req->getExtension();
        $about = '['.$ext.']';
        if ($xml_file = $res->getXmlFile()) {
          $about .= ' lib/'.$req->getExtension().'/xml/'.$xml_file.'.xml';
          //$app = ($ext == 'core' || $ext == 'devel') ? '' : $ext; // OK si éléments de coré déjà créés : ex. status -> état
        }
        else {
          if (defined('MODEL')) {
            $model = constant('MODEL');
            if (!empty($model) && $model != 'MODEL') {
              $about .= '['.$model.']';
            }
          }
          $about .= ' '.$req->getScope().'/'.$ext.'/'.$req->getAction();
        }
        $item = String::getInstance(true);
        $item->commit(array('code' => $code,
                            'about' => $about,
                            'app' => $app,
                            'locales' => I18N::LOCALE_FR,
                            'status' => String::STATUS_PENDING,
                            'type' => String::TYPE_BACK,
                            'used_at' => DH::now()));
        $item->setFlag(String::FLAG_BATCH);
      }
      $content = $item->getProperty('content', $locale);
      $content_fr = $item->getProperty('content', I18N::LOCALE_FR);
      if (empty($content)) {
        $content = $code.($locale == I18N::LOCALE_FR ? '' : '('.Locale::toCode($locale).')');
        $item->setProperty('content', $content, $locale);
      }
      if (empty($content_fr)) {
        $item->setProperty('content', $code, I18N::LOCALE_FR);
      }
      self::$strings_back[$locale][$code] = $content;
    }
    /*if ($session = S('req')->isTraced()) {
      $item = String::getInstance(array('code' => $code));
    }*/
    $content = self::$strings_back[$locale][$code];
    if ($content === null) {
      return $code.'('.Locale::toCode($locale).')';
    }
    if (is_array($bindings) && count($bindings) >= 1 && strlen($content) > 3) {
      $patterns = array();
      foreach ($bindings as $key => $value) {
        $patterns[] = '(('.$key.'))';
      }
      $content = str_replace($patterns, $bindings, $content);
    }
    return $content;
  }

  public static function reloadBackDict() {
    foreach ((array) P('back.admin.locales') as $locale) {
      include self::PATH.'/messages_back_'.Locale::toCode($locale).'.inc';
      self::$strings_back[$locale] = $strings;
    }
  }

}