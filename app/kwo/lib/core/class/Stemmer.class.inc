<?php

class Stemmer {

  public static $stemmers = array();

  public static function factory($locale=null) {
    $locale = $locale ?: Locale::FR;
    if (!self::$stemmers[$locale]) {
      if ($locale == Locale::FR) {
        include LIB_PATH.'/'.CORE.'/class/Stemmers/FrenchStemmer.class.inc';
        self::$stemmers[$locale] = new FrenchStemmer();
      }
      else {
        $locale = 0;
        $class = __CLASS__;
        self::$stemmers[$locale] = new $class;
      }
    }
    return self::$stemmers[$locale];
  }


  public function extract($word) {
    return $word;
  }

}


