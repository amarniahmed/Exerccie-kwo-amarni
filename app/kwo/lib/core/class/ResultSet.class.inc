<?php

Class ResultSet implements Iterator, Countable 
{
  private $current_record = false;
  private $dbh = null;
  private $fields = null;
  private $found_rows = null;
  private $index = 0;
  private $map = null;
  private $result = null;
  private $sql = null;
  private $name = null;
  
  public function __construct($result, $sql, $dbh) { 
    $this->result = $result;
    $this->sql = $sql;
    $this->dbh = $dbh;
    if (stripos($this->sql, 'SQL_CALC_FOUND_ROWS', 6) > 0) {
      $this->foundRows();
    }
  }

  public function asSql() {
    return $this->sql;
  }
  
  public function asHash($key_field=null, $value_field=null) {
    if ($key_field === null) {
      $fields = $this->getFields();
      $key_field = $fields[0];
      $value_field = $fields[1];
    }
    $h = array();
    foreach ($this as $row) {
      $h[$row[$key_field]] = $value_field ? $row[$value_field] : $row;
    }
    return $h;
  }

  public function asArray($key=null) {
    $results = array();
    foreach ($this as $row) {
      if ($key === null) {
        $results[] = $row;
        continue ;
      }
      $results[$row[$key]] = $row; 
    }
    return $results;
  }

  public function asSet($field=null) {
    $set = array();
    foreach ($this as $row) {
      $set[] = $field ? $row[$field] : current($row);
    }
    return $set;
  }

  public function fetchArray() { 
    return $this->result->fetch_array(MYSQLI_NUM);
  }

  public function fetchHash() { 
    $h = $this->result->fetch_array(MYSQLI_ASSOC); 
    //todo: return null
    if ($h === null) return false;
    if ($this->map !== null) {
      $tmp = array();
      foreach ($this->map as $from => $to) {
        $tmp[$to] = $h[$from];
      }
      $h = $tmp;
    }
    return $h;
  }  

  public function fetchObject() { return $this->result->fetch_object(); }

  public function fetchValue() { 
    $row = $this->result->fetch_row(); 
    return $row[0];
  }

  public function foundRows() {
    if ($this->found_rows !== null) {
      return $this->found_rows;
    }
    $result = $this->dbh->query('SELECT FOUND_ROWS()');
    if ($result === false) {
      return $this->trace('found_rows failure ['.$this->sql.']', __METHOD__);
    }
    $row = $result->fetch_row();
    $result->free();
    $this->found_rows = $row[0];
    return $this->found_rows;
  }

  public function free() { 
    // peut on detruire l'objet ResultSet ?
    return $this->result->free();
  }

  public function getNext($type=MYSQLI_ASSOC) { 
    return $this->result->fetch_array($type); 
  }

  public function isEmpty() { return $this->numRows() < 1; }

  public function numRows() {
    $ret = $this->result->num_rows;
    if ($ret === false) {
      $this->trace($this->sql, __METHOD__);
      return 0;
    }
    return (int) $ret;
  }

  public function count() {
    return $this->numRows();
  }

  public function getFields() {
    if ($this->map !== null) {
      return array_values($this->map);
    }
    if ($this->fields) return $this->fields;
    $this->fields = array();
    while ($meta = $this->result->fetch_field()) {
      $this->fields[] = $meta->name;
    }
    return $this->fields;
  }

  public function numFields() {
    $ret = $this->result->field_count;
    return $ret === false ? $this->trace($this->sql, __METHOD__) : $ret;
  }

  public function result($row=0, $col=0) { 
    $ret = $this->result->data_seek($row);
    if ($ret === false) {
      $this->trace($this->sql, __METHOD__);
      return $ret;
    }
    $ret = $this->result->field_seek($col); 
    return $ret === false ? $this->trace($this->sql, __METHOD__) : $ret;
  }

  public function sum($field) {
    $out = 0;
    foreach ($this as $row) {
      $out += $row[$field];
    }
    return $out;
  }

  public function map($map) {
    if (!is_array($map) || count($map) < 1) {
      S('log')->error('bad input', __METHOD__);
      S('log')->debug($map, 'mapping');
      throw new Exception('bad mapping');
    }
    $this->map = array();
    foreach ($map as $key => $value) {
      $key = is_int($key) ? $value : $key;
      $this->map[$key] = $value;
    }
    return $this;
  }

  public function unmap() {
    $this->map = null;
    return $this;
  }

  public function debug($prefix='') { 
    $prefix = empty($prefix) ? '' : ' : '.$prefix;
    S('log')->debug($prefix.$this->sql); 
  }

  private function trace($msg, $method, $trace_error=true) {
    S('log')->log->error($msg, $method); 
    if ($trace_error === true) {
      S('log')->log->error($this->dbh->error.' ('.$this->dbh->errno.')',
                           __METHOD__); 
    }
    return false;
  }

  public function goLast() {
    if ($this->numRows() < 1) return false;
    $ret = $this->result->data_seek($this->numRows() - 1);
    if ($ret === false) return $this->trace($this->sql, __METHOD__);
    return $ret;
  }

  public function goFirst() { 
    if ($this->numRows() < 1) return ;
    $ret = $this->result->data_seek(0);
    if ($ret === false) return $this->trace($this->sql, __METHOD__);
    return $ret;
  }

  /*** Iterator ***/
  public function rewind() { 
    $this->current_record = false;
    if ($this->numRows() < 1) return ;
    $ret = $this->result->data_seek(0);
    if ($ret === false) return $this->trace($this->sql, __METHOD__);
    $this->current_record = $this->fetchHash();
    $this->index = 0;
    return $ret;
  }

  public function current() {
    return $this->current_record;
  }
  
  public function key() {
    return $this->index;
  }

  public function next() {
    $this->current_record = $this->fetchHash();
    $this->index++;
    return $this->current_record;
  }
  
  public function valid() {
    return $this->current_record !== false && $this->current_record !== null;
  }

  /*** End Iterator ***/
  
  public function setName($name) {
    $this->name = $name;
  }

  public function hasPagination() {
    return $this->limit < 1 ? false : $this->foundRows() > $this->limit;
  }

  public function asCollection($model) {
    $h = $this->getBoundaries();
    $opts = array('limit' => $h['limit'], 
                  'page' => $h['offset'] + 1,
                  'offset' => $h['offset']);
    $collection = Item::collect($model, $opts);
    $collection->setResultSet($this);
    $collection->setSql($this->sql);
    return $collection;
  }

  public function getBoundaries() {
    preg_match('/ LIMIT\s+(?<limit>\d+)\s+OFFSET\s+(?<offset>\d+)/i', $this->sql, $h);
    $h['limit'] = $h['limit'] < 1 ? 50 : $h['limit'];
    $h['offset'] = intval($h['offset'] / $h['limit']);
    return $h;
  }

  public function getPagination() {
    $h = $this->getBoundaries();
    return new Pagination(array('limit' => $h['limit'],
                                'offset' => $h['offset'],
                                'found_rows' => $this->foundRows()));
  }

  public function asDataFileName() {
    if ($this->name) return $this->name;
    $meta = $this->result->fetch_field_direct(0);
    return $meta->table;
  }

  public static function getInstance($table, array $opts=array()) {
    $dbo = S('dbo');
    $sql = 'SELECT';
    if (is_array($opts['modifiers'])) {
      $sql .= ' '.join(' ', $opts['modifiers']);
    }
    if (isset($opts['map'])) {
      $opts['map'] = (array) $opts['map'];
      foreach ($opts['map'] as $key => &$value) {
        $value = is_int($key) ? $value : $key.' AS '.$value;
      }
      $sql .= ' '.join(',', $opts['map']); 
    }
    else {
      $sql .= ' *';
    }
    $sql .= ' FROM '.$table;
    if (is_array($opts['filters'])) {
      $sql .= ' WHERE '.$dbo->hash2sql($opts['filters']);
    }
    if (is_array($opts['order'])) {
      $orders = array();
      foreach ($opts['order'] as $key => $value) {
        if (!preg_match('/^[\w\.]+$/', $key.$value)) {
          S('log')->error('invalid order ['.$key.'-'.$value.']', __METHOD__);
          continue ;
        }
        if (is_numeric($key)) {
          $orders[] = $value;
        }
        else {
          $orders[] = $key.(strtoupper($value) ===  'DESC' ? ' DESC' : '');
        }
      }
      $sql .= ' ORDER BY '.join(', ', $orders);
    }
    if ($opts['limit'] >= 1) {
      $sql .= ' LIMIT '.$opts['limit'];
    }
    $sql .= empty($opts['group']) ? '' : ' GROUP BY '.join(', ', $opts['group']);
    return $dbo->query($sql, null, $opts['modifiers']);
  }

}

