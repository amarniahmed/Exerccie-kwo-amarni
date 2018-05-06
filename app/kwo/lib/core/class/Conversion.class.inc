<?php

// Google : on a jusqu'au 8 du mois pour déclarer les leads

class Conversion extends ActiveRecord
{
  const FLAG_AUTOMATIC = 1;
  const FLAG_DUBIOUS = 2;

  public static $flags = array(self::FLAG_AUTOMATIC => 'automatique',
                               self::FLAG_DUBIOUS => 'douteux');

  const STATUS_CANCELED = -100;
  const STATUS_PENDING = 1;
  const STATUS_CONFLICT = 50;
  const STATUS_ADJUDGED = 100;

  public static $statuses = array(self::STATUS_ADJUDGED => 'attribué',
                                  self::STATUS_CONFLICT => 'en conflit',
                                  self::STATUS_PENDING => 'à attribuer',
                                  self::STATUS_CANCELED => 'annulé');

  const MEDIA_WEB = 1;
  const MEDIA_WEBSERVICE = 2;
  const MEDIA_PHONE = 3;
  const MEDIA_ADMIN = 4;
  const MEDIA_CALLCENTER = 5;

  public static $medias = array(self::MEDIA_WEB => 'web',
                                self::MEDIA_WEBSERVICE => 'webservice',
                                self::MEDIA_PHONE => 'téléphone',
                                self::MEDIA_ADMIN => 'backoffice',
                                self::MEDIA_CALLCENTER => 'call center');

  public static $types = array();

  public static $colors = array(self::STATUS_CANCELED => 'red',
                                self::STATUS_PENDING => 'yellow',
                                self::STATUS_CONFLICT => 'orange',
                                self::STATUS_ADJUDGED => 'green');

  public static function add($item, $opts=null) {
    $opts = (array) $opts;
    $model_id = 0;
    $record_id = 0;
    $visit_id = (int) $opts['visit_id'];
    $user_id = (int) $opts['user_id'];
    $media = (int) $opts['media'] ?: self::MEDIA_WEB;
    $app_id = (int) $opts['app_id'] ?: S('req')->getAppId();
    $medium = $source = $campaign = $term = 0;
    try {
      if ($item instanceof ActiveRecord) {
        $metas = $item->getMetas();
        $visit_id = $visit_id > 0 ? $visit_id : (int) $metas[Meta::FIELD_VISIT];
        $user_id = $user_id > 0 ? $user_id : (int) $metas[Meta::FIELD_USER];
        $model_id = $item->getModelId();
        $record_id = $item->getId();
      }
      elseif (is_array($item)) {
        $model_id = (int) $item['model_id'];
        $record_id = (int) $item['record_id'];
        $item = Item::getInstance($model_id, $record_id);
      }
      if ($model_id < 1 || $record_id < 1) {
        S('log')->trace('model_id ('.$model_id.'), '.
                        'record_id ('.$record_id.') required',
                        __METHOD__);
        return ;
      }

      $dbo = S('dbo');
      if ($media == self::MEDIA_WEB) {
        if ($visit_id < 1) {
          S('log')->trace('visit_id ('.$visit_id.')', __METHOD__);
          return ;
        }
        $set = $dbo->fetchArray('SELECT medium, source, campaign, term, number, layout, visitor_id'
                                .' FROM '.Visit::TABLE
                                .' WHERE id='.intval($visit_id));
        $medium = (int) $set[0];
        $source = (int) $set[1];
        $campaign = (int) $set[2];
        $term = (int) $set[3];
        $number = (int) $set[4];
        $layout = (int) $set[5];
        $visitor_id = (int) $set[6];
      }

      $tmp = array();
      $tmp[] = Model::id('order');
      $tmp[] = Model::id('user');
      if (in_array($model_id, $tmp)) {
        $row = array('model_id' => $model_id,
                     'record_id' => $record_id);
        if ($dbo->getTable(T('conversion'))->hasRow($row)) {
          return ;
        }
      }

      $flags = 0;
      $status = self::STATUS_PENDING;
      if ($number == 1) {
        $flags = self::FLAG_AUTOMATIC;
        $status = self::STATUS_ADJUDGED;
      }

      $row = array('model_id' => $model_id,
                   'record_id' => $record_id,
                   'visit_id' => $visit_id,
                   'winner_id' => $visit_id,
                   'visitor_id' => (int) $visitor_id,
                   'medium' => (int) $medium,
                   'source' => (int) $source,
                   'campaign' => (int) $campaign,
                   'term' => (int) $term,
                   'layout' => (int) $layout,
                   'flags' => $flags,
                   'status' => $status);

      if ($opts['unique'] == true) {
        if ($dbo->getTable(T('conversion'))->hasRow($row)) {
          // S('log')->trace('goal already exists', __METHOD__);
          return ;
        }
      }
      if (method_exists($item, 'getConversionWorth')) {
        $row['worth'] = $item->getConversionWorth($opts);
      }
      $row['user_id'] = (int) $user_id;
      $row['media'] = (int) $media;
      $row['visit_number'] = (int) $number;
      $row['app_id'] = $app_id;
      $row['nth_conversion_day'] = 1;
      $row['nth_visit_day'] = 1;
      $row['span'] = 0;

      if ($number > 1) {
        $n = $dbo->fetchValue('SELECT DISTINCT TO_DAYS(converted_at)'
                              .' FROM '.T('conversion')
                              .' WHERE visitor_id='.$visitor_id
                              .  ' AND visit_id<='.$visit_id);
        $row['nth_conversion_day'] = $n;
        $n = $dbo->fetchValue('SELECT DISTINCT TO_DAYS(created_on)'
                              .' FROM '.T('visit')
                              .' WHERE visitor_id='.$visitor_id
                              .  ' AND id<='.$visit_id);
        $row['nth_visit_day'] = $n;
        $n = $dbo->fetchValue('SELECT TO_DAYS("'.DH::today().'") - TO_DAYS(created_on)'
                              .' FROM '.T('visit')
                              .' WHERE visitor_id='.$visitor_id
                              .' ORDER BY id ASC'
                              .' LIMIT 1');
        $row['span'] = (int) $n;
      }

      $conversion_id = $dbo->insert(T('conversion'), $row);
      if ($dbo->affectedRows() >= 1) {
        if ($visit_id >= 1) {
          $visit = Visit::getInstance($visit_id);
          $visit->increment('conversions');
        }
        S('app')->hook('onConvert',
                       array('item' => $item,
                             'opts' => $opts));
      }
      $item->incrementMeta(Meta::FIELD_CONVERSIONS);
      if ($row['user_id'] >= 1) {
        try {
          $user = User::getInstance($row['user_id']);
          $user->setFlag(User::FLAG_CONVERSION);
        }
        catch (Exception $e) { S('log')->error($e, __METHOD__); }
      }
      $conversion = Conversion::getInstance($conversion_id);
      $conversion->setWinner();
    }
    catch (Exception $e) { S('log')->error($e, __METHOD__); }
  }

  public function setWinner($visit=null) {
    $status = 0;
    if ($visit === null) {
      if ($this->status != self::STATUS_PENDING) return ;
      $visit = Visit::getInstance($this->visit_id);
      $mediums = array(Tracker::MEDIUM_DIRECT, Tracker::MEDIUM_SEARCH);
      if (!in_array($visit->medium, $mediums)) return ;
      $mediums = array(Tracker::MEDIUM_CPC, Tracker::MEDIUM_SHOPPING, Tracker::MEDIUM_AFFILIATE);
      foreach ($visit->getSiblings(array('days' => 30)) as $row) {
        if (in_array($row['medium'], $mediums)) {
          $flags = $this->flags | self::FLAG_AUTOMATIC;
          $status = self::STATUS_ADJUDGED;
          $visit = Visit::getInstance($row['id']);
          break ;
        }
      }
      if ($status != self::STATUS_ADJUDGED) return ;
    }
    else {
      $status = self::STATUS_ADJUDGED;
      $flags = $this->flags & ~self::FLAG_AUTOMATIC;
      if (!($visit instanceof Visit)) {
        $visit = Visit::getInstance($visit);
      }
    }
    $attrs = array('winner_id' => $visit->getId(),
                   'medium' => $visit->medium,
                   'source' => $visit->source,
                   'campaign' => $visit->campaign,
                   'term' => $visit->term,
                   'layout' => $visit->layout,
                   'flags' => $flags,
                   'status' => $status,
                   'adjudged_at' => DH::now());
    $this->commit($attrs);
    $this->setCost();
  }

  public static function types() {
    $set = S('dbo')->asSet('SELECT DISTINCT model_id'
                           .' FROM '.T('conversion'));
    $types = array();
    foreach ($set as $model_id) {
      if (Model::exists($model_id)) {
        $name = Model::name($model_id);
      }
      else {
        $name = '* model ('.$model_id.') *';
      }
      $types[$model_id] = $name;
    }
    return $types;
  }

  public static function remove(ActiveRecord $item) {
    $visit_id = (int) $item->getMeta(Meta::FIELD_VISIT);
    if ($visit_id < 1) {
      S('log')->trace('item ['.$item->getModelId().':'.$item->getId().']'
                      .' should have a meta_visit > 0',
                      __METHOD__);
      return ;
    }
    $visit = Visit::getInstance($visit_id);
    $visit->decrement('conversions');
    try {
      $n = S('dbo')->exec('DELETE FROM '.T('conversion')
                          .    ' WHERE model_id='.$item->getModelId()
                          .      ' AND record_id='.$item->getId()
                          .      ' AND visit_id='.$visit_id);
      if ($n >= 1) {
        S('app')->hook('onConversionRemove',
                           array('item' => $item));
      }
    }
    catch (Exception $e) { S('log')->error($e, __METHOD__); }
  }

  public static function asTableRow($row) {
    $set = array();
    $html = '';
    return $html;
  }

  public function getName($locale=null) {
    return self::getType($this->model_id, $record_id);
  }

  public static function getType($model_id, $record_id) {
    $name = Model::name($model_id);
    if ($model_id == Model::id('goal') && $record_id > 0) {
      $name = self::$goals[$record_id];
    }
    return $name;
  }

  private static $strings = array();
  private static $goals = array();

  public function asFinderHash($h) {
    $h['type'] = self::getType($this->model_id, $this->record_id);
    if ($this->hasFlag(self::FLAG_AUTOMATIC)) {
      $h['type'] .= '*';
    }
    try {
      $item = Item::getInstance($this->model_id, $this->record_id,
                                array('ignore' => true));
      $h['name'] = $item->getName();
    }
    catch (Exception $e) {
      $h['name'] = '-';
    }
//    $h['media'] = self::$medias[$h['media']];
    $h['medium'] = Tracker::$mediums[$h['medium']];
    $h['source'] = self::$strings[$h['source']];
    if ($h['source'][0] == '|') {
      list (, $medium, $id) = explode('|', $h['source']);
      try {
        $item = Item::getInstance($medium, $id, array('ignore' => true));
        $h['source'] = $item->getName();
      }
      catch (Exception $e) {
        $model = Model::exists($medium) ? Model::name($medium) : $medium;
        $h['source'] = '** '.$model.' ('.$id.') **';
//        S('log')->error($e, __METHOD__);
      }
    }
    $h['term'] = self::$strings[$h['term']];
    $h['campaign'] = self::$strings[$h['campaign']];
    return $h;
  }

  public function onFind($finder) {
    $strings = array();
    foreach ($finder->getCollection() as $elt) {
      $strings[$elt->source]++;
      $strings[$elt->term]++;
      $strings[$elt->campaign]++;
    }
    self::$goals = Model::asHash('goal', 'code');
    $strings = array_keys($strings);
    self::$strings = $this->dbo->asHash('SELECT id, name'
                                        .' FROM '.Tracker::TABLE_EXPRESSION
                                        .' WHERE id IN ([id])',
                                        array('[id]' => $strings));
  }

  public function getStrings() {
    $strings = array($this->source, $this->term, $this->campaign);
    $strings = $this->dbo->asHash('SELECT id, name'
                                  .' FROM '.Tracker::TABLE_EXPRESSION
                                  .' WHERE id IN ([id])',
                                  array('[id]' => $strings));
    $h = array();
    $h[l('medium')] = Tracker::$mediums[$this->medium];
    $h[l('source')] = Tracker::asSourceLink($strings[$this->source]);
    $h[l('term')] = $strings[$this->term];
    $h[l('campaign')] = $strings[$this->campaign];
    return $h;
  }

  public function asEditHash() {
    $h = array();
    try {
      $item = $this->getItem();
      $h[l('type de conversion')] = Item::link($item);
      $h[l('intitulé de la conversion')] = $item->getName();
    }
    catch (Exception $e) {
      $h[l('type')] = '** '.Model::name($this->model_id).' ('.$this->record_id.')**';
      S('log')->error($e);
    }
    $h[l('date de la conversion')] = $this->converted_at;
    $h[l('visite associée')] = Visit::link($this->visit_id);
    if ($this->user_id > 0) {
      $h[l('user associé')] = Item::link(array('user', $this->user_id));
    }
    if ($this->span > 0) {
      $h[l('délai depuis 1ère visite')] = $this->span.' j ('.$this->visit_number.' visites)';
    }
    $origin = array();
    $origin[l('visite «gagnante»')] = Visit::link($this->winner_id);
    $origin += $this->getStrings();
    $origin[l('présentation')] = $this->layout ? Layout::name($this->layout) : '';
    return array('details' => $h,
                 'origin' => $origin,
                 'visits' => H::get('/back/core/conversion.visits', array('item' => $this->getKey())));
  }

  private function setCost() {
    $item = Tracker::asSourceItem($this->source);
    $cost = 0;
    if ($item instanceof ActiveRecord && method_exists($item, 'getConversionCost')) {
      $cost = $item->getConversionCost($this);
    }
    elseif ($this->medium === Tracker::MEDIUM_AFFILIATE) {
      $cost = 0.10;
    }
    $this->setAttribute('cost', $cost);
  }

  public static function asRows($input) {
    if ($input instanceof Visit) {
      $visit_id = $input->id;
      $rs = S('dbo')->query('SELECT c.*'
                            .' FROM '.T('conversion').' AS c'
                            .' WHERE c.visit_id='.$visit_id
                            .' ORDER BY c.id DESC');
      $rows = array();
      foreach ($rs as $row) {
        try {
          $h = array();
          $item = Item::getInstance($row['model_id'],
                                    $row['record_id']);
          $h[l('type')] = Item::link(array($row['model_id'], $row['record_id']));
          $h[l('nom')] = $item->getName();
          $h[l('valeur')] = $row['worth'];
          $h[l('heure')] = $row['converted_at'];
          $rows[] = $h;
        }
        catch (Exception $e) {
          $rows[] = $h;
        }
      }
      return $rows;
    }
  }

}