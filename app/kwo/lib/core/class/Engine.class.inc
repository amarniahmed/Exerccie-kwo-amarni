<?php

class Engine extends ActiveRecord {

  const TABLE_BUCKET = 'core_bucket';
  const TABLE_INDEX = 'core_index';
  const TABLE_STEM = 'core_stem';

  const FLAG_FULLTEXT = 1;
  const FLAG_SYNONYM = 2;

  public static $flags = array(self::FLAG_FULLTEXT => 'fulltext',
                               self::FLAG_SYNONYM => 'synonymes');

  public static $known_locales = array(I18N::LOCALE_FR, I18N::LOCALE_EN, 
                                       I18N::LOCALE_ES, I18N::LOCALE_IT);

  public function getDetails() {
    $n = S('dbo')->fetchValue('SELECT COUNT(*) FROM '.Document::TABLE);
    return array('nbre de documents indexés' => $n);
  }
  
  public static $singletons = array();
  
  public static function factory() {
    if (self::$singletons['engine'] === null) {
      self::$singletons['engine'] = Engine::getInstance(1);
    }
    return self::$singletons['engine'];
  }
  
  public static function getIndexer() {
    $indexer = self::$singletons['indexer'];
    if ($indexer === null) {
      $indexer = new Indexer();
      $indexer->setEngine(self::factory());
    }
    return $indexer;
  }

  public static function query($opts=null) {
    return Searcher::getInstance($opts);
  }
  
  public static function tokenize($content) {
    $tokens = preg_split('/[\W\s]+/u', $content);
    $tokens = empty($tokens) ? array() : $tokens;
    return $tokens;
  }
  
  public static function crawl() {
    if (ini_get('mbstring.func_overload') < 1) {
      S('log')->error('mbtring.func_overlaod should be 7');
      return ;
    }
    $opts = array('filters' => array('|<|status' => Document::STATUS_ONLINE),
                  'group' => array('model_id', 'record_id'));
    $n = 0;
    foreach (ResultSet::getInstance(Document::TABLE, $opts) as $row) {
      try {
 //       S('log')->debug(array($row['model_id'], 
//                              $row['record_id']));
        $item = Item::getInstance((int) $row['model_id'], 
                                  (int) $row['record_id']); 
        $item->index();
        $n++;
      }
      catch (Exception $e) { 
//        S('log')->error($e, __METHOD__);
        Document::remove($row); 
      } 
    }
    return $n;
  }

}