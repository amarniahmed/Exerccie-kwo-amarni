<?php

class Form extends ActiveRecord
{
  const FLAG_ACK_SEND = 1;
  const FLAG_ACK_SHOW = 2;
  const FLAG_ALERT = 4;
  const FLAG_AUTH = 8;
  const FLAG_CAPTCHA = 16;
  const FLAG_CONVERSION = 32;
  const FLAG_COLUMNS = 64;

  public static $flags = array(self::FLAG_ACK_SEND => 'mail de confirmation',
                               self::FLAG_ACK_SHOW => 'msg de confirmation',
                               self::FLAG_ALERT => 'avertissement',
                               self::FLAG_AUTH => 'compte obligatoire',
                               self::FLAG_CAPTCHA => 'captcha',
                               self::FLAG_CONVERSION => 'conversion',
                               self::FLAG_COLUMNS => 'en 2 colonnes');

  public function getDetails() {
    return array('nbre réponses' => $this->countSubmissions());
  }

  public function asEditHash() {
    $out = array();
    if ($this->isItem()) {
      $out['workspace'] = H::get('/back/core/field.workspace',
                                 array('form_id' => $this->getId()));
    }
    return $out;
  }

  public function getFieldCodes() {
    $fields = array();
    foreach ($this->getFields() as $field) {
//      if (!$field->isOnline()) continue ;
      $fields[$field->id] = $field->code;
    }
    return $fields;
  }

  public function hasRequiredField() {
    foreach ($this->getFields() as $field) {
      if ($field->isOnline() && $field->isRequired()) return true;
    }
    return false;
  }

  public function asHtml($locale, $bindings=null, $failures=null) {
    if (!$this->isOnline()) return '';
    $req = S('req');
    $this->getProperties($locale);

    $key = 'form'.$this->getId();
    if ($req->hasAttribute($key)) {
      $attr = $req->getAttribute($key);
      $bindings = (array) $attr['fields'];
      $failures = (array) $attr['failures'];
    }
    else {
      $bindings = (array) $bindings;
      $failures = (array) $failures;
    }
    $args = array();
    $columns = array(0 => '', 1 => '');
    $fields = '';
    $hidden = '<input type="hidden" name="item" value="'.$this->getKey().'" />'."\n";

    foreach ($this->getFields() as $field) {
      if (($field->locales & $locale) < 1) continue ;
      if (!$field->isOnline() || !$field->isHidden()) continue ;
      $hidden .= $field->asHtml($locale, $bindings, $failures)."\n";
    }

    if ($this->hasFlag(self::FLAG_COLUMNS)) {
      foreach ($this->getFields() as $field) {
        if (($field->locales & $locale) < 1) continue ;
        if (!$field->isOnline() || $field->isHidden()) continue ;
        $col = $field->hasFlag(Field::FLAG_COLUMN_RIGHT) ? 1 : 0;
        $columns[$col] .= $field->asHtml($locale, $bindings, $failures)."\n";
      }
      if ($this->hasFlag(self::FLAG_CAPTCHA)) {
        $columns[0] .= Field::captcha($failures);
      }
      $fields .= '<div class="form-content__form-fields__columns first">'.implode('</div>'."\n".'<div class="form-content__form-fields__columns last">', $columns).'</div>';
    }
    else {
      foreach ($this->getFields() as $i => $field) {
        if (($field->locales & $locale) < 1) continue ;
        if (!$field->isOnline() || $field->isHidden()) continue ;
        $fields .= $field->asHtml($locale, $bindings, $failures)."\n";
      }

      if ($this->hasFlag(self::FLAG_CAPTCHA)) {
        $fields .= Field::captcha($failures);
      }
    }
    $args['action'] = '/submission.save';
    $args['class'] = $this->class ? ' '.$this->class : '';
    $args['fields'] = $fields;
    $args['hidden'] = $hidden;
    $args['form'] = $this;

    $template = $this->template ?: 'form';

    return Template::render($template, $args);
  }

  public function asShortCodeContent($args=null) {
    return $this->asHtml();
  }

}
