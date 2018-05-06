<?php

try { 

  $id = (int) $req->id;

  $item = Item::getInstance(MODEL, $id);

  $attribute = $req->attribute;

  $value = $item->getFinderAttribute($attribute);

  if (is_scalar($value)) {
    $name = $attribute;
    $type = 'text';
    $model = MODEL;
    $class = new $model;
    if ($attribute === 'status') {
      $type = 'select';
      $options = $class::$statuses;
    }
    elseif ($attribute === 'tax') {
      $type = 'select';
      $options = $class::$taxes;
    }
  }
  else {
    $hash = $value;
    $type = $hash['type'];
    $value = $hash['value'];
    $name = $hash['name'];
    $options = $hash['options'];
  }
  
  $name = 'items['.$id.']['.$name.']';

  if ($type === 'select') {
    $model = MODEL;
    $class = new $model;
    $html  = '<select name="'.$name.'" class="inline-input">';
    $html .= H::options($options, $value);
    $html .= '</select>';
  }
  elseif ($type === 'text') {
    $value = UiManager::input_protect($value);
    $html = '<input type="text" name="'.$name.'" value="'.$value.'" class="inline-input" />';
  }
  elseif ($type === 'textarea') {
    $value = UiManager::input_protect($value);
    $html = '<textarea name="'.$name.'" class="inline-input">'.$value.'</textarea>';
  }
  

  //  error_log($html);
  $res->write($html);

}
catch (Exception $e) { $err->add($e); }
