<?php

/****

si le tag est dans une url (ex. /tags/<TAG>) encoder avec rawurlencode

****/


class Cloud 
{
  private $action = null;
  private $hash = null;
  private $level_count = null;
  private $separator = null;
  private $tag_count = null;

  public function __construct($opts) {
    $opts = (array) $opts;
    $this->tag_count = $opts['tag_count'] > 1 ? (int) $opts['tag_count'] : 20;
    $this->level_count = $opts['level_count'] > 1 ? (int) $opts['level_count'] : 5;
    $this->action = isset($opts['action']) ? $opts['action'] : '/tag/((tag))';
    $this->separator = isset($opts['separator']) ? $opts['separator'] : '&middot;';
    $filters = $opts['filters'];
    $where = '';
    if ($opts['duration'] >= 1) {
      $where .= ' AND DATEDIFF("'.DH::now().'", created_on) <= '.$opts['duration'];
    }

    if ($filters instanceof Collection) {
      $filters->map(array('id'));
      $sql = 'SELECT T.name,'
        .     ' COUNT(I.tag_id) AS C'
        .' FROM '.Item::TABLE_TAG.' AS I, '.T('tag').' AS T'
        .' WHERE I.tag_id=T.id'
        .  ' AND (T.flags & '.Tag::FLAG_CLOUD_EXCLUDE.') = 0'
        .  ' AND I.model_id='.$filters->getModelId()
        .  ' AND I.record_id IN ('.$filters->asSql().')'
        .$where
        .' GROUP BY I.tag_id'
        .' ORDER BY C DESC'
        .' LIMIT '.$this->tag_count;
      $filters->unmap();
    }
    elseif (is_numeric($filters)) {
      $sql = 'SELECT T.name,'
        .     ' COUNT(I.tag_id) AS C'
        .' FROM '.Item::TABLE_TAG.' AS I, '.T('tag').' AS T'
        .' WHERE I.tag_id=T.id'
        .  ' AND (T.flags & '.Tag::FLAG_CLOUD_EXCLUDE.') = 0'
        .  ' AND I.model_id='.intval($filters)
        .$where
        .' GROUP BY I.tag_id'
        .' ORDER BY C DESC'
        .' LIMIT '.$this->tag_count;
    }
    elseif ($filters === null) {
      $sql = 'SELECT T.name,'
        .     ' COUNT(I.tag_id) AS C'
        .' FROM '.Item::TABLE_TAG.' AS I, '.T('tag').' AS T'
        .' WHERE I.tag_id=T.id'
        .  ' AND (T.flags & '.Tag::FLAG_CLOUD_EXCLUDE.') = 0'
        .$where
        .' GROUP BY I.tag_id'
        .' ORDER BY C DESC'
        .' LIMIT '.$this->tag_count;
    }
    else {
      $sql = $filters;
    }
    $this->hash = S('dbo')->asHash($sql, (array) $opts['bindings']);
    if (is_array($opts['additional'])) {
      foreach ($opts['additional'] as $word => $weight)
        $this->hash[$word] += $weight;
    }
    //    S('log')->debug($this->hash);
    $this->linearize();
    //    S('log')->debug($this->hash);
  }

  public function asHash() {
    return $this->hash;
  }

  public function asHtml() {
    ksort($this->hash);
    $links = array(); 
    foreach ($this->hash as $tag => $weight) {
      $url = str_replace('((tag))', urlencode($tag), $this->action);
      $links[] = '<a href="'.$url.'" class="tag-size-'.$weight.'" rel="tag">'.h($tag).'</a>';
    }
    $ret .= join(' '.$this->separator.' '."\n", $links);
    return $ret;
  } 
  
  /* Pareto */
  private function linearize() {
    $h = array();
    foreach ($this->hash as $tag => $weight) { 
      $h[$tag] = log($weight); 
    }
    $max = current(array_slice($h, 0, 1, true));
    $min = current(array_slice($h, -1, 1, true)); 
    if ($max === $min) {
      foreach ($h as &$weight) $weight = 1; 
      return ;
    }
    $slope = ($this->level_count - 1) / ($max - $min);
    $middle = ($this->level_count - 1) / 2;
    foreach ($h as &$weight) {
      $distance = $weight - $min;
      $result = $slope * $distance + 1;
      $result = $result < 1 ? 1 : $result;
      $result = $result > $this->level_count ? $this->level_count : $result;
      $weight = floor($result);
    }
    ksort($h);
    $this->hash = $h;
  }

  public static function getInstance($opts=null) {
    return new Cloud($opts);
  }

}