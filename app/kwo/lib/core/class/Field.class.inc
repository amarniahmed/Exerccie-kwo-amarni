<?php

class Field extends ActiveRecord {

  const FLAG_REQUIRED = 1;
  const FLAG_READONLY = 2;
  const FLAG_IN_NAME = 4;
  const FLAG_COLUMN_LEFT = 8;
  const FLAG_COLUMN_RIGHT = 16;

  public static $flags = array(self::FLAG_REQUIRED => 'obligatoire',
                               self::FLAG_READONLY => 'lecture seule',
                               self::FLAG_IN_NAME => 'dans résumé',
                               self::FLAG_COLUMN_LEFT => 'colonne de gauche',
                               self::FLAG_COLUMN_RIGHT => 'colonne de droite');

  const TYPE_TEXT = 1;
  const TYPE_SELECT = 2;
  const TYPE_RADIO = 3;
  const TYPE_CHECKBOX = 4;
  const TYPE_YESNO = 5;
  const TYPE_HIDDEN = 6;
  const TYPE_FILE = 7;
  const TYPE_TITLE = 8;

  public static $type_names = array(self::TYPE_TEXT => 'texte',
                                    self::TYPE_SELECT => 'menu déroulant',
                                    self::TYPE_RADIO => 'radio',
                                    self::TYPE_CHECKBOX => 'checkbox',
                                    //                                    self::TYPE_YESNO => 'yesno',
                                    self::TYPE_HIDDEN => 'caché',
                                    self::TYPE_FILE => 'fichier',
                                    self::TYPE_TITLE => 'titre (section)');

  public static $type_codes = array(self::TYPE_TEXT => 'text',
                                    self::TYPE_SELECT => 'select',
                                    self::TYPE_RADIO => 'radio',
                                    self::TYPE_CHECKBOX => 'checkbox',
                                    //                                    self::TYPE_YESNO => 'yesno',
                                    self::TYPE_HIDDEN => 'hidden',
                                    self::TYPE_FILE => 'file');

  const CONTROL_SIZE = 1;
  const CONTROL_EMAIL = 2;
  const CONTROL_DATE = 3;

  public static $control_types = array(0 => 'aucun',
                                       self::CONTROL_SIZE => 'taille',
                                       self::CONTROL_EMAIL => 'email',
                                       self::CONTROL_DATE => 'date');

  const RECORD_NAME = 'record';

  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['code'])) {
      if (empty($attrs['code']) || is_numeric($attrs['code'])) {
        throw new ActiveRecordException('champ [code] invalide');
      }
      else {
        $attrs['code'] = SH::slugify($attrs['code']);
      }
      if (!isset($attrs['name']) || empty($attrs['name'])) {
        $attrs['name'] = $attrs['code'];
      }
    }
/*    if (isset($attrs['options']) && isset($attrs['type']) &&
        in_array($attrs['type'],
                 array(self::TYPE_SELECT, self::TYPE_RADIO, self::TYPE_CHECKBOX)) &&
        empty($attrs['options'])) {
      throw new ActiveRecordException('le champs "options" est obligatoire');
    } */
  }

  public function markAsEvolved() {
    parent::markAsEvolved();
    $this->getForm()->markAsEvolved();
  }

  public function asEditHash() {
    return array('disable_home' => true);
  }

  public function isHidden() {
    return $this->type == self::TYPE_HIDDEN;
  }

  public function isRequired() {
    return $this->hasFlag(self::FLAG_REQUIRED);
  }

  public function asHtml($locale, $bindings=null, $failures=null) {
    if ($this->type == self::TYPE_TITLE) {
      return $this->renderTitle($bindings, $locale);
    }
    $props = $this->getProperties($locale);
    if ($this->isHidden()) {
      $attrs = array('type' => 'hidden',
                     'name' => $this->getInputName(),
                     'value' => h($this->getDefaultValue($bindings)));
      return '<input '.AH::asAttributes($attrs).' />';
    }
    $classes = array('form-field',
                     'form-field-'.$this->code,
                     'form-field-'.$this->getId(),
                     'form-field-type-'.self::$type_codes[$this->type]);
    if ($this->type == self::TYPE_TEXT && $this->size > 0) {
      $classes[] = 'form-field-type-textarea';
    }
    if ($this->isRequired()) {
      $classes[] = 'form-field-required';
    }
    if ($this->hasFlag(self::FLAG_READONLY)) {
      $classes[] = 'form-field-readonly';
    }
    if (in_array($this->getId(), $failures)) {
      $classes[] = 'form-field-failure';
    }
    if (!empty($this->class)) {
      $classes[] = $this->class;
    }
    $html  = '';
    $html .= '<div class="'.join(' ', $classes).'">'."\n";
    $label = $props['label'];
    if ($label) {
      $html .= '<label>';
      $html .= c($label);
      if ($this->isRequired()) {
        $html .= '<span class="form-field-asterisk">*</span>';
      }
      if (!empty($props['description'])) {
        $html .= '<div class="form-field-description">'.$props['description'].'</div>'."\n";
      }
      $html .= '</label>'."\n";
    }
    $html .= '<div class="form-field-control">'."\n";
    switch ($this->type) {
      case self::TYPE_CHECKBOX: $html .= $this->renderCheckbox($bindings); break;
      case self::TYPE_RADIO: $html .= $this->renderRadio($bindings); break;
      case self::TYPE_SELECT: $html .= $this->renderSelect($bindings); break;
      case self::TYPE_TEXT: $html .= $this->renderText($bindings); break;
      //case self::TYPE_YESNO: $html .= $this->renderYesno($bindings); break;
      case self::TYPE_FILE: $html .= $this->renderFile($bindings); break;
    }
    if (!empty($props['help'])) {
      $html .= '<div class="form-field-help">'.$props['help'].'</div>'."\n";
    }
    if (!empty($props['error'])) {
      $style = in_array($this->getId(), $failures) ? '' : ' style="display:none;"';
      $html .= '<div class="form-field-error"'.$style.'>'.$props['error'].'</div>'."\n";
    }
    $html .= '</div>'."\n";

    $html .= "\n";
    $html .= '</div>'."\n";
    return $html;
  }

  private function getInputName() {
    return self::RECORD_NAME.'['.$this->getId().']';
  }

  private function getDefaultValue($bindings) {
    if (isset($bindings[$this->id])) {
      $value = $bindings[$this->id];
      return $value;
    }
    $value = $this->getProperty('default');
    if ($value['0'] == '$') {
      $value = substr($value, 1);
      $value = $bindings[$value];
    }
    return $value;
  }

  public function getOptions() {
    $set = $this->getProperty('options');
    if (strpos($set, '::') === false) {
      $options = SH::asSet($set);
    }
    else {
      $callback = explode('::', $set);
      $options = call_user_func($callback, $this);
    }
    return $options;
  }

  private function renderFile($bindings) {
    $attrs = array('class' => array('text', 'field-file'),
                   'type' => 'text',
                   'on' => array('click' => 'new Kwo.Dialogs.FileManager(this)'));
    $attrs['value'] = h($this->getDefaultValue($bindings));
    if ($this->hasFlag(self::FLAG_READONLY)) {
      $attrs['readonly'] = 'true';
    }
    $html .= H::elt('input', null, $attrs);
    $attrs = array('class' => array('field-file'),
                   'type' => 'hidden',
                   'name' => $this->getInputName());
    $html .=  H::elt('input', null, $attrs);
    return $html;
  }

  private function renderText($bindings) {
    if ($this->size > 0) {
      return $this->renderTextarea($bindings);
    }
    $attrs = array('class' => 'text',
                   'type' => 'text',
                   'name' => $this->getInputName());
    $attrs['value'] = h($this->getDefaultValue($bindings));
    if (strlen($this->placeholder) > 2) {
      $attrs['placeholder'] = $this->placeholder;
    }
    if ($this->hasFlag(self::FLAG_READONLY)) {
      $attrs['readonly'] = 'true';
    }
    if ($this->control_type == self::CONTROL_DATE) {
      $attrs['readonly'] = 'true';
      $attrs['onclick'] = 'new Kwo.Datepicker(this)';
    }
    $html .= H::elt('input', null, $attrs);
    return $html;
  }

  private function renderTextarea($bindings) {
    $attrs = array('name' => $this->getInputName(),
                   'rows' => $this->size);
    if (strlen($this->placeholder) > 2) {
      $attrs['placeholder'] = $this->placeholder;
    }
    if ($this->hasFlag(self::FLAG_READONLY)) {
      $attrs['readonly'] = 'true';
    }
    $default = $this->getDefaultValue($bindings);
    $html = '<textarea '.AH::asAttributes($attrs).'>'.h($default).'</textarea>';
    return $html;
  }

  private function renderSelect($bindings) {
    $attrs = array('name' => $this->getInputName());
    if ($this->size > 0) {
      $attrs['size'] = $this->size;
      $attrs['multiple'] = 'multiple';
      $attrs['name'] .= '[]';
    }
    if ($this->hasFlag(self::FLAG_READONLY)) {
      $attrs['readonly'] = 'true';
    }
    if (!empty($this->event) && strpos($this->event, '|') !== false) {
      list($event, $value) = explode('|', $this->event);
      $attrs[$event] = $value;
    }
    $default = $this->getDefaultValue($bindings);
    $default = SH::asSet($default);
    $options = $this->getOptions();
    $html  = '<select '.AH::asAttributes($attrs).'>'."\n";
    foreach ($options as $k => $option) {
      $selected = in_array($option, $default) ? ' selected="selected"' : '';
      $html .= ' <option value="'.$option.'"'.$selected.' data-key="'.$k.'">'.$option.'</option>'."\n";
    }
    $html .= '</select>';
    return $html;
  }

  private function renderCheckbox($bindings) {
    $attrs = array('name' => $this->getInputName().'[]',
                   'type' => 'checkbox');
    if ($this->hasFlag(self::FLAG_READONLY)) {
      $attrs['readonly'] = 'true';
    }
    if (!empty($this->event) && strpos($this->event, '|') !== false) {
      list($event, $value) = explode('|', $this->event);
      $attrs[$event] = $value;
    }
    $default = $this->getDefaultValue($bindings);
    $default = SH::asSet($default);
    $options = $this->getOptions();
    $html = '<div class="field-options">';
    foreach ($options as $option) {
      $attrs['value'] = $option;
      $checked = in_array($option, $default) ? ' checked="checked"' : '';
      $html .= ' <label>';
      $html .= '<input '.AH::asAttributes($attrs).$checked.' />';
      $html .= $option;
      $html .= '</label>'."\n";
    }
    $html .= '</div>';
    return $html;
  }

  private function renderRadio($bindings) {
    $attrs = array('name' => $this->getInputName().'[]',
                   'type' => 'radio');
    if ($this->hasFlag(self::FLAG_READONLY)) {
      $attrs['readonly'] = 'true';
    }
    if (!empty($this->event) && strpos($this->event, '|') !== false) {
      list($event, $value) = explode('|', $this->event);
      $attrs[$event] = $value;
    }
    $default = $this->getDefaultValue($bindings);
    $default = SH::asSet($default);
    $options = $this->getOptions();
    $html = '<div class="field-options">';
    foreach ($options as $option) {
      $attrs['value'] = $option;
      $checked = in_array($option, $default) ? ' checked="checked"' : '';
      $html .= ' <label>';
      $html .= '<input '.AH::asAttributes($attrs).$checked.' />';
      $html .= $option;
      $html .= '</label>'."\n";
    }
    $html .= '</div>';
    return $html;
  }

  private function renderTitle($bindings, $locale) {
    $title = $this->getProperty('label', $locale);
    $html = '<div class="form-section-title">'.$title.'</div>';
    return $html;
  }

  public static function captcha($failures) {
    $html = '';
    $classes  = array('form-field',
                      'form-field-captcha',
                      'form-field-required');
    if (in_array('captcha', $failures)) {
      $classes[] = 'form-field-failure';
    }
    $html .= '<div class="'.join(' ', $classes).'">'."\n";
    $html .= '<label>'.c(l('captcha')).'</label>'."\n";
    $html .= '<div class="form-field-control">'.W::captcha().'</div>'."\n";
    $html .= '<div class="form-field-help" style="display:none;">Veuillez indiquez</div>'."\n";
    $html .= '<div class="form-field-error" style="display:none;">Tous les caractères doivent être indiqués</div>'."\n";
    $html .= '</div>'."\n";
    return $html;
  }

  public static function asList($form) {
    if (!$form->isItem()) return '';
    $fields = $form->getFields();
    $html = '<ul>'."\n";
    if (count($fields) < 1) {
      $html .= '<li id="element_0" data-id="0" data-position="0">';
      $html .= ' <div><a href="javascript:void(0)">DROP HERE</a></div>';
      $html .= ' <div></div>';
      $html .= '</li>'."\n";
    }
    else {
      foreach ($fields as $field) {
        $html .= '<li id="xxx_'.$field->getId().'" data-id="'.$field->id.'" data-position="'.($field->position + 1).'">';
        $class = 'handle';
        $class .= $field->status < 1 ? ' offline' : '';
        $html .= '<div>';
        $html .= '<strong class="'.$class.' icon-reorder"></strong>';
        $html .= ' <a href="javascript:void(0)">'.$field->code.'</a></div>';
        $html .= ' <div></div>';
        $html .= '</li>'."\n";
      }
    }
    $html .= '</ul>'."\n";
    return $html;
  }

}
