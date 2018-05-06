<?php

final class Table 
{
  private $dbo = null;
  private $columns = null;
  private $fields = null;
  private $name = null;

  public function __construct($name, $dbo=null) { 
    $this->name = $name;
    $this->dbo = $dbo ?: S('dbo');
    $this->dbo->connect();
  }

  public function reset() {
    $this->fields = null;
    $this->columns = null;
    return $this;
  }

  public function getFields() {
    if (is_array($this->fields)) {
      return $this->fields;
    }
    $rs = @mysql_query('SHOW COLUMNS FROM '.$this->name, $this->dbo->link());
    if ($rs === false) return S('log')->error(mysql_error(), __METHOD__); 
    if (mysql_num_rows($rs) < 1) return S('log')->error('no column', __METHOD__); 
    $results = array();
    while ($row = mysql_fetch_assoc($rs)) {
      $results[] = $row['Field'];
    }
    $this->fields = $results;
    return $this->fields;
  }

  public function getColumns() {
    if (is_array($this->columns)) {
      return $this->columns;
    }
    $rs = @mysql_query('SHOW COLUMNS FROM '.$this->name, $this->dbo->link());
    if ($rs === false) return S('log')->error(mysql_error(), __METHOD__); 
    if (mysql_num_rows($rs) < 1) return S('log')->error('no column', __METHOD__); 
    $results = array();
    while ($row = mysql_fetch_assoc($rs)) {
      $results[] = $row;
    }
    $this->columns = $results;
    return $this->columns;
  }

  public function hasFields() {
    if (func_num_args() < 1) return false;
    $fields = $this->getFields();
    if (!is_array($fields)) return false;
    return count(array_diff(is_array(func_get_arg(0)) ? func_get_arg(0) : func_get_args(), 
                            $fields)) < 1;
  }

  public function hasField($key) {
    $fields = $this->getFields();
    return in_array($key, $fields);
  }

  public function hasRow(array $fields) {
    $parts = array();
    foreach ($fields as $key => $value) {
      $value = $this->dbo->protect($value);
      $parts[] = '`'.$key.'`="'.$value.'"';
    }
    if (empty($parts)) return false;
    $ret = $this->dbo->fetchValue('SELECT COUNT(*)'
                                  .' FROM '.$this->name
                                  .' WHERE '.join(' AND ', $parts)) >= 1;
    return $ret;
  }

  public function dump() {
    $out_path = TMP_PATH.'/dump-'.$this->name.'.sql';
    $cmd = 'mysqldump -t '.$this->dbo->name().' '.$this->name.' > '.$out_path;
    `$cmd`;
  }

  public function truncate() {
    $this->dbo->exec('TRUNCATE TABLE `'.$this->name.'`');
    return $this;
  } 

  public function drop() {
    $ret = $this->dbo->exec('DROP TABLE IF EXISTS `'.$this->name.'`');
    if ($ret !== false) {
      $this->dbo->resetTables();
    }
  } 

  public function insert($row, $opts=null) {
    $opts = (array) $opts;
    if ($opts['unique'] === true) { 
      if ($this->hasRow($row)) return ; 
      unset($opts['unique']);
    }
    return $this->dbo->insert($this->name, $row, $opts);
  }

  public function optimize() {
    $this->dbo->exec('OPTIMIZE TABLE `'.$this->name.'`');
    return $this;
  }

  public function repair() {
    $this->dbo->exec('REPAIR TABLE `'.$this->name.'`');
    return $this;
  }
 
  public function resetAutoIncrement() {
    $this->dbo->exec('ALTER TABLE `'.$this->name.'` AUTO_INCREMENT=1');
    return $this;
  }

  public function name() {
    return $this->name;
  }

  public function getName() {
    return $this->name;
  }

  public function __toString() {
    return $this->name;
  }

}

