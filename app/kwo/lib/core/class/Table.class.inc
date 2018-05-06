<?php

final class Table 
{
  private $dbo = null;
  private $columns = null;
  private $fields = null;
  private $name = null;

  public function __construct($name, $dbo=null) { 
    $this->dbo = $dbo ?: S('dbo');
    $this->name = $name;
  }

  public function reset() {
    $this->columns = null;
    $this->fields = null;
    return $this;
  }

  public function getFields() {
    if (!is_array($this->fields)) {
      $columns = $this->getColumns();
      if (!is_array($columns)) return null;
      $this->fields = array();
      foreach ($columns as $column) {
        $this->fields[] = $column['Field'];
      }
    }
    return $this->fields;
  }

  public function getColumns() {
    if (!is_array($this->columns)) {
      $rs = $this->dbo->query('SHOW COLUMNS FROM '.$this->name);
      if ($rs === false) return null;
      if ($rs->numRows() < 1) {
       S('log')->error('no column', __METHOD__); 
       return null;
      }
      $this->columns = array();
      while ($row = $rs->fetchHash()) {
        $this->columns[] = $row;
      }
    }
    return $this->columns;
  }

  public function hasFields() {
    if (func_num_args() < 1) return false;
    $fields = $this->getFields();
    if (!is_array($fields)) return false;
    $args = is_array(func_get_arg(0)) ? func_get_arg(0) : func_get_args();
    return count(array_diff($args, $fields)) < 1;
  }

  public function hasField($key) {
    $fields = $this->getFields();
    if (!is_array($fields)) return false;
    return in_array($key, $fields);
  }

  public function hasRow(array $fields) {
    $dbo = $this->dbo;
    $parts = array();
    foreach ($fields as $key => $value) {
      $value = $dbo->protect($value);
      $parts[] = '`'.$key.'`="'.$value.'"';
    }
    if (empty($parts)) return false;
    $ret = $dbo->fetchValue('SELECT COUNT(*)'
                            .' FROM '.$this->name
                            .' WHERE '.join(' AND ', $parts)) >= 1;
    return $ret;
  }

  public function dump() {
    $dbo = $this->dbo;
    $out_path = TMP_PATH.'/dump-'.$this->name.'.sql';
    $cmd = 'mysqldump -t '.$dbo->getName().' '.$this->name.' > '.$out_path;
    `$cmd`;
  }

  public function truncate() {
    $dbo = $this->dbo;
    $dbo->exec('TRUNCATE TABLE `'.$this->name.'`');
    return $this;
  } 

  public function drop() {
    $dbo = $this->dbo;
    $ret = $dbo->exec('DROP TABLE IF EXISTS `'.$this->name.'`');
    if ($ret !== false) {
      $dbo->resetTables();
    }
  } 

  public function insert($row, $opts=null) {
    $dbo = $this->dbo;
    $opts = (array) $opts;
    if ($opts['unique'] === true) { 
      if ($this->hasRow($row)) return ; 
      unset($opts['unique']);
    }
    return $dbo->insert($this->name, $row, $opts);
  }

  public function optimize() {
    $dbo = $this->dbo;
    $dbo->exec('OPTIMIZE TABLE `'.$this->name.'`');
    return $this;
  }

  public function repair() {
    $dbo = $this->dbo;
    $dbo->exec('REPAIR TABLE `'.$this->name.'`');
    return $this;
  }
 
  public function resetAutoIncrement() {
    $dbo = $this->dbo;
    $dbo->exec('ALTER TABLE `'.$this->name.'` AUTO_INCREMENT=1');
    return $this;
  }

  public function getName() {
    return $this->name;
  }

  public function name() {
    S('log')->trace('** DEPRECATED ('.$this->name.') - contact FXB **', __METHOD__);
    // todo : à supprimer
    return $this->name;
  }

  public function __toString() {
    S('log')->trace('** DEPRECATED ('.$this->name.') - contact FXB **', __METHOD__);
//    S('log')->backtrace();
//    die(0);
    return $this->name;
  }

public function asDataFileName() {
    return $this->name;
  }

}

