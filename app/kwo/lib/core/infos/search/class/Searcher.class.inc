<?php

class Searcher
{
  private $dbo = null;
  private $locale = 0;
  private $limit = 20;
  private $order = 'weight';
  private $page_count = 10;
  private $options_arr = array();
  private $query = '';

  // UTILISER PAGE_COUNT PR LIMITER LA PAGINATION

  public function __construct($options_arr=null) {
    $this->dbo = S('dbo');
    if ($options_arr !== null) {
      $this->options_arr = $options_arr;
    }
    $this->query = trim($this->options_arr['query']);

    if ($this->query === '*') $this->query = '';

    $this->options_arr['offset'] = ($this->options_arr['offset'] >= 1) ? $this->options_arr['offset'] : 1;
    $this->options_arr['limit'] = ($this->options_arr['limit'] >= 1) ? $this->options_arr['limit'] : 1;

    $this->locale = (int) $this->options_arr['locale'];
  }

  private function query() {
    $bindings = array();
    $offset = ($this->options_arr['offset'] - 1) * $this->options_arr['limit'];
    $limit = $this->options_arr['limit'];
    $where = '';

    if (empty($this->options_arr['order'])) {
      if (empty($this->query)) {
        $order = ' ORDER BY D.title';
      }
      else {
        $order = ' ORDER BY docweight DESC';
      }
    }
    else {
      $order = ' ORDER BY '.$this->options_arr['order'];
    }

    if (empty($this->query)) {
      if (!empty($this->options_arr['letter']) && strlen($this->options_arr['letter']) >= 1) {
        $where .= ' AND title LIKE "'.$this->options_arr['letter'][0].'%"';
      }
    }
    else {
      $ids_arr = self::query2ids($this->query, $this->locale);
      if (count($ids_arr)<1) $ids_arr[] = 0;
      $where .= ' AND W.word_id IN ([words_arr])';
      $bindings['[words_arr]'] = $ids_arr;
    }

    if ($this->locale >= 1) {
      $where .= ' AND D.locale=:locale:';
      $bindings[':locale:'] = $this->locale;
    }

    $metas_joins = ' LEFT JOIN sys_item_meta AS MD1 on MD1.record_id=D.record_id AND MD1.model_id=D.model_id AND MD1.type='.Meta::RANKING   
        .' LEFT JOIN sys_item_meta AS MD2 on MD2.record_id=D.record_id AND MD2.model_id=D.model_id AND MD2.type='.Meta::RATING    
        .' LEFT JOIN sys_item_meta AS MD3 on MD3.record_id=D.record_id AND MD3.model_id=D.model_id AND MD3.type='.Meta::RATING_COUNT    
        .' LEFT JOIN sys_item_meta AS MD4 on MD4.record_id=D.record_id AND MD4.model_id=D.model_id AND MD4.type='.Meta::SUBSCRIPTION_COUNT;
    $metas_cols = ', MD2.value as rating, MD3.value as rates, MD4.value as subscriptions';
    
    if (!empty($this->query)) {
      /*      $sql = 'SELECT D.title, D.icon, D.description, D.url, D.size, D.created_at, D.model_id, D.record_id, sum(W.weight) + D.ranking + IFNULL(MD1.value, 0) as docweight,'
        .' W.position, concat(W.record_id,"-",W.model_id) as doc_key'.$metas_cols
        .' FROM se_word AS W, se_document AS D'.$metas_joins    
        .' WHERE W.record_id=D.record_id AND W.model_id=D.model_id'
        .$where
        .' GROUP BY doc_key'
        .$order
        .' LIMIT '.$limit
        .' OFFSET '.$offset;*/
    }
    else {
       $sql = 'SELECT D.title, D.icon, D.description, D.url, D.size, D.created_at, D.model_id, D.record_id, D.ranking + IFNULL(MD1.value, 0) as docweight'.$metas_cols
         .' FROM se_document AS D'.$metas_joins
         .' WHERE 1'.$where
         .$order
         .' LIMIT '.$limit
         .' OFFSET '.$offset;
    }

    $rs = $this->dbo->query($sql, 
                            $bindings, 
                            array('SQL_CACHE', 'SQL_CALC_FOUND_ROWS'));
    //    $this->dbo->debug();

    if (strlen($this->query) > 0) {
      $event = !empty($this->query) ? $this->query : '*';
      $category = ($rs === false || $rs->numRows() < 1) ? 'search-empty' : 'search';
      S('req')->track($event, $category);
    }

    return $rs;
  }

  private function getPagination() {
    $offset = $this->options_arr['offset'];
    $found_rows = $this->options_arr['found_rows'];
    $limit = $this->options_arr['limit'];
    $a = $found_rows / $limit;
    $b = $found_rows % $limit;
    if ($b > 0) $a++;
    $page_count = $a;
    $pages_arr = array();
    for ($i = 1; $i <= $page_count; $i++) {
      $pages_arr[] = array('name'=>$i,
                           'is_selected_flag' => ($i == $offset),
                           'action' => '/search/results?query='.$this->query.'&offset='.$i);
    }
    return $pages_arr;
  }

  public function asText($template='search:results') {
    $rs = $this->query();
    $search['documents'] = array();
    foreach ($rs as $i => $hash) {
      $hash['index'] = ($this->options_arr['offset'] - 1) * $this->options_arr['limit'] + $i + 1;
      $hash['rate'] = (strlen($hash['rate']) >= 5) ? AH::decode($hash['rate']) : null;
      $hash['subscriptions'] = (int) $hash['subscriptions'];
      $hash['created_at'] = DH::formatDate($hash['created_at']);
      $hash['docweight'] = $hash['docweight'];
      $search['documents'][] = $hash;
    }
    $search['query'] = empty($this->query) ? '*' : $this->query;
    $search['action'] = $this->options_arr['action'];
    $search['letter'] = $this->options_arr['letter'];
    $search['found_rows'] = $this->options_arr['found_rows'] = $rs->foundRows();
    $search['limit'] = $this->options_arr['limit'];
    $search['pages'] = $this->getPagination();
    if (empty($this->query)) {
      $search['letters'] = range('a', 'z');
    }
    $search['start'] = ($this->options_arr['offset'] - 1) * $this->options_arr['limit'] + 1;
    $end = $this->options_arr['offset'] * $this->options_arr['limit'];
    if ($end > $this->options_arr['found_rows']) {
      $end = $this->options_arr['found_rows'];
    }
    $search['end'] = $end;
    return Template::render($template, $search);
  }

  public static function query2ids($query, $locale=I18N::LOCALE_FR) {
    $words = (array) preg_split('/[\W\s]+/', $query);
    $stems = array();
    $stopwords = Indexer::getStopWords($locale);
    foreach ($words as $word) {
      if (strlen($word) <= 2 || in_array($word, $stopwords)) {
        continue;
      }
      $stems[] = Indexer::extractStem($word);
    }
    if (count($stems) < 1) return array();
    $ret = S('dbo')->asArray('SELECT word_id'
                             .' FROM se_thesaurus'
                             .' WHERE word IN ([stems])',
                             array('[stems]' => $stems),
                             'word_id');
    return $ret;
  }

}
