<?php

// loader en html et recuperer tous les mots qui sont entre
// B, H1, STRONG, H2
// ON BOOST POUR : LES PAGES ANCIENNES (>1an), les pages avec liens

class Indexer
{
  private $engine = null;
  private $ttl = 0;
  private $words = null;

  public static $document_count = 0;
  public static $stems = array();
  public static $stem_max = 4000;

  public function __construct() {
    $this->ttl = 3600 * 24 * 31 * 3;
    //    $this->engine = Engine::factory();
  }

  public function setEngine($engine) {
    $this->engine = $engine;
  }

  public function process($document) {

    $dbo = S('dbo');

    $filters = array('model_id' => $document->model_id,
                     'record_id' => $document->record_id,
                     'locale' => $document->locale);

    $rs = ResultSet::getInstance(Document::TABLE, array('filters' => $filters));

    if ($rs->numRows() >= 1) {
      $document_id = (int) $rs->fetchValue();
    }

    if ($document_id < 1) {
      $row = array('model_id' => $document->model_id,
                   'record_id' => $document->record_id,
                   'locale' => $document->locale);
      $document_id = (int) $dbo->insert(Document::TABLE, $row);
      if ($document_id < 1) {
        S('log')->error('document creation failure', __METHOD__);
        return ;
      }
    }
    else {
      $dbo->exec('DELETE FROM '.Engine::TABLE_BUCKET
                 .    ' WHERE document_id='.$document_id);
      $dbo->exec('DELETE FROM '.Engine::TABLE_INDEX
                 .    ' WHERE document_id='.$document_id);
    }

    $row = array('locale' => $document->locale,
                 'status' => Document::STATUS_ONLINE,
                 'size' => str_word_count($document->content, 0));
    $opts = array('filters' => array('id' => $document_id));
    $dbo->update(Document::TABLE, $row, $opts);

    if (in_array($document->locale, Engine::$known_locales)) {

      self::$document_count++;

      if (self::$document_count == 50 || self::$document_count == 500) {
        self::loadBestStems();
      }

      self::getStopWords($document->locale);
      $this->words = array();

      $this->indexString(self::normalize($document->name), $document->locale, array('factor' => 80));
      $this->indexString(self::normalize($document->keywords), $document->locale, array('factor' => 30));
      $this->indexString(self::normalize($document->description), $document->locale, array('factor' => 2));
      $this->indexString(self::normalize($document->content), $document->locale, array('factor' => 0));

      if ($document->word_count > 1) {
        $this->words = array_slice($this->words, 0, $document->word_count);
      }

      if (count($this->words) < 1) {
        $this->indexString(self::normalize('nulldoc'), $document->locale, array('factor' => 0));
      }

      /*    if (($document->updated_at - $document->created_at) > $this->ttl ||
            strlen($document->content) >= 500) {
            $document->boost_factor += 5;
            }*/

      $i = 0;
      $records = array();

      foreach ($this->words as $word => $hash) {
        $records[] = array('stem_id' => self::word2id($word),
                           'document_id' => $document_id,
                           'weight' => $hash['weight'] + $document->boost_factor);
        if (($i++ % 1000) === 0) {
          $dbo->insert(Engine::TABLE_BUCKET, $records);
          $records = array();
        }
      }

      if (count($records) >= 1) {
        $dbo->insert(Engine::TABLE_BUCKET, $records);
      }

    }

    $row = array('document_id' => $document_id,
                 'model_id' => $document->model_id,
                 'record_id' => $document->record_id,
                 'locale' => $document->locale,
                 'name' => $document->name,
                 'content' => $document->asText());
    $indexes = (array) $document->indexes;
    $row += $indexes;
    $dbo->insert(Engine::TABLE_INDEX, $row);

  }

  public static function loadBestStems() {
    $h = S('dbo')->asHash('SELECT stem_id, count(*) as n'
                             .' FROM '.Engine::TABLE_BUCKET
                             .' GROUP BY stem_id'
                             .' ORDER BY n DESC'
                             .' LIMIT '.self::$stem_max);
    self::$stems = S('dbo')->asHash('SELECT name, id'
                                    .' FROM '.Engine::TABLE_STEM
                                    .' WHERE id IN ('.join(',', array_keys($h)).')'
                                    .' ORDER BY name');
    if (false) {
      $tmp = array_slice(self::$stems, 0, 100);
      S('log')->debug($tmp);
    }
  }

  public static function word2id($word) {
    $id = self::$stems[$word];
    if ($id > 0) {
      return $id;
    }
    $id = S('dbo')->fetchValue('SELECT id'
                               .' FROM '.Engine::TABLE_STEM
                               .' WHERE name=:name:',
                               array(':name:' => $word));
    if ($id < 1) {
      $id = S('dbo')->insert(Engine::TABLE_STEM,
                             array('name' => $word));
      if ($id < 1) {
        S('log')->error('word : '.$word, __METHOD__);
      }
    }
    if (count(self::$stems) < self::$stem_max) {
      self::$stems[$word] = (int) $id;
    }
    return $id;
  }

  public function indexString($content, $locale, $opts=null) {
    if (strlen($content) < 1) return ;
    $opts = (array) $opts;
    $factor = (int) $opts['factor'];
    $stemming = isset($opts['stemming']) ? (bool) $opts['stemming'] : true;
    self::getStopWords($locale);
    $words = Engine::tokenize($content);
    $i = $position = 0;
    foreach ($words as $word) {
      $i++;
      if (strlen($word) <= 1 || strlen($word) >= 25) continue ;
      if (is_numeric($word) && (strlen($word) < 4 || strlen($word) > 6)) continue ;
      if (in_array($word, self::$stopwords[$locale])) continue ;
      $stem = $word;
      if ($stemming === true) {
        $stem = self::extractStem($stem, $locale);
      }
      //      echo "[".$word."] -> [".$stem."]\n";
      if ($position >= 500)     $weight = 10;
      elseif ($position >= 100) $weight = 20;
      elseif ($position >= 50)  $weight = 40;
      elseif ($position > 10)   $weight = 60;
      else                      $weight = 70;
      $this->words[$stem]['weight'] += $weight + $factor;
      /*      if ($this->words[$stem]['position'] < 1 && $track_position === true) {
        $this->words[$stem]['position'] = $i;
        }*/
      $position++;
    }
  }

  public function index($item) {
    if (!$item->isIndexable()) {
      Document::remove($item);
      return ;
    }
    $set = S('dbo')->asSet('SELECT DISTINCT locale'
                           .' FROM '.Document::TABLE
                           .' WHERE model_id='.$item->getModelId()
                           .  ' AND record_id='.$item->getId());
    if (Model::hasAnyField($item, array('locale', 'locales'))) {
      $locales = $item->getLocales();
      if (empty($locales)) return ;
    }
    else {
      $locales = S('app')->getLocales();
    }
    foreach ($set as $locale) {
      if (!in_array($locale, $locales)) {
        Document::remove($item, $locale);
      }
    }
    foreach ($locales as $locale) {
      try {
        $document = $item->asDocument($locale);
        if (empty($document)) {
          Document::remove($item);
          continue ;
        }
        $this->process($document);
      }
      catch (Exception $e) {
        S('log')->debug($e, __METHOD__);
      }
    }
  }

  //-- STATIC ------------------------------------------------------------------

  private static $suffixes = null;
  private static $stopwords = array();

  public static function getStopWords($locale) {
    if (is_array(self::$stopwords[$locale])) {
      return self::$stopwords[$locale];
    }
    $file = LIB_PATH.'/'.CORE.'/opt/stopwords.'.Locale::toCode($locale);//$file = LIB_PATH.'/'.CORE.'/opt/stopwords.'.I18N::locale2code($locale);
    $content = @file_get_contents($file);
    if ($content === false) {
      S('log')->trace('failure ['.Locale::toCode($locale).'] ('.$locale.')',//S('log')->trace('failure ['.I18N::locale2code($locale).'] ('.$locale.')',
                      __METHOD__);
      self::$stopwords[$locale] = array();
      return ;
    }
    self::$stopwords[$locale] = (array) explode("\n", $content);
    return self::$stopwords[$locale];
  }

  public static function normalize($text) {
    $text = html_entity_decode($text, ENT_QUOTES, 'UTF-8');
    //    $text = SH::stripAccents($text);
    $text = strtolower($text);
    $text = str_replace(array('<', '>', '_'),
                        array(' <', '> ', ''),
                        $text);
    $text = strip_tags($text);
    //    $text = strtolower($text);
    return $text;
  }

  public static function extractStem($word, $locale) {
    return Stemmer::factory($locale)->extract($word);
  }

}



    /*    if (true) {
      return Stemmer::factory($locale)->extract($word);
    }

    if (self::$suffixes === null) {
      $suffixes = explode("\n", file_get_contents(LIB_PATH.'/core/opt/suffix.fr'));
      self::$suffixes = '/('.join('|', $suffixes).')$/u';
    }
    $word = preg_replace('/(es|e|s|x)$/u', '', $word);
    if (strlen($word) <= 4) return $word;
    $stem = preg_replace(self::$suffixes, '', $word);
    $stem = str_replace(array('ff', 'mm', 'nn', 'pp', 'rr', 'tt'),
                        array('f', 'm', 'n', 'p', 'r', 't'),
                        $stem);
    $stem = trim($stem);
    $out = $word != $stem && strlen($stem) >= 1 ? $stem : $word;
    //    S('log')->trace($word.' - '.$stem);
    return $out;*/