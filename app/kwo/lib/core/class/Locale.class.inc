<?php

/**
https://support.google.com/webmasters/answer/189077?hl=en
**/

class Locale extends ActiveRecord {

  const FR = 1;
  const EN = 2;

  //-----------
  const ES = 4;
  const IT = 8;
  const PT = 16;
  const DE = 32;

  const STATUS_OFFLINE = -1;
  const STATUS_ONLINE = 1;

  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_OFFLINE => 'hors ligne');

  protected function onBeforeCommit(&$attrs) {
    // TODO : vérifier si cette locale est utilisée dans une des apps
    //        interdire la suppression des bits <= 6
    //        interdire que 2 bits soient identiques
    if ($this->isItem() && $this->bit <= 6 && $attrs['bit'] > 0 && $attrs['bit'] != $this->bit) {
      throw new ActiveRecordException('Locale protégée, ce bit ne peut être modifié');
    }
  }

  protected function onAfterCommit($attrs) {
    ConfigManager::DBCacheBuild();
  }

  protected function onAfterDestroy() {
    ConfigManager::DBCacheBuild();
  }

  public static function asCache() {
    $rs = S('dbo')->query('SELECT bit, code, name, native'
                          .' FROM '.T('locale')
                          .' WHERE status>0');
    $parameters = array();
    foreach ($rs as $row) {
      $row['bit'] = pow(2, ($row['bit'] - 1));
      $parameters[$row['bit']] = $row;
    }
    return $parameters;
  }

  public static function toNum($locale) {
    self::asHash();
    if ($locale == 'fr') return 1;
    if (is_numeric($locale)) return (int) $locale;
    if (!isset(self::$code2bit[$locale])) {
      S('log')->error('invalid locale ['.$locale.']', __METHOD__);
    }
    $bit = (int) self::$code2bit[$locale];
    return ($bit < 1 ? self::FR : $bit);
  }

  public static function toCode($locale) {
    self::asHash();
    if ($locale == 1) return 'fr';
    if (!is_numeric($locale)) return $locale;
    $locale = (int) $locale;
    if (!isset(self::$locales[$locale]['code'])) {
      S('log')->error('invalid locale ('.$locale.')', __METHOD__);
    }
    $code = self::$locales[$locale]['code'];
    return (strlen($code) <= 1 ? 'fr' : $code);
  }

  public static function toName($locale) {
    self::asHash();
    $locale = self::toNum($locale);
    if (!isset(self::$locales[$locale]['name'])) {
      S('log')->error('invalid locale ('.$locale.')', __METHOD__);
    }
    return self::$locales[$locale]['name'];
  }

  private static $locales = null;
  private static $code2bit = null;

  public static function asHash() {
    if (self::$locales === null) {
      self::$locales = $GLOBALS['locales'];
      if (empty(self::$locales)) {
        self::$locales = array();
        self::$locales[self::FR] = array('bit' => 1,
                                         'code' => 'fr',
                                         'name' => 'français',
                                         'native' => 'français');
      }
    }
    if (self::$code2bit === null) {
      self::$code2bit = array();
      foreach (self::$locales as $bit => $h) {
        self::$code2bit[$h['code']] = $bit;
      }
    }
    return self::$locales;
  }

}