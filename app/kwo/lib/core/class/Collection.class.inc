<?php

class Collection implements Iterator, Countable
{
  private $bindings = array();
  private $cache = false;
  private $class = null;
  private $cols = null;
  private $current = null;
  private $dbo = null;
  private $exclude = array();
  private $fields = null;
  public  $filters = null;
  private $found_rows = 0;
  private $having = null;
  private $index = 0;
  private $id = null;
  private $is_item_iterator = false;
  private $last = 0;
  private $locale = 0;
  private $limit = 0;
  private $map = null;
  private $model = '';
  private $modifiers = null;
  private $name = null;
  private $num_rows = 0;
  public  $offset = 0;
  private $order = null;
  private $page = null;
  private $pair_id = 0;
  private $random = false;
  private $rs = null;
  private $sql = null;
  private $tables = null;
  private $table = null;

  public function __construct($model, $opts) {
    $opts = (array) $opts;
    if (isset($opts['filters']) && empty($opts['filters'])) {
      $opts['filters'] = array();
    }
    $this->id = $opts['id'];
    $this->name = $opts['name'];
    $this->class = is_object($model)
                 ? $model
                 : Item::getInstance($model);
    $this->model = $this->class->getModel();
    $this->table = T($this->model);
    foreach ($opts as $key => $value) {
      $this->{$key} = $value;
    }
    $this->filters = (array) $this->filters;
    $this->tables = (array) $this->tables;
    $this->having = (array) $this->having;
    $this->offset = (int) $this->offset;

    $this->class->_opts['metas'] = (array) $this->metas;
    if ($this->properties === true) {
      $hash = (array) Model::parameter($this->model, 'properties');
      $this->class->_opts['properties'] = array_keys($hash);
    }
    else {
      $this->class->_opts['properties'] = (array) $this->properties;
    }
    $this->class->_opts['group'] = (array) $this->group;
    $this->class->_opts['exclude'] = (array) $this->exclude;
    $this->locale = intval($opts['locale']);

    if ($this->locale < 1 && Model::isLocalized($this->model)) {
      $req = S('req');
      if ($req->isFront() || $req->isMiddle() || $req->isAccount()) {
        if (P('app.autoload_properties') === true) {
          $this->locale = S('res')->getLocale();
        }
      }
    }

    $this->class->_locale = $this->locale;

    if (($this->offset >= 1 || $this->page >= 1) && $this->limit < 1) {
      $this->limit = 20;
    }

//    d($this->page);
    if ($this->page >= 1) {
//      $this->page = $this->page;
      $this->offset = (int) ($this->limit * ($this->page - 1));
//      S('log')->debug('offset:'.$this->offset);
    }
    elseif ($this->offset == 1 || $this->offset == 2) { // TODO : remove
      S('log')->trace('Etes vous sûr de ne pas avoir besoin de "page" plutot que de "offset" ?',
                      __METHOD__);
    }

    if (is_array($this->order)) {
      $orders = array();
      foreach ($this->order as $key => $value) {
        if (!preg_match('/^[\w\.]+$/', $key.$value)) {
          S('log')->error('invalid order value ['.$key.$value.']', __METHOD__);
          continue ;
        }
        if (is_array($value)) {
        	$orders[] = 'FIELD('.$key.', "'.implode('","', $value).'") ASC'; //DESC
        }
        elseif (is_numeric($key)) {
          $orders[] = $value;
        }
        else {
          $orders[] = $key.(strtoupper($value) ===  'DESC' ? ' DESC' : '');
        }
      }
      $this->order = join(', ', $orders);
      $this->reverse = false;
    }
    elseif ($this->order === 'rand' || $this->order === 'random') {
      $this->order = 'RAND()';
    }
    else {
      if (strlen($this->order) >= 1) {
        S('log')->warn('order must be a hash', __METHOD__);
      }
      $this->order = null;
    }

    if ($this->reverse === true) {
      $this->order = empty($this->order)
                   ? $this->table.'.id DESC'
                   : $this->order.' DESC';
    }

    $this->class->_opts['include'] = is_array($this->include)
                                   ? $this->include
                                   : SH::asSet($this->include);

    if ($this->last >= 1) {
      if ($this->last !== true) {
        $this->limit = $this->last;
      }
      if (empty($this->order)) {
        $column = 'id';
        if (Model::hasField($this->model, 'published_at')) {
          $column = 'published_at';
        }
        elseif (Model::hasField($this->model, 'published_on')) {
          $column = 'published_on';
        }
        $this->order = $this->table.'.'.$column.' DESC';
      }
    }
    elseif ($this->limit >= 1) {
      $this->limit = $this->limit;
    }
    if ($this->random === true) {
      $this->order = 'RAND()';
    }
    $this->cache = is_bool($this->cache) ? $this->cache : false;
    if (is_array($this->map)) {
      $this->map($this->map);
    }
    $this->modifiers = (array) $this->modifiers;
    $this->dbo = S('dbo');
  }

  public function getPage() {
//    d($this->page);
    return $this->page;
  }

  public function unconstrain() {
    d(array($this->limit,$this->offset,$this->page));
    $this->limit = null;
    $this->offset = null;
    $this->page = null;
  }

  public function getOffset() {
    return $this->offset;
  }

  public function setOffset($offset) {
    $this->offset = (int) $offset;
    $this->page = 0;
    $this->reset();
  }

  public function getFilters() {
    return $this->filters;
  }

  public function setFilters($filters) {
    $this->filters = $filters;
    $this->reset();
  }

  public function setOrder($order) {
    $this->order = $order;
    $this->reset();
  }

  public function getModel() {
    return $this->model;
  }

  public function isMapped() {
    return $this->map != null;
  }

  private function start($msg) {
    if ($this->rs !== null) return ;
    $rs = $this->dbo->query($this->asSql());
    //    error_log($this->asSql());
    $this->setResultSet($rs);
  }

  public function setResultSet($rs) {
    if ($this->rs === false) {
      throw new Exception(__METHOD__);
    }
    $this->rs = $rs;
    $this->num_rows = $this->rs->numRows();
    $this->found_rows = $this->rs->foundRows();
  }

  public function hasPagination() {
    return $this->limit < 1 ? false : $this->getFoundRows() > $this->limit;
  }

  public function getPagination($opts=null) {
    $opts = (array) $opts;
    $opts += array('limit' => $this->limit < 1 ? 1 : $this->limit,
                   // fix Aleksandr
                   'page' => (int)$this->page,
                   'found_rows' => $this->getFoundRows());
    return new Pagination($opts);
  }

  public function getPages() {
    if ($this->limit < 1) return false;
    return range(1, ceil($this->getFoundRows() / $this->limit));
  }

  public function getPageCount() {
    if ($this->limit < 1) return 1;
    return ceil($this->getFoundRows() / $this->limit);
  }

  public function getOffsetFrom() {
    if ($this->limit < 1) return 1;
//    return $this->numRows() >= 1 ? $this->limit * $this->offset + 1 : 1;
    return $this->numRows() >= 1 ? $this->offset + 1 : 1;
  }

  public function getOffsetTo() {
//    return $this->limit < 1 ? false : $this->limit * $this->offset + $this->numRows();
    return $this->limit < 1 ? false : $this->offset + $this->numRows();
  }

  public function getItemAt($index=0) {
    $i = 0;
    foreach ($this as $item) {
      if ($i == $index) return $item;
      $i++;
    }
    return null;
  }

  public function getItemID($id) {
    foreach ($this as $item) {
      if ($item->getId() == $id) return $item;
    }
    return null;
  }

  public function setSql($sql) {
    $this->sql = $sql;
  }

  public function asSql() {
    if ($this->sql !== null) return $this->sql;
    $modifiers = $this->modifiers;
    $this->index = 0;

    if (isset($this->filters['sql'])) {
      $tmp_sql = $this->filters['sql'];
      $this->bindings = $this->filters['bindings'];
    }
    else {
      $tmp_sql = $this->hash2sql($this->filters);
    }

    $parts = $this->class->_getSqlParts(self);
    $cols = $parts['cols'];
    if (is_array($this->map)) {
      $cols = '';
      foreach ($this->map as $key => $value) {
        if (is_int($key)) {
          //          $cols .= strpos($value, '.') === false ? $value : $value;
          $cols .= ','.$value;
        }
        else {
          if (strpos($key, '.') === false) {
            $key = $this->table.'.'.$key;
          }
          $cols .= ','.$key.' AS '.$value;
        }
      }
    }
    if (count($this->cols) >= 1) {
      foreach ($this->cols as $key => $value) {
        if (is_numeric($key)) {
          $cols .= ', '.$value;
        }
        else {
          $cols .= ', '.$key.' AS '.$value;
        }
      }
    }
    $cols = ' '.ltrim($cols, ',');
    $from = $parts['from'];
    foreach ($this->tables as $key => $value) {
      $from .= ', '.(is_numeric($key) ? $value : $key.' as '.$value);
    }
    $order = strlen($this->order) >= 2 ? ' ORDER BY '.$this->order : '';
    $limit = $offset = '';

    if ($this->cache === true) {
      $modifiers[] = 'SQL_CACHE';
    }
    if ($this->limit >= 1) {
      if ($this->limit <= 6) {
        $modifiers[] = 'SQL_SMALL_RESULT';
      }
      if ($this->last < 1) {
        $modifiers[] = 'SQL_CALC_FOUND_ROWS';
      }
      $limit = ' LIMIT '.intval($this->limit);
      /*      if ($this->offset >= 1) {
        $offset =  ' OFFSET '.((int) $this->limit * $this->offset);
        }*/
      $offset =  ' OFFSET '.intval($this->offset); //intval($this->limit * $this->offset);
    }
    $where = $parts['where'] ? $parts['where'] : '1';

    if (strlen($tmp_sql) > 1) {
      $where = $where == '1' ? $tmp_sql : $where.' AND ('.$tmp_sql.')';
    }

    $having = '';
    if (is_array($this->having) && !empty($this->having)) {
      $having = ' HAVING '.$this->hash2sql($this->having);
    }

    $modifiers = join(' ', $modifiers);
    $modifiers = empty($modifiers) ? '' : ' '.$modifiers;

    if (strlen($this->through) >= 2) {
      $from .= ', '.$this->through;
      $where = $this->table.'.id='.$this->through.'.'.$this->class->_foreign_key.' AND ('.$where.')';
    }

    $group = empty($parts['group']) ? '' : ' GROUP BY '.$parts['group'];

    $this->sql = 'SELECT'.$modifiers.$cols.' FROM '.$from.' WHERE '.$where.$group.$having.$order.$limit.$offset;
    $this->sql = $this->dbo->prepare($this->sql, $this->bindings);

    return $this->sql;
  }

  /**
array(array('a' => 1,
      array('OR' => array('b' => 2)));

**/

  public function hash2sql($h) {
    $sql = '';
    $i = 0;
    foreach ($h as $key => $value) {
      $count = count($value);
      if ($i === 0) { $ope = ''; }
      elseif (!is_array($value)) $ope = ' AND ';
      elseif ($count === 1 && key($value) === 'OR') $ope = ' OR ';
      else $ope = ' AND ';
      $sql .= $ope;
      if (is_array($value) && (is_numeric($key) || $key === 'OR')) {
        $sql .= $count > 1 ? '(' : '';
        $sql .= $this->hash2sql($value);
        $sql .= $count > 1 ? ')' : '';
      }
      else {
        $sql .= $this->pair2sql($key, $value);
      }
      $i++;
    }
    return empty($sql) ? '1' : $sql;
  }

  private function pair2sql($key, $value) {
    $sep = '.'.$this->pair_id++;

    if (!empty($key)) {
      if ($key[0] === '|') {
        list(, $ope, $key) = explode('|', $key);
        $ope = strtolower($ope);
        if ($ope[0] === '!') {
          $not = true;
          $ope = substr($ope, 1);
        }
      }
      /*      $col = strpos($key, '.') === false
           ? $this->table.'.'.'`'.$key.'`'
           : $key;*/
   //      $col = '`'.$key.'`';

      $ambiguous = array('id', 'model_id', 'record_id', 'flags', 'status', 'name', 'code');
      $col = in_array($key, $ambiguous) ? $this->table.'.'.$key : $key;
      //error_log("$key -> $col");

      if (strpos($col, '.') > 1) {
        $parts = explode('.', $col);
        if (strpos($parts[0], '_') === false) {
          if (Model::exists($parts[0])) {
            $parts[0] = T($parts[0]);
          }
          elseif ($parts[0] === 'visit') {
            $parts[0] = Visit::TABLE;
          }
        }
        $col = join('.', $parts);
      }

      $ope = $ope === null ? '=' : $ope;
      if ($value instanceof Collection || ($ope === '=' && is_array($value))) {
        $ope = 'in';
      }
      elseif ($ope === '%%' && is_array($value)) {
        $ope = 'regexp';
      }
    }
    else {
      $ope = '#';
    }

    if ($ope == '<>' && is_array($value)) {
      $not = true;
      $ope = 'in';
    }

    if (in_array($ope, array('=', '<', '>', '<=', 'ioe', '>=', '<>', 'LIKE'))) {
      // TODO : cas spécial is_int is_float
      $ope = $ope == 'ioe' ? '<=' : $ope;
      $value = is_numeric($value) ? $value : $this->dbo->protect($value);
      $sql = $col.' '.$ope." '".$value."'";
    }
    elseif ($ope === '#') {
      $sql = $value;
    }
    elseif ($ope === '%') {
      $sql = $col.' LIKE "#'.$key.$sep.'#%"';
      $this->bindings['#'.$key.$sep.'#'] = $value;
    }
    elseif ($ope === '&' || $ope === 'or' || $ope === 'and') {
      $values = (array) $value;
      if ($ope === 'or' || count($values) <= 1) {
        $sql = '('.$col.' & '.intval(array_sum($values)).') > 0';
      }
      else {
        $value = array_sum($values);
        if ($value < 1) return ;
        $sql = '('.$col.' & '.$value.') >= '.$value;
      }
    }
    elseif ($ope === 'hbits') {
      $on = 0;
      $off = 0;
      foreach ($value as $bit => $state) {
        if ($state > 0) $on += $bit;
        else $off += $bit;
      }
      $sql = '';
      if ($on > 0) {
        $sql .= '('.$col.' & '.$on.') >= '.$on;
      }
      if ($off > 0) {
        if ($on > 0) $sql .= ' AND ';
        $sql .= '('.$col.' & '.$off.') = 0';
      }
      if ($on > 0 && $off > 0) {
        $sql = '('.$sql.')';
      }
      //      S('log')->debug($value, $col);
      /*      $value = (int) (is_array($value) ? array_sum($value) : $value);
       $sql = '('.$col.' | '.$value.') = 0';*/
    }
    elseif ($ope === 'tag') {
      $tag_id = (int) Tag::asId($value, S('res')->getLocale());
      $sql = $this->table.'.id IN (SELECT t.record_id'
                                  .' FROM '.Item::TABLE_TAG.' AS t'
                                 .' WHERE t.model_id='.$this->class->getModelId()
                                   .' AND t.tag_id='.$tag_id.')';

    }
    elseif ($ope === 'interest') {
      if (!in_array('interests', $this->class->_opts['metas'])) {
        $this->class->_opts['metas'][] = 'interests';
      }
      $value = NH::asBit($value);
      $sql = '(md.interests & '.$value.') >= '.$value;
    }
    elseif ($ope === 'manager') {
      $sql = $this->table.'.id IN (SELECT a.record_id'
                                  .' FROM '.ACL::TABLE.' AS a'
                                 .' WHERE a.user_id='.intval($value)
                                   .' AND a.model_id='.$this->class->getModelId().')';
    }
    elseif ($ope === 'y') {
      $sql = 'YEAR('.$col.')='.intval($value);
    }
    elseif ($ope === 'm') {
      $sql = 'MONTH('.$col.')='.intval($value);
    }
    elseif ($ope === 'w') {
      $sql = 'WEEK('.$col.', 1)='.intval($value);
    }
    elseif ($ope === 'wd') {
      $sql = 'WEEKDAY('.$col.')='.intval($value);
    }
    elseif ($ope === '%%') {
      $sql = $col.' LIKE '.'"%#'.$key.$sep.'#%"';
      $this->bindings['#'.$key.$sep.'#'] = $value;
    }
    elseif ($ope === 'in') {
      $value = $value instanceof Collection ? $value->asSet() : $value;
      if (is_array($value)) {
        $sql = $col.' IN '.'(['.$key.$sep.'])';
        $this->bindings['['.$key.$sep.']'] = $value;
      }
      else {
        $sql = $col.' IN '.'('.$value.')';
      }
    }
    elseif ($ope === '#') {
      $sql = $value;
    }
    elseif ($ope === '><') {
      $sql = '('.$col.' BETWEEN '.':'.$key.$sep.'0: AND :'.$key.$sep.'1:)';
      if (!is_array($value)) {
        $value = explode(' - ', $value);
        foreach ($value as &$v) $v = trim($v);
      }
      $this->bindings[':'.$key.$sep.'0:'] = $value[0];
      $this->bindings[':'.$key.$sep.'1:'] = $value[1];
    }
    elseif ($ope === 'regexp') {
      if (is_array($value)) {
        $sql = $col.' REGEXP '."'(#".$key.$sep."#)'";
        $this->bindings['#'.$key.$sep.'#'] = join('|', $value);
      }
      else {
        $sql = $col.' REGEXP '."'#".$key.$sep."#'";
        $this->bindings['#'.$key.$sep.'#'] = $value;
      }
    }
    elseif ($ope === 'match') {
      if (is_array($value)) {
        $sql = ' MATCH ('.$col.') AGAINST '."('#".$key.$sep."#' IN BOOLEAN MODE)";
        $this->bindings['#'.$key.$sep.'#'] = '+'.join(' +', $value);
      }
      else {
        $sql = ' MATCH ('.$col.') AGAINST '."('#".$key.$sep."#' IN BOOLEAN MODE)";
        $this->bindings['#'.$key.$sep.'#'] = $value;
      }
    }
    elseif ($ope === 'empty') {
      $sql = $col.' = ""';
    }
    elseif ($ope === 'have') {
      $sql = ' FIND_IN_SET(:'.$key.$sep.':, '.$col.')';
      $this->bindings[':'.$key.$sep.':'] = $value;
    }
    return $not === true ? 'NOT ('.$sql.')' : $sql;
  }

  public function isEmpty() {
    return $this->count() < 1;
  }

  // TODO : remove
  public function numRows() {
    return $this->getNumRows();
  }

  public function getNumRows() {
    $this->start(__METHOD__);
    return $this->num_rows;
  }

  public function count() {
    return $this->numRows();
  }

  public function reset() {
    $this->sql = null;
    $this->rs = null;
    $this->map = null;
  }

  public function map($map) {
    if (!is_array($map) || count($map) < 1) {
      S('log')->debug($map, __METHOD__);
      throw new Exception('invalid collection mapping');
    }
    $this->reset();
    $this->map = array();
    foreach ($map as $key => $value) {
      $key = is_int($key) ? $value : $key;
      $this->map[$key] = $value;
    }
    return $this;
  }

  public function unmap() {
    $this->reset();
    return $this;
  }

  public function getFields() {
    $this->start(__METHOD__);
    return $this->map !== null
                        ? array_values($this->map)
                        : $this->rs->getFields();
  }

  public function getFoundRows() {
    $this->start(__METHOD__);
    return $this->found_rows;
  }

  // TODO : remove
  public function foundRows() {
    return $this->getFoundRows();
  }

  public function first() {
    $this->start(__METHOD__);
    if ($this->rs->numRows() < 1) {
      S('log')->error('numrows < 1', __METHOD__);
      throw new Exception('can not go first');
    }
    $this->rewind();
    return $this->current;
  }

  public function last() {
    $this->start(__METHOD__);
    if ($this->rs->numRows() < 1) {
      S('log')->error('numrows < 1', __METHOD__);
      throw new Exception('can not go last');
    }
    $this->rs->goLast();
    return $this->next();
  }

  public function getNext() {
    $this->start(__METHOD__);
    return $this->next();
  }

  public function itemize() {
    $this->is_item_iterator = !$this->is_item_iterator;
  }

  public function current() {
    return $this->current;
  }

  public function key() {
    return $this->index - 1;
  }

  public function next() {
    $obj = $this->rs->fetchObject();
    if (empty($obj)) {
      $this->current = null;
      return false;
    }
    $this->index++;
    if ($this->map !== null) {
      $this->current = (array) $obj;
    }
    else {
      $this->current = clone $this->class;
      $this->current->_setNucleus($obj);
    }
    $item = $this->is_item_iterator ? $this->current->getItem() : $this->current;
    if ($this->locale > 0) {
      $item->getProperties($this->locale, true);
    }
    return $item;
  }

  public function rewind() {
    $this->start(__METHOD__);
    $this->current = null;
    $this->rs->goFirst();
    $this->index = 0;
    $this->next();
  }

  public function valid() {
    return $this->current !== null;
  }

  public function asHash($key_field='id', $value_field=null) {
    $this->cache = true;
    $h = array();
    $value_field = $value_field === true ? 'name' : $value_field;
    foreach ($this as $item) {
      $h[$item->getAttribute($key_field)] = $value_field === null
                                          ? $item
                                          : $item->getAttribute($value_field);
    }
    return $h;
  }

  public function debug($sql=false) {
    S('log')->debug($this->asArray());
    if ($sql === true) {
      S('log')->debug($this->sql, __METHOD__);
    }
  }

  public function trace() {
    $this->start(__METHOD__);
    S('log')->trace($this->sql, __METHOD__);
//    S('log')->backtrace();
  }

  public function asArray($attr=null) {
    $results = array();
    foreach ($this as $item) {
      if ($this->map !== null) {
        $results[] = $attr === null
                   ? $item
                   : array_intersect_key($item, array_flip($attr));
      }
      else {
        $results[] = $attr === null
                   ? $item->getAttributes()
                   : $item->getAttribute($attr);
      }
    }
    return $results;
  }

  public function asSet($attr='id') {
    $set = array();
    foreach ($this as $item) {
      $set[] = is_array($item)
             ? $item[$attr]
             : $item->getAttribute($attr);
    }
    return $set;
  }

  public function getModelId() {
    return $this->class->getModelId();
  }

  public function setName($name) {
    $this->name = $name;
  }

  public function asFeed($channel) {
    return FeedWriter::getInstance($channel, $this);
  }

  public function asSitemap() {
    try {
      return new Sitemap($this, array('locale' => $this->locale));
    }
    catch (Exception $e) {
      return S('log')->error($e, __METHOD__);
    }
  }

  public function sum($attr) {
    $ret = 0;
    foreach ($this as $item) {
      $ret += $item->getAttribute($attr);
    }
    return $ret;
  }

  public function maximum($attr) {
    $ret = 0;
    foreach ($this as $item) {
      $value = $item->getAttribute($attr);
      $ret = $value > $ret ? $value : $ret;
    }
    return $ret;
  }

  public function minimum($attr) {
    $ret = 0;
    $first = true;
    foreach ($this as $item) {
      $value = $item->getAttribute($attr);
      $ret = ($first === true || $value < $ret) ? $value : $ret;
      $first = false;
    }
    return $ret;
  }

  public function implode($glue, $attr='name') {
    $set = array();
    foreach ($this as $item) {
      $set[] = $item->getAttribute($attr);
    }
    return implode($glue, $set);
  }

  public function average($attr) {
    $values = array();
    foreach ($this as $item) {
      $values[] = $item->getAttribute($attr);
    }
    $n = count($values);
    return $n < 1 ? 0 : array_sum($values) / $n;
  }

  public function skip($num) {
    $num = (int) $num;
    foreach ($this as $i => $item) {
      if ($i >= $num) break ;
    }
    return $this;
  }

  public function __clone() {
    $this->rs = null;
    $this->sql = null;
  }

  public function getName() {
    return $this->name === null ? $this->getModel() : $this->name;
  }

  public function asDataFileName() {
    return $this->name === null ? $this->getModel() : $this->name;
  }

  public static function getInstance($model, $opts=null) {
    if ($opts === false) {
      return self::getEmptyInstance($model);
    }
    return new Collection($model, $opts);
  }

  public static function getEmptyInstance($model) {
    return new Collection($model, array('filters' => array('id' => '0')));
  }

}
