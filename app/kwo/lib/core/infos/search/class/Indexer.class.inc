<?php

// loader en html et recuperer tous les mots qui sont entre
// B, H1, STRONG, H2
//ON BOOST POUR LES PAGES ANCIENNES (>1an)

class Indexer
{
  private $ttl = 0;

  public function __construct() {
    $this->ttl = 3600 * 24 * 31 * 3;
  }

  public function process($document) {     
    
    self::getStopWords($document->locale);

    $dbo = S('dbo');  

    $id = $dbo->insert('se_document', 
                       array('record_id' => $document->record_id,
                             'model_id' => $document->model_id,
                             'locale' => $document->locale,
                             'updated_at' => $document->updated_at,
                             'created_at' => $document->created_at,
                             'url' => $document->url,
                             'size' => strlen($content),
                             'title' => $document->title,
                             'description' => $document->description,
                             'icon' => $document->icon)); 


    $this->words_arr = array();
    
    $this->indexString(self::normalize($document->title), $document->locale, 0.8);
    $this->indexString(self::normalize($document->keywords), $document->locale, 0.3);
    $this->indexString(self::normalize($document->description), $document->locale, 0.02);
    $this->indexString(self::normalize($document->content), $document->locale, 0, true);
   
    if (($document->updated_at - $document->created_at) > $this->ttl || 
        strlen($document->content) >= 500) {
      $document->boost_factor += 0.1;
    }

    $i = 0;
    $inserts_arr = array();
    foreach ($this->words_arr as $word => $word_arr) {
      $word_arr['word_id'] = $dbo->fetchValue('SELECT word_id FROM se_thesaurus WHERE word=:word:',
                                              array(':word:' => $word));
      if ($word_arr['word_id'] === false) {
        $word_arr['word_id'] = $dbo->insert('se_thesaurus', 
                                            array('word' => $word));
      }
      $word_arr['weight'] += $document->boost_factor;
      $inserts_arr[] = array('word_id' => $word_arr['word_id'],
                             'weight' => $word_arr['weight'],
                             'position' => $word_arr['position'],
                             'record_id' => $document->record_id,
                             'model_id' => $document->model_id);
      if (($i++ % 300) === 0) {
        $dbo->insert('se_word', $inserts_arr);
        $inserts_arr = array();
      }       
    }

    if (count($inserts_arr) >= 1) {
      $dbo->insert('se_word', $inserts_arr);
    }

  }

  public function indexString($content, $locale, $factor, $position_flag=false) {
    if (strlen($content) < 1) return ;
    $words_arr = preg_split('/[\W\s]+/', $content);
    $i = $position = 0;
    foreach ($words_arr as $word) {
      $i++;
      if (strlen($word) <= 2 || strlen($word) >= 25 || 
          in_array($word, self::$stopwords_arr) ||
          (ord($word) >= 48 && ord($word) <= 57)) {
        continue;
      }
      $word = self::extractStem($word);      
      if ($position >= 500)     $weight = 0.1;
      elseif ($position >= 100) $weight = 0.2;
      elseif ($position >= 50)  $weight = 0.4;
      elseif ($position > 10)   $weight = 0.6;
      else                      $weight = 0.7;
      $this->words_arr[$word]['weight'] += $weight + $factor; 
      if ($this->words_arr[$word]['position'] < 1 && $position_flag === true) {
        $this->words_arr[$word]['position'] = $i; 
      }
      $position++;
      /*$stem = self::extractStem($word);
      if ($stem !== false) {
        $this->words_arr[$stem]['weight'] += 0.01;
        if ($this->words_arr[$stem]['position'] < 1) {
          $this->words_arr[$stem]['position'] = 0;
        }
      }*/
    }
  }
  
  //-- STATIC ------------------------------------------------------------------
  
  private static $suffix = null;
  private static $stopwords_arr = null;
  private static $singleton = null;

  public static function getStopWords($locale) {
    if (is_array(self::$stopwords_arr[$locale])) {
      return self::$stopwords_arr[$locale]; 
    }
    self::$stopwords_arr[$locale] = (array) explode("\n", 
                                                     @file_get_contents(LIB_PATH.'/search/opt/stopwords.'.I18N::lang($locale)));
    return self::$stopwords_arr[$locale];
  }

  public static function normalize($text) {
    $text = strtolower($text);
    $text = str_replace(array('<', '>', '_'), array(' <', '> ', ''), $text);
    $text = strip_tags($text);
    $text = html_entity_decode($text, ENT_QUOTES);
    $text = SH::stripAccents($text);
    return $text;
  }
  
  public static function extractStem($word) {
    if (self::$suffix === null) {
      $suffix_arr = explode("\n", file_get_contents(LIB_PATH.'/search/opt/suffix.fr'));
      self::$suffix = '/('.join('|', $suffix_arr).')$/';
    }
    $word = preg_replace('/(es|e|s|x)$/', '', $word);
    if (strlen($word) <= 4) return $word;
    $stem = preg_replace(self::$suffix, '', $word);
    return ($word != $stem && strlen($stem) >= 1) ? $stem : $word;
  }



  public static function getInstance() { 
    if (self::$singleton !== null) return self::$singleton;
    $class = __CLASS__;
    self::$singleton = new $class; 
    return self::$singleton;
  }

}


