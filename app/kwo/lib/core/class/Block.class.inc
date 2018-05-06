<?php

class Block extends ActiveRecord {

  const TEMPLATE = 'block';

  const FLAG_AJAX = 1;

  public static $flags = array(self::FLAG_AJAX => 'chargement AJAX');

  public function asHtml($template=null, $bindings = null) {
    $template = $template ?: self::TEMPLATE;
    if ($bindings === true) {
      S('ctx')->setAttribute('block', $this->getAttributes());
    } else {
      $bindings = (array) $bindings;
      $bindings['block'] = $this->getAttributes();
    }
    return Template::render($template, $bindings);
  }

}