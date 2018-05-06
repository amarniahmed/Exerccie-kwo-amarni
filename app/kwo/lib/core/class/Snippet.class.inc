<?php

class Snippet extends ActiveRecord
{
  const STATUS_DRAFT = 0;
  const STATUS_PENDING = 10;
  const STATUS_ONLINE = 20;

  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_PENDING => 'en ligne, à revoir',
                                  self::STATUS_DRAFT => 'brouillon');

  public static $colors = array(self::STATUS_ONLINE => 'green',
                                self::STATUS_PENDING => 'blue',
                                self::STATUS_DRAFT => 'red');

  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['code'])) {
      $attrs['name'] = $attrs['code'];
    }
  }

  public static function apps() {
    $rs = S('dbo')->query('SELECT DISTINCT app'
                          .' FROM '.T('snippet')
                          .' WHERE LENGTH(app)>0');
    $h = array();
    foreach ($rs as $row) {
      $h[$row['app']] = $row['app'];
    }
    asort($h);
    return $h;
  }

  public function asShortCodeContent($args=null) {
    $locale = S('res')->getLocale();
    return $this->getProperty('content', $locale);
  }

  public static function get($code) {
    $opts = array('locale' => S('res')->getLocale());
    return Snippet::getInstance($code, $opts);
  }

  public static function content($code, $field='content', $bindings=null) {
    $locale = S('res')->getLocale();
    $msg = null;
    try {
      $opts = array('ignore' => true,
                    'locale' => $locale);
      $snippet = Snippet::getInstance($code, $opts);
      $msg = $snippet->getAttribute($field);
    }
    catch (Exception $e) {  }
    if ($msg === false || $msg === null) {
      S('log')->error('snippet ['.$code.'] ('.$locale.')', __METHOD__);
      return $code;
    }
    $patterns = array();
    $bindings = (array) $bindings;
    foreach ($bindings as $key => $value) {
      $patterns[] = '(('.$key.'))';
    }
    $msg = str_replace($patterns, $bindings, $msg);
    return $msg;
  }

}