<?php

class Translation extends ActiveRecord {

  const FLAG_PERFECTIBLE = 1;

  public static $flags = array(self::FLAG_PERFECTIBLE => 'perfectible');

  const STATUS_TO_TRANSLATE = 20;
  const STATUS_TO_REVISE = 40;
  const STATUS_DRAFT = 60;
  const STATUS_QUESTION = 80;
  const STATUS_PENDING_VALIDATION = 100;
  const STATUS_VALIDATED = 120;

  public static $statuses = array(self::STATUS_TO_TRANSLATE => 'à traduire',
                                  self::STATUS_TO_REVISE => 'à revoir',
                                  self::STATUS_DRAFT => 'en cours rédaction',
                                  self::STATUS_QUESTION => 'précision demandée',
                                  self::STATUS_PENDING_VALIDATION => 'en attente validation',
                                  self::STATUS_VALIDATED => 'accepté');

  const PRIORITY_NORMAL = 1;
  const PRIORITY_AVERAGE = 5;
  const PRIORITY_MAXIMUM = 10;

  public static $priorities = array(self::PRIORITY_NORMAL => 'normale',
                                    self::PRIORITY_AVERAGE => 'moyenne',
                                    self::PRIORITY_MAXIMUM => 'maximale');

  const SEO_TITLE = -1;
  const SEO_DESCRIPTION = -2;
  const SEO_KEYWORDS = -3;

  public static $seo_fields = array(self::SEO_TITLE => 'title',
                                    self::SEO_DESCRIPTION => 'description',
                                    self::SEO_KEYWORDS => 'keywords');

  public static $colors = array(self::STATUS_VALIDATED => 'green',
                                self::STATUS_PENDING_VALIDATION => 'blue',
                                self::STATUS_QUESTION => 'yellow',
                                self::STATUS_DRAFT => 'yellow',
                                self::STATUS_TO_REVISE => 'orange',
                                self::STATUS_TO_TRANSLATE => 'red');

  public function asFinderHash($h) {
    $h['priority'] = self::$priorities[$this->priority];
    return $h;
  }

  public function isValidated() {
    return $this->status == self::STATUS_VALIDATED;
  }

  public function isPendingValidation() {
    return $this->status == self::STATUS_PENDING_VALIDATION;
  }

  public function isToRevise() {
    return $this->status == self::STATUS_TO_REVISE;
  }

  public function isToTranslate() {
    return $this->status == self::STATUS_TO_TRANSLATE;
  }

  public function asPropertiesHash() {
    $item = $this->getItem();
    $h = Model::parameter(Model::name($item->getModelId()), 'properties');
    $h = array_flip($h);
    $properties = array();

    foreach ($this->properties as $locale => $hash) {
      foreach ($hash as $property_id => $property_value) {
        if ($property_id < 1) continue ;
        $name = $h[$property_id];
        $properties[$name] = $property_value;
      }
    }
    return $properties;
  }

  public function asSeoHash() {
    $seo = array();
    foreach ($this->properties as $locale => $h) {
      foreach ($h as $property_id => $property_value) {
        if ($property_id >= 0) continue ;
        $name = self::$seo_fields[$property_id];
        $seo[$name] = $property_value;
      }
    }
    return $seo;
  }

  public function getReferenceItemSeo() {
    $h = $this->dbo->fetchHash('SELECT title, description, keywords'
                               .' FROM '.Item::TABLE_SEO
                               .' WHERE model_id='.$this->model_id
                               . ' AND record_id='.$this->record_id
                               . ' AND locale IN ('.self::getReferenceLocale().', 0)'
                               .' ORDER BY locale DESC'
                               .' LIMIT 1');
    return $h;
  }

  public function asEditPropertiesHash($locale) {
    $item = $this->getItem();
    $filters = array('model_id' => $item->getModelId(),
                     '|!&|flags' => Property::FLAG_NO_TRANSLATION);
    $opts = array('filters' => $filters,
                  'order' => array('id' => 'asc'));
    $properties = Property::collect($opts);
    $h = array();

    foreach ($properties as $property) {
      $reference = $item->getProperty($property->name, self::getReferenceLocale());
      $rows = preg_match_all('[\r\n|\r|\n]', trim($reference), $matches);

      $input = array('name' => 'record[properties]['.$locale.']['.$property->getId().']',
                     'value' => $this->properties[$locale][$property->getId()],
                     'rows' => $rows);

      $h[] = array('name' => $property->name,
                   'reference' => $reference,
                   'input' => $input);
    }

    if ($this->hasSeo()) {
      $reference_seo = $this->getReferenceItemSeo();
      $hash = $this->dbo->fetchHash('SELECT '.implode(', ', self::$seo_fields)
                                    .' FROM '.Item::TABLE_SEO
                                    .' WHERE model_id='.$this->model_id
                                    .  ' AND record_id='.$this->record_id
                                    .  ' AND locale='.$locale);

      foreach (self::$seo_fields as $id => $field) {
        $reference = $reference_seo[$field];
        $rows = preg_match_all('[\r\n|\r|\n]', $reference, $matches);
        $input = array('name' => 'record[properties]['.$locale.']['.$id.']',
                       'value' => $this->properties[$locale][$id],
                       'rows' => $rows);
        $h[] = array('name' => 'seo.'.$field,
                     'reference' => $reference,
                     'input' => $input);
      }
    }
    return $h;
  }

  public function asRowName() {
    $item = $this->getItem();
    $label = Model::hasField($item->getModelId(), 'code') ? $item->code : $item->getName($this->locale);
    return '['.Model::name($item->getModelId()).'] '.$label;
  }

  public function asName() {
    $item = $this->getItem();
    $name = $this->asRowName();
    if (count($item->_belongs_to) == 1) {
      foreach ($item->_belongs_to as $model => $h) {
        try {
          $parent = $item->getOne($model);
          $parent_label = Model::hasField($parent->getModelId(), 'code')
                        ? $parent->code
                        : $parent->getName($this->locale);
          $name = '['.$model.'] '.$parent_label.' &raquo; '.$name;
        }
        catch (Exception $e) { return $name; }
      }
    }
    return $name;
  }

  public function getMd5() {
    $str = '';
    foreach ($this->properties as $locale => $h) {
      $str .= $locale;
      foreach ($h as $id => $content) {
        $str .= '-'.$id.'-'.h($content);
      }
    }
    return md5($str);
  }

  public function hasSeo() {
    $n = (int) $this->dbo->fetchValue('SELECT COUNT(record_id)'
                                      .' FROM '.Item::TABLE_SEO
                                      .' WHERE model_id='.$this->model_id
                                      .  ' AND record_id='.$this->record_id
                                      .  ' AND locale='.$this->locale
                                      .  ' AND LENGTH(title) > 3');
    return $n > 0;
  }

  // Updates -------------------------------------------------------------------

  public function updateItemProperties() {
    $item = $this->getItem();
    if (!$item->hasFlag($this->locale, 'locales')) {
      $item->setFlag($this->locale, 'locales');
    }
    $item->setProperties($this->asPropertiesHash(), $this->locale);
  }

  public function updateItemSeo() {
    $h = $this->asSeoHash();
    if (count($h) < 1) return ;
    $filters = array('model_id' => $this->model_id,
                     'record_id' => $this->record_id,
                     'locale' => $this->locale);
    $this->dbo->update(Item::TABLE_SEO, $h, array('filters' => $filters));

    $tags = SH::asSet($h['keywords']);
    $this->getItem()->setTags($tags, $this->locale);
  }

  // Kwo -----------------------------------------------------------------------

  public function asEditHash() {
    if (!$this->isItem()) return array();
    $html = '';
    $html .= '<table class="fixed">'."\n";
    $html .= '<thead>'."\n";
    $html .= '<tr>'."\n";
    $html .= '<th style="width:10%;">Propriété</th>'."\n";
    $html .= '<th style="width:45%;">Référence</th>'."\n";
    $html .= '<th style="width:45%;">Traduction</th>'."\n";
    $html .= '</tr>'."\n";
    $html .= '</thead>'."\n";
    $html .= '<tbody>'."\n";
    foreach ($this->asEditPropertiesHash($this->locale) as $property) {
      $html .= '<tr>'."\n";
      $html .= '<td>'.$property['name'].'</td>'."\n";
      $html .= '<td>'."\n";
      $html .=   '<div style="max-height:200px; overflow-y:auto; overflow-x:hidden;">'."\n";
      $html .=   h($property['reference']).'</div></td>'."\n";
      $html .= '<td>'."\n";
      $html .=   '<div style="max-height:200px; overflow-y:auto; overflow-x:hidden;">'."\n";
      $html .=   h($property['input']['value']).'</div></td>'."\n";
      $html .= '</tr>'."\n";
    }
    $html .= '</tbody>'."\n";
    $html .= '</table>'."\n";

    $item = $this->getItem();
    $link = Item::link($item, true);
    $grid = array('name' => $link,
                  'model' => $item->getModel(),
                  'id' => $item->id,
                  'langue' => Locale::toCode($this->locale).' <img src="'.PIX_URL.'/lang/'.$this->locale.'.png" />');//'langue' => I18N::locale2code($this->locale).' <img src="'.PIX_URL.'/lang/'.$this->locale.'.png" />');
    $h = array('properties' => $html,
               'grid' => $grid);
    return $h;
  }

  public function validate($h) {
    $item = $this->getItem();
    foreach ($h['properties'] as $locale => $values) {
      foreach ($values as $property_id => $content) {
        $property = Property::getInstance($property_id);
        $reference = $item->getProperty($property->name, self::getReferenceLocale());
        $reference_bindings = self::extractBindings($reference);
        $bindings = self::extractBindings($content);
        foreach ($reference_bindings as $binding) {
          if (!in_array($binding, $bindings)) {
            throw new Exception('Les bindings ne peuvent être supprimés : '.$binding);
          }
        }
      }
    }
    return true;
  }

  // Static --------------------------------------------------------------------

  public static function add($item, $locale, $opts=null) {
    $priority = (int) ($opts['priority'] ?: self::PRIORITY_NORMAL);
    $status = (int) ($opts['status'] ?: self::STATUS_TO_TRANSLATE);
    if (!array_key_exists($status, self::$statuses)) {
      S('log')->error('invalid status ('.$status.')', __METHOD__);
      return ;
    }
    $record = array('model_id' => $item->getModelId(),
                    'record_id' => $item->getId(),
                    'content' => $opts['content'],
                    'locale' => $locale,
                    'priority' => $priority);

    $id = (int) Translation::id(array('model_id' => $record['model_id'],
                                      'record_id' => $record['record_id'],
                                      'locale' => $locale));
    $id = $id > 0 ? $id : true;

    $translation = Translation::getInstance($id);
    $translation->commit($record);
    $translation->setStatus($status);

    if (User::exists($opts['user_id'])) {
      $translation->setUser($opts['user_id']);
    }
    return $translation;
  }

  public static function models() {
    $set = S('dbo')->asSet('SELECT DISTINCT model_id FROM '.T('translation'));
    $models = array();
    foreach ($set as $model_id) {
      if (Model::exists($model_id)) {
        $name = Model::name($model_id);
      }
      else {
        $name = '* model ('.$model_id.') *';
      }
      $models[$model_id] = $name;
    }
    return $models;
  }

  public static function getLocalizedModels() {
    $models = array();
    if (is_array(P('app.translation.models'))) {
      foreach (P('app.translation.models') as $name) {
        if (!Model::exists($name)) {
          S('log')->error('model unknown ['.$name.']', __METHOD__);
          continue ;
        }
        $models[$name] = $name;
      }
    }
    else {
      foreach (Model::all() as $id => $name) {
        if (!Model::hasField($name, 'locales') || Model::count($name) < 1) continue ;
        $models[$name] = $name;
      }
    }
    asort($models);
    return $models;
  }

  public static function getReferenceLocale() {
    $locales = I18N::locales();
    reset($locales);
    $locale = key($locales);
    return $locale ?: Locale::FR;
  }

  public static function locales() {
    $locales = I18N::locales();
    unset($locales[self::getReferenceLocale()]);
    return $locales;
  }

  public static function getEditorStatuses($mode) {
    $statuses = self::$statuses;
    unset($statuses[self::STATUS_VALIDATED]);
    if ($mode == 'edit') {
      unset($statuses[self::STATUS_TO_REVISE]);
      unset($statuses[self::STATUS_TO_TRANSLATE]);
    }
    return $statuses;
  }

  public static function extractBindings($str) {
    preg_match_all('/\(\([\w\-_]+\)\)/', $str, $matches);
    $bindings = current($matches);
    sort($bindings);
    return $bindings;
  }

  // Hooks ---------------------------------------------------------------------

  public function onAfterCommit($attrs) {
    $item = $this->getItem();
    $this->setAttribute('name', $item->getName());
  }

  public function onAfterStatusChange($old) {
    if ($old < self::STATUS_VALIDATED && $this->isValidated()) {
      $this->updateItemProperties();
      if ($this->hasSeo()) {
        $this->updateItemSeo();
      }
    }
  }

}