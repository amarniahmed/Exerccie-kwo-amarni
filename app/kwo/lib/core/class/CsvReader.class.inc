<?php

// http://tools.ietf.org/html/rfc4180

/* attention au format SYLK : premiere colonne est un id
pour éviter ne pas donner un nom de colonne avec des majuscules pour les 2 premiers chars
ex : ID Panier -> id Panier
en regle générale mieux vaut donc etre en minuscules */

class CsvReader extends File implements Iterator
{
  public $allowed_fields = null;
  public $current = null;
  public $field_enclosed_by = null;
  public $field_escaped_by = '\\';
  public $field_separated_by = null;
  public $fields = null;
  public $fields_count = 0;
  public $index = 0;
  public $line_terminated_by = "\n";
  public $map = null;
  public $max_line_length = 0;
  public $required_fields = null;
  public $skip_first = false;

  public function __construct($file_path, $opts=null) {
    /*    if (strpos($file_path, '/') === false) {
      $file_path = '/'.DOC_PATH.'/'.$file_path;
      }*/
    parent::__construct($file_path, 'r');
    parent::open();

    $opts = (array) $opts;

    foreach ($opts as $key => $value) {
      $this->{$key} = $value;
    }

    $this->getFormat();
    $this->getFields();

    $this->fields_count = count($this->fields);

    if (is_array($this->required_fields)) {
      $ret = AH::match($this->required_fields, $this->fields);
      if ($ret !== true) {
        S('log')->debug($this->fields, __METHOD__);
        S('log')->error('missing column ['.$ret.']', __METHOD__);
        throw new Exception('missing column ['.$ret.']');
      }
    }

    if (is_array($this->allowed_fields)) {
      $ret = AH::match($this->fields, $this->allowed_fields);
      if ($ret !== true) {
        S('log')->error('unknown column ['.$ret.']', __METHOD__);
        throw new Exception('unknown column ['.$ret.']');
      }
    }

  }

  public function map($map) {
    if (!is_array($map) || count($map) < 1) {
      S('log')->debug($map, __METHOD__);
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

  private function getFormat() {
    if ($this->field_enclosed_by !== null &&
        $this->field_separated_by !== null) {
      return ;
    }
    $chunk = $this->getChunk(256);
    if (preg_match('/"[ ]*;[ ]*"/', $chunk)) {
      $this->field_enclosed_by = '"';
      $this->field_separated_by = ';';
    }
    elseif (preg_match('/"[ ]*\t[ ]*"/', $chunk)) {
      $this->field_enclosed_by = '"';
      $this->field_separated_by = "\t";
    }
    elseif (preg_match('/"[ ]*,[ ]*"/', $chunk)) {
      $this->field_enclosed_by = '"';
      $this->field_separated_by = ',';
    }
    else {
      $this->field_enclosed_by = ' ';
      if (strpos($chunk, "\t") > 0) {
        $this->field_separated_by = "\t";
      }
      elseif (strpos($chunk, ';') > 0) {
        $this->field_separated_by = ';';
      }
      else {
        $this->field_separated_by = ',';
      }
    }
//    S('log')->trace('('.$this->field_enclosed_by.')-('.$this->field_separated_by.')');
  }

  public function getFields() {
    if (is_array($this->fields)) {
      return $this->fields;
    }
    $this->skip_first = true;
    $this->fields = fgetcsv($this->fd,
                            $this->max_line_length,
                            $this->field_separated_by,
                            $this->field_enclosed_by);
    return $this->fields;
  }

  public function getNext() {
    return $this->next();
  }

  public function next() {
    $cels = fgetcsv($this->fd, $this->max_line_length,
                        $this->field_separated_by, $this->field_enclosed_by);
    if ($cels === false) {
      $this->current = false;
      return $this->current;
    }
    $this->index++;
    $n = count($cels);
    if ($this->fields_count > $n) {
      $cels = array_pad($cels, $this->fields_count, '');
    }
    elseif ($this->fields_count < $n) {
      $cels = array_slice($cels, 0, $this->fields_count);
    }
    $this->current = array_combine($this->fields, $cels);
    if ($this->map !== null) {
      $current = array();
      foreach ($this->map as $from => $to) {
        $current[$to] = $this->current[$from];
      }
      $this->current = $current;
    }
    return $this->current;
  }

  public function asArray($field=null) {
    $results = array();
    while ($row = $this->getNext()) {
      $results[] = $field === null ? $row : $row[$field];
    }
    return $results;
  }

  public function asHash($key, $value=null) {
    $hash = array();
    while ($row = $this->getNext()) {
      $hash[$row[$key]] = $value === null ? $row : $row[$value];
    }
    return $hash;
  }

  /*** Iteration ***/
  public function rewind() {
    parent::rewind();
    if ($this->skip_first === true) { $this->next(); }
    $this->next();
    $this->index = 0;
  }
  public function current() { return $this->current; }
  public function key() { return $this->index; }
  public function valid() { return ($this->current !== false); }
  /*** Iteration ***/

  public static function getInstance($file_path, $opts=null) {
    ini_set('auto_detect_line_endings', true);
    return new CsvReader($file_path, $opts);
  }

  public function asDataFileName() {
    return 'csv';
  }

}
