<?php

class Context
{
  public function debug() { 
    S('log')->debug($this->getAttributes('debug')); 
  }

  public function setContent($content) {
    $this->setAttribute('_content', $content);
  }

  public function getBranchAttribute($key) {
    return $this->getAttribute($key);
  }

  public function setAttributes($var) {
    if (empty($var)) return false;
    if ($var instanceof ActiveRecord || $var instanceof Request) {
      if ($var instanceof ActiveRecord) { 
        if (!$var->isItem()) return true;
        if (!$this->hasAttribute('_model_id')) {
          $this->setAttribute('_model_id', $var->getModelId());
          $this->setAttribute('_record_id', $var->getId());
        }
      }
      foreach ($var->getAttributes() as $key => $value) {
        $this->setAttribute($key, $value);
      }
    }
    elseif (is_array($var) || is_object($var)) {
      foreach ($var as $key => $value) {
        $this->setAttribute($key, $value);
      }
    }
    return true;
  }

  public function removeAttributes() {
    foreach (get_object_vars($this) as $key => $value) {
      $this->{$key} = null;
    }
  }

  public function getAttributes($debug=false) { 
    if ($debug === 'debug') { 
      $h = array();
      foreach (get_object_vars($this) as $key => $value) {
        if ($value === null) continue ;
        if ($value instanceof ActiveRecord) {
          $value = $value->getAttributes();
        }
        elseif ($value instanceof Collection) {
          $value = '{Collection} '.$value->getName().' ('.$value->count().' elts)'; 
        }
        $h[$key] = $value; 
      }
      return $h;
    }
    return get_object_vars($this);
  }

  public function getPublicAttributes() { 
    $h = array();
    foreach ($this->getAttributes() as $key => $value) { 
      if ($key[0] === '_' || 
          $value instanceof ActiveRecord || $value instanceof Collection || is_resource($value)) continue ; 
      if (is_array($value)) { 
        if (empty($value)) {
          $h[$key] = $value;
          continue ;
        }
        foreach ($value as $k => $v) {
          if ($v instanceof ActiveRecord || $v instanceof Collection || is_resource($v)) continue ;
          $h[$key][$k] = $v;
        }
        continue ;
      }
      $h[$key] = $value;
    } 
    return $h;
  }

  public function setAttribute($key, $value) { $this->{$key} = $value; }
  public function hasAttribute($key) { return property_exists($this, $key); }
  public function getAttribute($key) { return $this->{$key}; }

  public static function getInstance() {
    if (Platform::hasSingleton('ctx')) return ;
    Platform::addSingleton('ctx', new Context());
  }

}
