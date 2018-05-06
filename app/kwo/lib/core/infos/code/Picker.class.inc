<?php

class Picker
{
  public  $args = null;
  public  $collection = null;
  public  $file = null;
  public  $filters = array();
  public  $height = 580;
  public  $id = 'picker';
  public  $include = null;
  private $limit = 30;
  public  $model = null;
  private $offset = 0;
  public  $order = null;
  public  $overlay = null;
  private $session_key = null;

  public function __construct($args=array()) { 

    $session = S('req')->getSession();
    $this->model = ucfirst($args['model']);


    $this->session_key = 'picker-'.$this->model;

    if ($args['reset'] == 1) { 
      $session->removeAttribute($this->session_key);
    }
    elseif ($args['open'] == 1 && is_array($session->getAttribute($this->session_key))) { 
      $args['open'] = 0;
      $args = $session->getAttribute($this->session_key);
    }
    else {
      $session->setAttribute($this->session_key, $args);
    }
    
    $this->args = $args;

    $this->file = LIB_PATH.'/'.M($this->model, 'extension').'/xml/'.$this->model.'.pick.xml';
        
    $this->file = file_exists($this->file)
                ? $this->file
                : LIB_PATH.'/'.CORE.'/xml/item/pick.xml';

    $this->loadFromXML();

    if ($args === null || $args['reset'] == 1) {
      return ;
    }

    $this->limit = strlen($args['limit']) >= 1 ? (int) $args['limit'] : $this->limit;
    $this->offset = strlen($args['offset']) >= 1 ? (int) $args['offset'] : 0;
    $this->order = strlen($args['order']) >= 1 ? $args['order'] : $this->order;

    $order = array();
    if (strlen($this->order) >= 1) {
      foreach (explode(',', $this->order) as $col) {
        $col = trim($col);
        list($field, $direction) = explode(' ', $col);
        $direction = trim(strtoupper($direction));
        $order[$field] = $direction === 'DESC' ? $direction : 'ASC';
      }
    }

    $filters = array();
    if ($this->hasFilters()) {
      foreach ($args['filters'] as $name => $value) {
        if (strlen($value) < 1) continue ;
        $filters[$name] = $value;
        $this->filters[$name]['value'] = $value;
      }
    }

    $opts = array('filters' => $filters,
                  'limit' => $this->limit,
                  'offset' => $this->offset,
                  'order' => $order,
                  'include' => strlen($this->include) >= 1 ? SH::asSet($this->include) : null);

    $this->collection = Item::collect($this->model, $opts);
  }

  private function hasFilters() {
    return isset($this->args['filters']);
  }

  public function offsetFrom() {
    return $this->collection->offsetFrom();
  }

  public function offsetTo() {
    return $this->collection->offsetTo();
  }

  public function hasPagination() {
    return $this->collection->hasPagination();
  }

  public function getPagination() {
    return $this->collection->getPagination();
  }

  private function loadFromXML() {
    
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    if ($doc->load($this->file) === false) {
      throw new Exception();
    }

    $pickers = $doc->documentElement->getElementsByTagName('picker');
    $picker = $pickers->item(0);

    $attributes = self::node2hash($picker);
    foreach ($attributes as $key => $value) {
      $this->{$key} = $value;
    }

    foreach ($picker->childNodes as $node) {
      if ($node->nodeName == 'filters') {
        foreach ($node->childNodes as $child) {
          if ($child->getAttribute('type') === 'tag') {
            $child->setAttribute('type', 'text');
            $child->setAttribute('name', '|TAG|');
            $child->setAttribute('label', 'tag');
          }
          $name = $child->getAttribute('name');
          if ($name[0] != '|' && 
              !in_array($name, array('id', 'state', 'status', 'type')) && 
              substr($name, -3) != '_id' && substr($name, -5) != '_type') {
            $name = '|%|'.$name;
            $child->setAttribute('name', $name);
          }
          $this->filters[$name] = self::node2hash($child);
          if ($this->hasFilters()) unset($this->filters[$name]['value']);
        }
      }
    }

  }

  public static function node2hash($node) {
    $map = $node->attributes;
    $hash = array();
    $convert = array('options', 'value');
    foreach ($map as $index => $attributes) {
      //      $hash[$attributes->name] = utf8_decode($attributes->value);
      $hash[$attributes->name] = $attributes->value;
      if (in_array($attributes->name, $convert)) {
        $hash[$attributes->name] = self::evalString($hash[$attributes->name]);
      }
    }
    return $hash;
  }

  public static function evalString($str) {
    eval('$ret = '.$str.';'); 
    return $ret;
  }

  public function __toString() { 
    try {
      $html = Template::render('ui.picker', array('picker' => $this));
      return $html;
    }
    catch (Exception $e) {
      S('req')->getSession()->removeAttribute($this->session_key);
      S('log')->error($e, __METHOD__);
    }
    return 'error';
  }

  public static function getInstance($params) {
    return new Picker($params);
  }

}