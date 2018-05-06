<?php

/*** definir un hook pour l'ajout d'objet
onProductCreate($product)
get_called_class
 ***/

class ActiveRecord
{
  protected $dbo = null;

  public $_args = array();
  public $_belongs_to = array();
  public $_cache = array();
  public $_changes = array();
  public $_foreign_key = null;
  public $_has_many = array();
  public $_has_one = array();
  public $_is_destroyed = false;
  public $_is_new = false;
  public $_locale = 0;
  public $_model = null;
  public $_model_id = null;
  public $_nucleus = null;
  public $_opts = array();
  public $_parts = null;
  public $_table = null;
  public $_urls = array();
  public $_verbose = true;

  public function __construct() {
    $this->dbo = S('dbo');
    $this->_cache['objects'] = array();
  }

  public function instantiate() {
    $this->onBeforeInstantiate();
    if ($this->getArguments() === null) return ;
    $where = '';
    $bindings = array();
    if (count($this->_args) === 1 && key($this->_args) === 'id') {
      $where = $this->_table.'.id='.intval(current($this->_args));
      $bindings = null;
    }
    else {
      foreach ($this->getArguments() as $key => $value) {
        if (!empty($where)) $where .= ' AND ';
        $col = strpos($key, '.') === false ? $this->_table.'.`'.$key.'`' : $key;
        if (is_int($value) || is_float($value)) {
          $where .= $col.'='.$value;
        }
        else {
          $where .= $col.'=:'.$key.':';
          $bindings[':'.$key.':'] = $value;
        }
      }
    }
    if ($this->_parts === null) {
      $this->_parts = $this->_getSqlParts(self);
    }
    $group = empty($this->_parts['group']) ? '' : ' GROUP BY '.$this->_parts['group'];
    if (!empty($this->_parts['where'])) {
      $where .= ' AND '.$this->_parts['where'];
    }
    $modifiers = '';
    $cache = false;
    if ($this->_model == 'snippet') {
      $cache = true;
    }
    $nucleus = $this->dbo->fetchObject('SELECT '.$modifiers.$this->_parts['cols']
                                       .' FROM '.$this->_parts['from']
                                       .' WHERE '.$where
                                       .$group
                                       .' LIMIT 1',
                                       $bindings,
                                       $cache);
    if (empty($nucleus)) {
      $this->_nucleus = null;
      if ($this->_verbose === true) {
        //        S('log')->backtrace();
        S('log')->error('element not found ['.$this->getModel().'] ('.$this->getModelId().')',
                        __METHOD__);
        S('log')->debug($this->getArguments(), __METHOD__);
      }
      throw new UnknownRecordException();
    }
    $this->_setNucleus($nucleus);
    $this->onAfterInstantiate();
  }

  public function _setNucleus($obj) {
    $this->_nucleus = is_array($obj) ? (object) $obj : $obj;
    $casts = Model::parameter($this->getModel(), 'casts');
    if (!is_array($casts)) return ;
    foreach ($casts as $key => $type) {
      $value = $this->_nucleus->{$key};
      if ($value === null) continue ;
      switch ($type) {
        case 'hash': $this->setNucleusValue($key, AH::decode($value)); break;
        case 'array': $this->setNucleusValue($key, AH::decode($value)); break;
        case 'set': $this->setNucleusValue($key, SH::asSet($value)); break;
      }
    }
    if (($session = S('req')->isTraced()) && $this->id > 0 &&
        !in_array($this->getModelId(), array(Model::id('admin'),
                                             Model::id('order'),
                                             Model::id('purchase'),
                                             Model::id('policy')))) {
      $req = S('req');
      if (!$req->isCli() && !$req->isBack()) {
        S('dbo')->insert(Item::TABLE_TRACER,
                         array('session' => $session,
                               'model_id' => $this->getModelId(),
                               'record_id' => $this->getId()));
      }
    }
  }

  protected function castAttributes(&$input) {
    $h = AH::duplicate($input);
    $casts = Model::parameter($this->getModel(), 'casts');
    if (!is_array($casts) || empty($h)) {
      return $h;
    }
    foreach ($casts as $key => $type) {
      if (!array_key_exists($key, $h)) continue ;
      $value = $h[$key];
      switch ($type) {
        case 'array': $h[$key] = AH::encode($value); break ;
        case 'hash': $h[$key] = AH::encode($value); break ;
        case 'set': $h[$key] = AH::toSet($value); break ;
        case 'date':
        case 'datetime':
          if (strpos($value, '/') !== false) {
            $h[$key] = DH::toDb($value);
          }
          elseif (empty($value)) {
            $h[$key] = '0000-00-00';
            if ($type == 'datetime') {
              $h[$key] .= ' 00:00:00';
            }
          }
          break ;
        case 'float':
          $value = is_numeric($value) ? $value : str_replace(',', '.', $value);
          $h[$key] = (float) $value;
          break ;
        case 'bits':
          $value = is_array($value) ? array_sum($value) : (int) $value;
          $h[$key] = $value;
          break ;
      }
    }
    return $h;
  }

  public function _getSqlParts($caller) {
    $group = '';
    $where = array();
    $from = $this->_table;
    $cols = array();
//    $cols = $this->_table.'.*';

    if (is_array($this->_opts['metas']) && count($this->_opts['metas']) >= 1) {
      $cols[] = 'md.'.join(', md.', $this->_opts['metas']);
      $from .= ' LEFT JOIN '.Meta::TABLE.' AS md'
                      .' ON (md.'.Meta::FIELD_MODEL.'='.$this->getModelId()
                      .' AND md.'.Meta::FIELD_RECORD.'='.$this->_table.'.id)';
    }

    if ($this->_opts['properties'] === true || count($this->_opts['properties']) >= 1) {
      $properties = Model::parameter($this->getModel(), 'properties');
      $this->_opts['properties'] = $this->_opts['properties'] === true
                                 ? array_keys($properties)
                                 : $this->_opts['properties'];
      $locale = $this->_locale >= 1 ? $this->_locale : S('res')->getLocale();
      $i = 0;
      foreach ($this->_opts['properties'] as $name) {
        $i++;
        $id = $properties[$name];
//        $cols .= ', p'.$i.'.value AS property_'.$name;
        $cols[] = 'p'.$i.'.value AS '.$name;
        $from .= ' LEFT JOIN '.Item::TABLE_PROPERTY.' AS p'.$i
                       .' ON (p'.$i.'.model_id='.$this->getModelId()
                       .' AND p'.$i.'.record_id='.$this->_table.'.id'
                       .' AND p'.$i.'.property_id='.intval($id)
                       .' AND p'.$i.'.locale IN (0, '.$locale.'))';
      }
    }

    foreach ($this->_opts['include'] as $index => $model) {
      if (!is_int($index)) {
        $relation = Model::parameters($index);
        $foreign_key = $model['foreign_key'];
        $model = $index;
      }
      else {
        $relation = Model::parameters($model);
        $foreign_key =& $relation['foreign_key'];
      }
      $table = T($model);
      if (isset($this->_belongs_to[$model])) {
        $from .= ' JOIN '.$table.' ON '.$table.'.id='.$this->_table.'.'.$foreign_key;
      }
      elseif (isset($this->_has_one[$model])) {
        $from .= ' JOIN '.$table.' ON '.$table.'.'.$foreign_key.'='.$this->_table.'.'.'id';
      }
      elseif (isset($this->_has_many[$model])) {
        $through = $this->_has_many[$model]['through'];
        if (strlen($through) < 1) {
          // mluberry 22/10/13
          if (isset($relation['belongs_to'][$this->_model])) {
            $model_foreign_key = $relation['belongs_to'][$this->_model]['foreign_key'];
            $from .= ' JOIN '.$table.' ON '.$table.'.'.$model_foreign_key.'='.$this->_table.'.id';
          }
          else{
            $from .= ' JOIN '.$table.' ON '.$table.'.id='.$this->_table.'.'.$foreign_key;
          }
        }
        else {
          $from .= ', '.$through.', '.$table;
          $where[] = $this->_table.'.id='.$through.'.'.$this->getModel().'_id AND '.$through.'.'.$model.'_id='.$table.'.id';
          if (!in_array($model, $this->_opts['exclude'])) {
            $cols[] = $through.'.'.$model.'_id AS '.$model.'_id, '.$through.'.'.$this->getModel().'_id AS '.$this->getModel().'_id';
          }
        }
      }
      else {
        $from .= ' JOIN '.$table.' ON '.$table.'.id='.$this->_table.'.'.$foreign_key;
      }
      if (!in_array($model, $this->_opts['exclude'])) {
        $cols[] = $relation['schema'];
      }
    }
    if (is_array($this->_opts['group']) && count($this->_opts['group']) >= 1) {
      $group = join(', ', $this->_opts['group']);
    }
    $cols[] = $this->_table.'.*';
    $cols = join(', ', $cols);
    return array('from' => $from, 'cols' => $cols, 'group' => $group, 'where' => implode(' AND ', $where));
  }

  public function setNucleusValue($key, $value) {
    $this->checkLoaded(__METHOD__);
    $this->_nucleus->{$key} = $value;
  }

  public function hook($method, $args) {
    if (!(method_exists($this, $method))) {
      return false;
    }
    return call_user_func(array($this, $method), $args);
  }

  public function getDatas() {
    return S('dbo')->asHash('SELECT type, value'
                            .' FROM '.Item::TABLE_DATA
                            .' WHERE model_id='.$this->getModelId()
                            .  ' AND record_id='.$this->getId());
  }

  public function getData($key) {
    return S('dbo')->fetchValue('SELECT value'
                                .' FROM '.Item::TABLE_DATA
                                .' WHERE model_id='.$this->getModelId()
                                .  ' AND record_id='.$this->getId()
                                .  ' AND type='.intval($key));
  }

  public function getDataParameter($key) {
    $params = S('dbo')->fetchValue('SELECT value'
                                   .' FROM '.Item::TABLE_DATA
                                   .' WHERE model_id='.$this->getModelId()
                                   .  ' AND record_id='.$this->getId()
                                   .  ' AND type='.Item::DATA_PARAMETERS);
    if (empty($params)) return null;
    $params = unserialize($params);
    return $params[$key];
  }

  public function getDataAttribute($key) {
    $attrs = S('dbo')->fetchValue('SELECT value'
                                  .' FROM '.Item::TABLE_DATA
                                  .' WHERE model_id='.$this->getModelId()
                                  .  ' AND record_id='.$this->getId()
                                  .  ' AND type='.Item::DATA_ATTRIBUTES);
    if (empty($attrs)) return null;
    $attrs = unserialize($attrs);
    return $attrs[$key];
  }

  public function hasData($key) {
    return S('dbo')->fetchValue('SELECT COUNT(*)'
                                .' FROM '.Item::TABLE_DATA
                                .' WHERE model_id='.$this->getModelId()
                                .  ' AND record_id='.$this->getId()
                                .  ' AND type='.intval($key)) > 0;
  }

  public function removeData($key) {
    $this->dbo->exec('DELETE FROM '.Item::TABLE_DATA
                     .' WHERE model_id='.$this->getModelId()
                     .  ' AND record_id='.$this->getId()
                     .  ' AND type='.intval($key)
                     .' LIMIT 1');
  }

  public function setData($key, $value) {
    if (empty($value)) {
      $this->removeData($key);
      return ;
    }
    $value = is_array($value) ? serialize($value) : $value;
    $dbo = S('dbo');
    $ret = $dbo->insert(Item::TABLE_DATA,
                        array('model_id' => $this->getModelId(),
                              'record_id' => $this->getId(),
                              'type' => $key,
                              'value' => $value),
                        array('modifiers' => 'ignore'));
    if ($dbo->affectedRows() < 1) {
      $filters = array('model_id' => $this->getModelId(),
                       'record_id' => $this->getId(),
                       'type' => $key);
      $dbo->update(Item::TABLE_DATA,
                   array('value' => $value,
                         'updated_at' => DH::now()),
                   array('filters' => $filters,
                         'limit' => 1));
    }
  }

  public function setDatas($h) {
    if (!is_array($h)) return false;
    foreach ($h as $key => $value) {
      $this->setData($key, $value);
    }
    return true;
  }

  public function incrementMeta($key) {
    if (!$this->hasMetas()) return ;
    return Meta::increment($this, $key);
  }

  public function decrementMeta($key) {
    if (!$this->hasMetas()) return ;
    return Meta::decrement($this, $key);
  }

  public function getMetas() {
    if (!$this->hasMetas()) return array();
    $args = func_get_args();
    $this->checkLoaded(__METHOD__.'('.join(',', $args).')');
    $metas = $keys = array();
    foreach ($args as $key) {
      if ($this->hasAttribute($key)) {
        $metas[$key] = $this->getAttribute($key);
      }
      else {
        $keys[] = $key;
      }
    }
    $h = Meta::get($this, $keys);
    foreach ($h as $key => $value) {
      $this->setNucleusValue($key, $value);
      $metas[$key] = $value;
    }
    return $metas;
  }

  public function getMeta($key) {
    $this->checkLoaded(__METHOD__.'('.$key.')');
    if ($this->hasAttribute($key)) {
      $value = $this->getAttribute($key);
    }
    else {
      $value = Meta::get($this, $key);
      $this->setNucleusValue($key, $value);
    }
    return $value;
  }

  public function setVisit($visit) {
    $id = $visit instanceof Request ? $visit->getVisitId() : (int) $visit;
//    d(N($this).'('.$this->id.') vis('.$id.')');
    $this->setMeta(Meta::FIELD_VISIT, $id);
  }

  public function setUser($user) {
    $this->checkLoaded(__METHOD__);
    $user = $user instanceof User
          ? $user
          : User::getInstance(intval($user));
    if ($this->hasAttribute('user_id')) {
      $this->setAttribute('user_id', $user->getId());
    }
    $this->setMeta(Meta::FIELD_USER, $user->getId());
    return $this;
  }

  public function setMeta($key, $value) {
    if (!$this->hasMetas()) return ;
    $this->setMetas(array($key => $value));
  }

  public function setMetas($metas) {
    if (!$this->hasMetas() || empty($metas)) return ;
//    $skip = array('latitude', 'longitude', 'status');
    $this->checkLoaded(__METHOD__);
    foreach ($metas as $name => $value) {
      Meta::set($this, $name, $value);
//      if (in_array($key, $skip)) continue ;
//      $this->setNucleusValue($name, $value);
    }
  }

  protected function hasMetas() {
    return Model::hasMetas($this->getModel());
  }

  /* http://en.wikipedia.org/wiki/Bit_manipulation */

  public function hasBit($bit) {
    if (!$this->hasMetas()) return false;
    $bit = is_array($bit) ? array_sum($bit) : (int) $bit;
    //    return (intval($this->getMeta(Meta::FIELD_BITS)) & $bit) > 0;
    return (intval($this->getMeta(Meta::FIELD_BITS)) & $bit) >= $bit;
  }

  public function setBit($bit) {
    if (!$this->hasMetas()) return ;
    $bits = (int) $this->getMeta(Meta::FIELD_BITS);
    $bits = $bits | $bit;
    $this->setMeta(Meta::FIELD_BITS, $bits);
  }

  public function unsetBit($bit) {
    if (!$this->hasMetas()) return ;
    $bits = (int) $this->getMeta(Meta::FIELD_BITS);
    $bits = $bits & ~$bit;
    $this->setMeta(Meta::FIELD_BITS, $bits);
  }

  public function hasFlag($flag, $column='flags') {
    $flag = is_array($flag) ? array_sum($flag) : $flag;
    $flag = $flag instanceof ActiveRecord ? NH::asBit($flag->getId()) : intval($flag);
    $flag = (int) $flag;
    return (intval($this->getAttribute($column)) & $flag) >= $flag;
  }

  public function toggleFlag($flag, $value, $column='flags') {
    $value = (bool) $value;
    if ($value === true) {
      $this->setFlag($flag);
    }
    else {
      $this->unsetFlag($flag);
    }
  }

  public function setFlag($flag, $column='flags') {
    if ($this->hasFlag($flag, $column)) return $this;
    $flag = $flag instanceof ActiveRecord ? NH::asBit($flag->getId()) : intval($flag);
    $value = $this->getAttribute($column);
    $value = $value | $flag;
    $this->setAttribute($column, $value);
    return $this;
  }

  public function unsetFlag($flag, $column='flags') {
    if (!$this->hasFlag($flag, $column)) return $this;
    $flag = $flag instanceof ActiveRecord ? NH::asBit($flag->getId()) : intval($flag);
    $value = $this->getAttribute($column);
    $value = $value & ~$flag;
    $this->setAttribute($column, $value);
    return $this;
  }

  public function getMask($column='flags', $decimal=false) {
    if (!$this->isItem()) return array();
    if (in_array($column, array('interests'))) {
      $value = $this->getMeta($column);
      $decimal = true;
    }
    else {
      $value = $this->getAttribute($column);
    }
    $bits = array();
    $value = (int) $value;
    if ($value <= 0) return $bits;
    for ($i = 0; $i <= 31; $i++) {
      $n = pow(2, $i);
      if ($n > $value) break ;
      if ($value & $n) $bits[] = $decimal ? ($i + 1) : $n;
    }
    return $bits;
  }

 /***
    qui a les notices 1 et 3
     select id from social_user where notices=(1 + 4);
    qui a la seule notice 3
     select id from social_user where notices=4;
    qui a la notice 3
     select id from social_user where notices & 4 > 0;
    supprimer notice 3
     update social_user set notices=notices-4
    ***/

  public function setMask($column, $bits, $dec=false) {
    if (is_object($bits) && $bits instanceof Collection) {
      $tmp = array();
      foreach ($bits as $item) {
        $tmp[] = NH::asBit($item->getId());
      }
      $bits = $tmp;
    }
    elseif ($dec === true) {
      foreach ($bits as &$value) $value = NH::asBit($value);
    }
    $value = array_sum($bits);
    if ($this->getAttribute($column) == $value) return ;
    return $this->setAttribute($column, $value);
  }

  public function getComments($opts=array()) {
    $this->checkLoaded(__METHOD__);
    $opts['include'] = 'user';
    $opts['metas'] = array(Meta::FIELD_CREATED_AT);
    $opts['filters'] = (array) $opts['filters'];
    $opts['filters']['model_id'] = $this->getModelId();
    $opts['filters']['record_id'] = $this->getId();
    return new Collection('comment', $opts);
  }

  public function isA($model_id) {
    return $this->getModelId() == Model::id($model_id);
  }

  public function belongsTo($item) {
    if (!$this->isItem()) return false;
    if (!($this->hasAttribute('model_id') &&
          $this->hasAttribute('record_id'))) return false;
    return $this->model_id == $item->getModelId() &&
           $this->record_id == $item->getId();
  }

  public function isAvailable() {
    return $this->isOnline();
  }

  public function isOnline() {
    if (!$this->isItem()) return false;
    return !$this->hasAttribute('status') || $this->status >= 1;
  }

  public function setLocale($locale=true) {
    $this->checkLoaded(__METHOD__);
    if ($locale === true) {
      $locale = S('res')->getLocale();
    }
    elseif ($locale instanceof User) {
      $locale = $locale->getLocale();
    }
    elseif ($locale instanceof Response) {
      $locale = $locale->getLocale();
    }
    elseif ($locale > 0) {
    }
    else {
      S('log')->error('invalid locale', __METHOD__);
      return ;
    }
    $this->commit(array('locale' => (int) $locale));
    return $this;
  }

  public function setStatus($status) {
    $this->checkLoaded(__METHOD__);
    $this->commit(array('status' => (int) $status));
    return $this;
  }

  public function increment($key, $quantity=1) {
    $this->checkLoaded(__METHOD__);
    $this->setAttribute($key, ($this->getAttribute($key) + $quantity));
    return $this;
  }

  public function decrement($key, $quantity=1) {
    $this->checkLoaded(__METHOD__);
    $this->setAttribute($key, $this->getAttribute($key) - $quantity);
    return $this;
  }

  private function checkLoaded($method) {
    if ($this->isItem()) return true;
    S('log')->error('object not loaded', $this->getModel().'::'.$method);
    throw new ClassRecordException('object not loaded');
  }

  /*** pivot ***/
  public function getPrev($check_association=true) {
    if (!$this->isItem() ||
        ($this->_cache['prev'] === null && !$this->hasPrev($check_association))) return null;
    return $this->_cache['prev'];
  }

  public function getNext($check_association=true) {
    if (!$this->isItem() ||
        ($this->_cache['next'] === null && !$this->hasNext($check_association))) return null;
    return $this->_cache['next'];
  }

  public function hasPrev($check_association=true) {
    if (!$this->isItem() || $this->_cache['prev'] === false) return false;
    $field = null;
    /*if ($this->hasAttribute('parent')) {
      $field = 'parent';
    }*/
    if ($check_association && count($this->_belongs_to) >= 1) {
      $h = current($this->_belongs_to);
      $field = $h['foreign_key'];
    }
    $where = $this->hasAttribute('status') ? 'status>=1' : '1';
    if ($field !== null) {
      $where .= ' AND '.$field.'='.intval($this->getAttribute($field));
    }
    $order = '';
    if ($this->hasAttribute('position')) {
      $where .= ' AND id<>'.$this->getId();
      $where .= ' AND position<'.$this->position;
      $order = ' ORDER BY position DESC';
    }
    elseif ($this->hasAttribute('published_at')) {
      $where .= ' AND id<>'.$this->getId();
      $where .= ' AND published_at<="'.$this->published_at.'"';
      $order = ' ORDER BY published_at DESC';
    }
 elseif ($this->hasAttribute('published_on')) {
      $where .= ' AND id<>'.$this->getId();
      $where .= ' AND published_on<=\''.$this->published_on.'\'';
      $order = ' ORDER BY published_on DESC';
    }
    else {
      $where .= ' AND id<'.$this->getId();
      $order = ' ORDER BY id DESC';
    }
    $nucleus = $this->dbo->fetchObject('SELECT * FROM '.$this->_table
                                       .' WHERE '.$where.$order.' LIMIT 1');
    if (empty($nucleus)) {
      $this->_cache['prev'] = false;
      return false;
    }
    $this->_cache['prev'] = clone $this;
    $this->_cache['prev']->_setNucleus($nucleus);
    return true;
  }

  public function hasNext($check_association=true) {
    if (!$this->isItem() || $this->_cache['next'] === false) return false;
    $field = null;
    /*if ($this->hasAttribute('parent')) {
      $field = 'parent';
    }*/
    $order = '';
    $where = $this->hasAttribute('status') ? 'status>=1' : '1';
    if ($check_association && count($this->_belongs_to) >= 1) {
      $h = current($this->_belongs_to);
      $field = $h['foreign_key'];
      $where .= ' AND '.$field.'='.intval($this->getAttribute($field));
    }
    if ($this->hasAttribute('position')) {
      $where .= ' AND id<>'.$this->getId();
      $where .= ' AND position>'.$this->position;
      $order = ' ORDER BY position ASC';
    }
    elseif ($this->hasAttribute('published_at')) {
      $where .= ' AND id<>'.$this->getId();
      $where .= ' AND published_at>"'.$this->published_at.'"';
      $order = ' ORDER BY published_at ASC, id ASC';
    }
    elseif ($this->hasAttribute('published_on')) {
      $where .= ' AND id<>'.$this->getId();
      $where .= ' AND published_on>"'.$this->published_on.'"';
      $order = ' ORDER BY published_on ASC, id ASC';
    }
    else {
      $where .= ' AND id>'.$this->getId();
      $order = ' ORDER BY id ASC';
    }
    $nucleus = $this->dbo->fetchObject('SELECT * FROM '.$this->_table
                                       .' WHERE '.$where.$order.' LIMIT 1');
    if (empty($nucleus)) {
      $this->_cache['next'] = false;
      return false;
    }
    $this->_cache['next'] = clone $this;
    $this->_cache['next']->_setNucleus($nucleus);
    return true;
  }

  public function getItem($opts=array()) {
    if ($this->_cache['item'] instanceof ActiveRecord) return $this->_cache['item'];
    if (!($this->hasAttributes('model_id', 'record_id') &&
          $this->model_id >= 1 && $this->record_id >= 1)) {
      throw new ActiveRecordException('no item inside');
    }
    $opts['ignore'] = true;
    $this->_cache['item'] = Item::getInstance((int) $this->model_id,
                                              (int) $this->record_id,
                                              $opts);
    return $this->_cache['item'];
  }

  protected function getItemName($full_flag=false) {
    $name = '';
    try {
      $item = $this->getItem();
      $name .= $item->getName();
      if ($full_flag === true) {
        $name .= ' ('.$item->getModel().')';
      }
    }
    catch (Exception $e) {
      if ($this->model_id >= 1) {
        $name = '('.Model::name($this->model_id).' ) ?';
      }
    }
    return $name;
  }

  public function getUser() {
    if ($this->_cache['user'] instanceof ActiveRecord) return $this->_cache['user'];
    $user_id = $this->user_id >= 1 ? $this->user_id : $this->getMeta(Meta::FIELD_USER);
    if ($user_id < 1) {
      throw new Exception('no user');
    }
    $this->_cache['user'] = User::getInstance($user_id);
    return $this->_cache['user'];
  }

  public function getVisit() {
    $visit_id = $this->hasAttribute('visit_id')
              ? $this->getAttribute('visit_id')
              : $this->getMeta(Meta::FIELD_VISIT);
    if ($visit_id < 1) {
      throw new Exception('no visit');
    }
    return Visit::getInstance($visit_id);
  }

  public function getThumbnail() {
    if ($this->hasAttribute('thumbnail')) return $this->thumbnail;
    if ($this->hasAttribute('image')) return $this->image;
    return null;
  }

  public function getImage() {
    if ($this->hasAttribute('image')) return $this->image;
    if ($this->hasAttribute('thumbnail')) return $this->thumbnail;
    return null;
  }

  public function getInterests() {
    return $this->isItem() ? $this->getMask('interests') : array();
  }

  public function setInterests($interests) {
    if (!is_array($interests)) return array();
    $this->checkLoaded(__METHOD__);
    $value = 0;
    foreach ($interests as $interest) {
      $value += NH::asBit($interest);
    }
    $this->setMeta(Meta::FIELD_INTERESTS, $value);
  }

  public function getFinderAttribute($key) {
    return $this->getAttribute($key);
  }

  public function setFinderAttribute($key, $value) {
    return $this->commit(array($key => $value));
  }

  public function isRoot() {
    return !$this->isItem() || $this->ancestor_id < 1;
  }

  public function isDestroyed() {
    return $this->_is_destroyed;
  }

  public function isDestroyable() {
    return true;
  }

  public function destroy() {

    $this->checkLoaded(__METHOD__);

    if ($this->isOrdered()) {
      throw new ActiveRecordException('an ordered item can not be destroyed');
    }

    if (!$this->isDestroyable()) {
      throw new ActiveRecordException('item can not be destroyed');
    }

    if ($this->hasBit(Meta::BIT_ETERNAL)) {
      S('log')->error('element not destroyable ['.$this->getModel().'] '.
                        '('.$this->getId().')', __METHOD__);
      throw new ActiveRecordException('élément protégé contre la suppression');
    }

    $this->onBeforeDestroy();

    $id = $this->getId();
    $uri = $this->getModelId();

    $models = array();
    if (is_array($this->_belongs_to)) {
      foreach ($this->_belongs_to as $model => $h) {
        $models[] = Model::id($model);
        $item = $this->getOne($model);
        try {
          $item->onRelationDestroy($this);
        }
        catch (Exception $e) { S('log')->error($e, __METHOD__); }
      }
    }

    if ($this->hasAttributes('model_id', 'record_id') &&
        $this->model_id >= 1 && $this->record_id >= 1 &&
        !in_array($this->model_id, $models)) {
      try {
        $this->getItem()->onRelationDestroy($this);
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }

    foreach ($this->_has_many as $model => $h) {
      if (strlen($h['through']) >= 2) {
        try {
          $this->dbo->exec('DELETE FROM '.$h['through']
                           .    ' WHERE `'.$this->_foreign_key.'`='.$id);
        }
        catch (Exception $e) { S('log')->error($e, __METHOD__); }
      }
      elseif ($h['dependent'] === true) {
        $filters = array($this->_foreign_key => $id);
        /*
        if ($h['structure'] === 'tree') {
          $filters['ancestor_id'] = 0;
          }
        */
        $opts = array('filters' => $filters);
        $collection = new Collection($model, $opts);
        foreach ($collection as $object) {
          $object->destroy();
        }
      }
    }

    $this->markAsChanged();

    $ret = $this->dbo->exec('DELETE FROM '.$this->_table
                            .    ' WHERE id='.$id
                            .    ' LIMIT 1');

    if ($ret < 1) {
      S('log')->error('failure ['.$this->getModel().' ('.$uri.')] ('.$id.')', _METHOD__);
      throw new ActiveRecordException('{'.$this->getModel().'} can not delete ('.$id.')');
    }

    $this->_is_destroyed = true;

    /*    S('log')->trace('[M='.$uri.':R='.$id.'] '
                      .strtoupper($this->getModel())
                      .' ['.$this.']', __METHOD__);*/

    foreach (P('item_tables') as $table) {
      try {
        if ($table == Meta::TABLE) {
          S('dbo')->exec('DELETE FROM '.$table
                         .    ' WHERE model='.$uri
                         .      ' AND record='.$id);
        }
        else {
          S('dbo')->exec('DELETE FROM '.$table
                         .' WHERE model_id='.$uri
                         .  ' AND record_id='.$id);
        }
      }
      catch (Exception $e) {
        S('log')->error('[M='.$uri.':R='.$id.'] '.$e->getMessage(), __METHOD__);
      }
    }

    try {
      $this->onAfterDestroy();
    }
    catch(Exception $e) { S('log')->error($e, __METHOD__); }

    try {
      $this->onAlter();
    }
    catch(Exception $e) { S('log')->error($e, __METHOD__); }

    S('app')->hook('onItemDestroy',
                   array('item' => $this));

  }

  public function getDescription($locale=null) {
    $this->checkLoaded(__METHOD__);
    $out = '';
    if ($this->hasAttribute('description')) {
      $out = $this->description;
    }
    return $out;
  }

  public function getName($locale=null) {
    if ($this->hasAttribute('name')) {
      return $this->getAttribute('name');
    }
    return $this->getAttribute('id');
  }

  public function getTitle($locale=null) {
    $this->checkLoaded(__METHOD__);
    return $this->hasAttribute('title') ? $this->title : $this->getName($locale);
  }

  public function setTags($tags, $locale=null) {
    $this->checkLoaded(__METHOD__);
    Tag::set($this, $tags, $locale);
    $this->markAsChanged();
  }

  public function getTags($locale=null) {
    if (!$this->isItem()) return array();
    $locale = $locale ?: $this->_locale();
    return $this->dbo->asSet('SELECT t.name'
                             .' FROM '.Item::TABLE_TAG.' AS i'
                             .' JOIN '.T('tag').' as t'
                             .  ' ON (t.id=i.tag_id)'
                             .' WHERE i.model_id='.$this->getModelId()
                             .  ' AND i.record_id='.$this->getId()
                             .  ' AND i.locale='.$locale);
  }

  public function asText($locale=null) {
    $fields = array('title', 'name', 'label',
                    'description', 'summary', 'content');
    $out = '';
    foreach ($fields as $field) {
      if (!$this->hasAttribute($field) ||
          strlen($this->getAttribute($field)) < 1) continue ;
      $out .= $this->getAttribute($field)."\n";
    }
    $tags = $this->getTags($locale);
    if (!empty($tags)) {
      $out .= join(', ', $tags);
    }
    return $out;
  }

  public function asDocument($locale) {
    $document = new Document($this, $locale);
    return $document;
  }

  public function asDocumentHash($locale) {
    $this->checkLoaded(__METHOD__);
    if (Model::hasProperties($this)) {
      $name  = $this->getProperty('title', $locale);
      $name .= $this->getProperty('name', $locale);
      $name .= $this->getProperty('label', $locale);
      $description = $this->getProperty('description', $locale);
      $content = $this->getProperty('content', $locale);
    }
    else {
      $name  = $this->title;
      $name .= $this->name;
      $name .= $this->label;
      $description = $this->description;
      $content = $this->content;
    }
    $h = array('model_id' => $this->getModelId(),
               'record_id' => $this->getId(),
               'name' => $name,
               'description' => $description,
               'content' => $content,
               'keywords' => $this->getTags($locale),
               'indexes' => array(),
               'word_count' => 0);
    return $h;
  }

  public function asExportHash() {
    return $this->getAttributes();
  }

  public function asSuggestionHash($locale) {
    $this->checkLoaded(__METHOD__);
    $locale = $locale ?: $this->getLocale();
    return array('name' => $this->getName($locale),
                 'label' => $this->getName($locale),
                 'locale' => $locale,
                 'content' => $this->asText($locale),
                 'attributes' => array('item' => $this->getKey()));
  }

  public function asIdentityHash($locale, $type=null, $options=null) {
    return array('name' => $this->getName($locale),
                 'description' => $this->getDescription($locale),
                 'image' => $this->getThumbnail());
  }

  public function asView($view) {
    if ($view == 'id') return $this->getId();
    if ($view == 'item') {
      return c(Model::name($this)).' ('.$this->getId().')</br>'.$this->getName();
    }
    return $this->getName();
  }

  public function asSearchHash($locale) {
    $h = array();
    $h['title'] = $this->getTitle();
    if ($this->hasAttribute('description')) {
      $h['description'] = $this->description;
    }
    if ($this->hasAttribute('thumbnail', true)) {
      $h['image'] = $this->thumbnail;
    }
    elseif ($this->hasAttribute('image', true)) {
      $h['image'] = $this->image;
    }
    $h['updated_at'] = $this->getMeta(Meta::FIELD_UPDATED_AT);
    $h['created_at'] = $this->getMeta(Meta::FIELD_CREATED_AT);
    $h['url'] = $this->asUrl();
    return $h;
  }

  public function asHeadHash($locale) {
    $h = $this->dbo->fetchHash('SELECT title, description, keywords'
                               .' FROM '.Item::TABLE_SEO
                               .' WHERE model_id='.$this->getModelId()
                               .  ' AND record_id='.$this->getId()
                               .  ' AND locale IN ('.$locale.', 0)'
                               .' ORDER BY locale DESC'
                               .' LIMIT 1');
    $h = (array) $h;
    $h['url'] = $this->asUrl($locale);
    return $h;
  }

  public function asRelationRow($relation, $row) {
    return $relation->getName();
  }

  public function asConversionName() {
    return $this->getModel().' ('.$this->getId().') : '.$this->getName();
  }

  public function asPickerHash() {
    $attrs = array();
    $attrs['id'] = $this->getId();
    $attrs['name'] = $this->getName() ?: $this->getModel().' ('.$this->getId().')';
    $attrs['model_id'] = $this->getModelId();
    $attrs['record_id'] = $this->getId();
    $attrs['model'] = $this->getModel();
    $attrs['key'] = $this->getKey();
    $attrs['item'] = $attrs['key'];
    $attrs['url'] = $this->asUrl();
    $attrs['attrs'] = $this->getAttributes();
    return $attrs;
  }

  public function asPickerRow() {
    $out = '';
    if (!empty($this->code)) {
      $out = $this->code.' : ';
    }
    $out .= $this->getName();
    return $out;
  }

  public function isIndexable() {
    return $this->isItem() && $this->isOnline();
  }

  public function index() {
    if (!$this->isItem()) return ;
    Engine::getIndexer()->index($this);
  }

  public function markAsChanged() {
    if (!$this->isItem()) return ;
    $models = (array) P('search.models');
    foreach ($models as &$model) {
      $model = Model::id($model);
    }
    if (in_array($this->getModelId(), $models)) {
      try {
        Document::setStatus($this);
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    $this->onChange();
    $this->markAsEvolved();
  }

  public function markAsEvolved() {
    if (!$this->isItem()) return ;
    $this->incrementMeta(Meta::FIELD_STATE);
    //    Cache::remove($this);
  }

  public function duplicate() {
    $h = $this->getAttributes();
    $this->onBeforeDuplicate($h);
    foreach (array('id') as $field) {
      unset($h[$field]);
    }
    $item = Item::getInstance((int) $this->getModelId());
    $item->commit($h);
    $rs = $this->dbo->query('SELECT *'
                            .' FROM '.Item::TABLE_PROPERTY
                            .' WHERE model_id='.$this->getModelId()
                            .  ' AND record_id='.$this->getId());
    foreach ($rs as $h) {
      unset($h['id']);
      $h['record_id'] = $item->getId();
      $this->dbo->insert(Item::TABLE_PROPERTY, $h);
    }
    $this->onAfterDuplicate($item);
    return $item;
  }

  public function hasLocale($locale) {
    $locale = (int) $locale;
    if ($this->hasAttribute('locales')) {
      return ($this->locales & $locale) >= $locale;
    }
    if ($this->hasAttribute('locale')) {
      return $this->locale == $locale;
    }
    return false;
  }

  public function getLocale() {
    return $this->hasAttribute('locale')
         ? (int) $this->getAttribute('locale')
         : S('app')->getLocale();
  }

  public function getLocales() {
    $this->checkLoaded(__METHOD__);
    $locales = array();
    if ($this->hasAttribute('locales')) {
      $locales = $this->locales > 0 ? $this->getMask('locales') : array();
    }
    elseif ($this->hasAttribute('locale')) {
      $locales = array($this->locale);
    }
    return $locales;
  }

  public function onComment($comment) {}

  protected function onPriceChange() {
    if ($this instanceof Purchasable) {
      Discount::applyCatalogDiscounts($this);
    }
  }

  public function onRelationCreate($item) {}
  public function onRelationUpdate($item) {}
  public function onRelationDestroy($item) {}

  public function onResourceCreate($resource) {}
  public function onResourceUpdate($resource) {}
  public function onResourceDestroy($resource) {}

//  public function onFind($finder) {}
//  public function onCollect($finder) {}

  public function onLetterSend() {}
  public function onTrack($type, $opts) {}
  public function onChange() {}

  protected function onBeforeDuplicate(&$attrs) {}
  protected function onAfterDuplicate($new_item) {}

  protected function onAdd(ActiveRecord $item) {}
  protected function onAlter() {}
  protected function onRemove(ActiveRecord $item) {}

  protected function onOnline() {}
  protected function onOffline() {}

  protected function onBeforeCommit(&$attrs) {}
  protected function onAfterCommit($attrs) {}

  protected function onBeforeCreate(&$attrs) {}
  protected function onAfterCreate() {}

  protected function onBeforeDestroy() {}
  protected function onAfterDestroy() {}

  protected function onBeforeInstantiate() {}
  protected function onAfterInstantiate() {}

  protected function onBeforeUpdate(&$attrs) {}
  protected function onAfterUpdate(&$attrs) {}

  protected function onBeforeStatusChange(&$attrs) {}
  protected function onAfterStatusChange($old) {}

  protected function onBeforeFlagChange($flag, $value) {}
  protected function onAfterFlagChange($flag, $value) {}

  public function hasChanged() {
    return count($this->_changes) >= 1;
  }

  public function prior($key) {
    return $this->_changes[$key];
  }

  public function changes() {
    return $this->_changes;
  }

  public function commit($attrs) {
    if (empty($attrs)) {
      $attrs = array();
    }
    $model = $this->getModel();
    $this->onBeforeCommit($attrs);
    if (isset($attrs['id'])) { unset($attrs['id']); }
    $old_status = null;
    if (isset($attrs['status']) && (!$this->isItem() || $attrs['status'] != $this->status)) {
      $old_status = $this->status;
      $this->onBeforeStatusChange($attrs);
    }
    $price_change = false;
    if (isset($attrs['price']) || isset($attrs['original_price'])) {
      if (!$this->isItem()) $price_change = true;
      else {
        if (isset($attrs['price']) && $attrs['price'] != $this->price) {
          $price_change = true;
        }
        if (isset($attrs['original_price']) && $attrs['original_price'] != $this->original_price) {
          $price_change = true;
        }
      }
    }
    if (strlen($attrs['image']) >= 5 &&
        isset($attrs['thumbnail']) && empty($attrs['thumbnail']) &&
        preg_match('#^/'.DOC_PATH.'/#u', $attrs['image'])) {
      try {
        $thumbnail_name = 'thumb-'.basename($attrs['image']);
        $thumb = Img::getInstance($attrs['image'])->copyTo($thumbnail_name, true);
        $box_size = P('thumbnail.box.size') ?: 50;
        $thumb->setBox($box_size);
        $attrs['thumbnail'] = '/'.$thumb->getName();
      }
      catch (Exception $e) { }
    }
    $metas = array(Meta::FIELD_UPDATED_AT => DH::now());
    if ($this->isItem()) {
      $is_online = $this->isOnline();
      if (isset($attrs['locale']) && $attrs['locale'] < 1) {
        $attrs['locale'] = S('app')->getLocale();
      }

      $this->onBeforeUpdate($attrs);
      $row = $this->castAttributes($attrs);
      $ret = $this->dbo->update($this->_table, $row,
                                array('filters' => array('id' => $this->getId()),
                                      'limit' => 1));
      $alterations = array();
      foreach ($row as $key => $value) {
        $old = $this->getAttribute($key);
        $this->_changes[$key] = $old;
        if ($value != $old) {
          $alterations[$key] = array($old, $value);
        }
        $this->_nucleus->{$key} = $value;
      }
      $this->_is_new = false;
      $this->saveVersion($alterations);
      $this->setArguments(array('id' => $this->getId()));
      $this->instantiate();
      $this->onAfterUpdate($attrs);
    }
    else {
      $is_online = false;
      if (!Model::hasField($model, 'scheduled_at')) {
        if (Model::hasField($model, 'published_at') && empty($attrs['published_at'])) {
          $attrs['published_at'] = DH::now();
        }
        if (Model::hasField($model, 'published_on') && empty($attrs['published_on'])) {
          $attrs['published_on'] = DH::today();
        }
      }
      if (Model::hasField($model, 'app_id') && $attrs['app_id'] < 1) {
        $attrs['app_id'] = S('req')->getAppId();
      }
      if (Model::hasField($model, 'locale') && $attrs['locale'] < 1) {
        $attrs['locale'] = S('app')->getLocale();
      }
      if (TreeManager::isNode($this)) {
        TreeManager::onNodeCreate($this, $attrs);
      }
      $this->onBeforeCreate($attrs);
      foreach ($this->_belongs_to as $relation) {
        if ($attrs[$relation['foreign_key']] < 1) {
          S('log')->error('creation failure ['.$model.'],'
                            .' foreign_key missing ['.$relation['foreign_key'].']', __METHOD__);
          throw new ClassRecordException('creation failure : foreign_key missing');
        }
      }

      $id = $this->dbo->insert($this->_table,
                               $this->castAttributes($attrs));
      if ($id === DataBaseObject::ERROR_DUPLICATION) {
        throw new DuplicateRecordException('duplicate record ['.$model.']');
      }
      if ($id === false) {
        S('log')->error('insert failure ['.$model.']', __METHOD__);
        throw new ClassRecordException('insertion failure');
      }
      $this->_is_new = true;
      $this->setArguments(array('id' => $id));
      $this->instantiate();
      $this->onAfterCreate();
      // ajout mluberry 28/11/12
      S('app')->hook('onAfterCreate',
                     array('item' => $this,
                           'attrs' => $attrs));
      $metas[Meta::FIELD_CREATED_AT] = DH::now();
    }

    $models = array();
    if (is_array($this->_belongs_to)) {
      $relation = reset($this->_belongs_to);
      if ($attrs[$relation['foreign_key']] >= 1) {
        $metas[Meta::FIELD_PARENT] = (int) $attrs[$relation['foreign_key']];
      }
      foreach ($this->_belongs_to as $model => $h) {
        $models[] = Model::id($model);
        try {
          $item = $this->getOne($model);
          if ($this->_is_new === true) {
            $item->onRelationCreate($this);
          }
          else {
            $item->onRelationUpdate($this);
          }
        }
        catch (Exception $e) { S('log')->error($e, __METHOD__); }
      }
    }

    if ($this->hasAttributes('model_id', 'record_id') &&
        $this->model_id >= 1 && $this->record_id >= 1 &&
        !in_array($this->model_id, $models)) {
      try {
        $item = $this->getItem();
        if ($this->_is_new === true) {
          $item->onRelationCreate($this);
        }
        else {
          $item->onRelationUpdate($this);
        }
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }

    foreach (array(Meta::FIELD_FLAGS, Meta::FIELD_STATUS, 'user_id') as $field) {
      if (!isset($attrs[$field])) continue ;
      $field = $field === 'user_id' ? Meta::FIELD_USER : $field;
      $metas[$field] = is_array($attrs[$field])
                     ? array_sum($attrs[$field])
                     : (int) $attrs[$field];
    }
    foreach (array(Meta::FIELD_LATITUDE, Meta::FIELD_LONGITUDE) as $field) {
      if (!isset($attrs[$field])) continue ;
      $metas[$field] = (float) $attrs[$field];
    }
    if (isset($attrs['locales'])) {
      $metas[Meta::FIELD_LOCALES] = is_array($attrs['locales'])
                                  ? array_sum($attrs['locales'])
                                  : (int) $attrs['locales'];
    }
    elseif (isset($attrs['locale'])) {
      $metas[Meta::FIELD_LOCALES] = (int) $attrs['locale'];
    }

    $this->setMetas($metas);
    $this->incrementMeta(Meta::FIELD_REVISION);
    $this->onAfterCommit($attrs);

    if ($old_status !== null) {
      $this->onAfterStatusChange($old_status);
    }
    if ($price_change === true) {
      $this->onPriceChange();
    }
    if ($this->hasAttribute('status')) {
      if ($is_online === false && $this->isOnline()) {
        $this->onOnline();
      }
      elseif ($is_online === true && !$this->isOnline()) {
        $this->onOffline();
      }
    }
    $this->markAsChanged();
    $this->onAlter();
    S('app')->hook('onAfterCommit',
                   array('item' => $this,
                         'attrs' => $attrs));
    return $this;
  }

  protected function saveVersion($attrs) {
    if (empty($attrs)) return ;
    if (Model::parameter($this, 'revision') != true) {
      return ;
    }
    try {
      $req = S('req');
      $record = array('model_id' => $this->getModelId(),
                      'record_id' => $this->getId());
      if ($req->isBack()) {
        $record['creator_model_id'] = Model::id('admin');
        $record['creator_record_id'] = S('admin')->getId();
      }
      else {
        $record['creator_model_id'] = Model::id('user');
        if ($req->isAuthenticated()) {
          $record['creator_record_id'] = $req->getUserId();
        }
      }
      $record['action'] = S('req')->getPath();
      $record['attributes'] = serialize($attrs);
      $this->dbo->insert(Item::TABLE_REVISION, $record);
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
  }

  public function asChannel() {
    $channel = array();
    $channel['id'] = $this->getKey();
    $channel['title'] = (string) $this;
    $channel['description'] = (string) $this->description;
    $channel['link'] = $this->asUrl();
    $channel['copyright'] = P('copyright');
    $channel['ttl'] = 240;
    if ($this->hasAttribute('image')) {
      $channel['image'] = $this->image;
    }
    $channel['updated_at'] = $this->getMeta(Meta::FIELD_UPDATED_AT);
    return $channel;
  }

  public function asFeed($entries=null) {
    $opts = array();
    if (!$this->isOnline()) {
      S('log')->error('offline item ['.$this->getModel().']'
                        .           ' ('.$this->getId().')',
                        __METHOD__);
      throw new Exception(l('error.item.invalid'));
    }
    if ($entries === null) {
      if (method_exists($this, 'getEntries')) {
        $entries = $this->getEntries();
      }
      elseif (count($this->_has_many)) {
        $model = key($this->_has_many);
        $opts['last'] = 10;
        if (Model::hasField($model, 'status')) {
          $opts['filters'] = array('|>|status' => 0);
        }
        $entries = $this->getItems($model, $opts);
      }
      else {
        $entries = $this->getComments(array('last' => 10));
      }
    }
    return FeedWriter::getInstance($this, $entries);
  }

  public function asFeedHash() {
    $feed = array();
    $feed['id'] = $this->getId();
    $feed['title'] = $this->getName();
    if ($this->hasAttribute('description')) {
      $feed['description'] = $this->description;
    }
    elseif ($this->hasAttribute('content')) {
      //      $feed['description'] = $this->content;
      $feed['description'] = H::ellipse($this->content,
                                        array('length' => 200));
    }
    if ($this->hasAttribute('author')) {
      $feed['author'] = $this->author;
    }
    elseif ($this->hasAttribute('user_id') && $this->user_id >= 1) {
      $feed['author'] = $this->getUser()->name;
    }
    elseif ($this->hasAttribute('author_id') && $this->author_id >= 1) {
      $feed['author'] = $this->getAuthor()->name;
    }
    $feed['link'] = $this->asUrl().'?knxm='.Tracker::MEDIUM_RSS.'&knxt='.urlencode($feed['title']);
    $feed['guid'] = $this->getModelId().'-'.$this->getId();
    $feed['category'] = join(',', $this->getTags());
    $feed['updated_at'] = $this->getMeta(Meta::FIELD_UPDATED_AT);
    return $feed;
  }

  public function asAutocompleteName($from=null) {
    return $this->getName();
  }

  public function convert($opts=null) {
    Conversion::add($this, $opts);
  }

  public function track($interaction=Interaction::TYPE_DISPLAY, $value=0, $opts=null) {
    $opts = (array) $opts;
    if ($value > 0) {
      $opts['value'] = (int) $value;
    }
    if ($interaction === Interaction::TYPE_DISPLAY) {
      $agent = S('req')->getClient('agent');
      if (preg_match('/(google|bing)bot/', $agent) === 1) {
        Item::log($this, Item::LOG_TYPE_CRAWL);
      }
      elseif (!preg_match('/crawl|bot/', $agent)) {
        Item::log($this, Item::LOG_TYPE_IMPRESSION);
      }
    }
    return Interaction::track($this, $interaction, $opts);
  }

  public function log($type=null) {
    return Item::log($this, $type);
  }

  public function getId() {
    return $this->getAttribute('id');
  }

  public function getBit() {
    return NH::asBit($this->getAttribute('id'));
  }

  public function getKey() {
    return Item::asKey($this);
  }

  public function getModel() { return $this->_model; }

  public function getModelId() { return (int) $this->_model_id; }

  public function getUrlBindings($keys, $locale) {
    $model = $this->getModel();
    $bindings = array();
    foreach ($keys as $key) {
      $value = '';
      if (Model::hasField($model, $key)) $value = $this->getAttribute($key);
      elseif (Model::hasProperty($model, $key)) $value = $this->getProperty($key, $locale);
      $bindings[$key] = $value;
    }
    return $bindings;
  }

  /** 2083 characters max **/
  public function asUrl($locale=null) {
    $locale = $locale ?: S('res')->getLocale();
    if (!empty($this->_urls[$locale])) return $this->_urls[$locale];
    if ($this->_urls[$locale] === false) {
      S('log')->error('recursive call ['.$this->getModel().']', __METHOD__);
      return '/';
    }
    $this->_urls[$locale] = false;
    $manager = S('app')->getManager();
    $method = 'url';
    $url = null;
    if ($this->getModelId() < Extension::THRESHOLD && $manager && method_exists($manager, $method)) {
      try {
        $args = array('locale' => $locale,
                      'url' => $this);
        $url = call_user_func(array($manager, $method), $args);
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    else {
      if ($this->isA('message')) {
        $url = '/account/message?id='.$this->getId();
      }
      elseif ($this->isA('post')) {
        $parts = DH::asHash($this->published_at);
        $url = join('/', array($parts['year'], $parts['month'], $parts['day'], $this->code));
      }
      elseif ($this->isA('url')) {
        if (strpos($this->name, '//') === false) {
          $url = H::url($this->name);
        }
        else {
          $url = $this->name;
        }
      }
      elseif ($this->isA('translation')) {
        $url = '/translator/-/id/'.$this->getId().'/locale/'.$this->locale;
      }
    }
    if (empty($url)) {
      $url = '/' . $this->getModel() . '?id=' . $this->getId();
    }
    $this->_urls[$locale] = $url;
    return $url;
  }

  public function debug() { S('log')->debug($this->getAttributes(), __METHOD__); }

  public function isOrderedBy($user) {
    $user_id = $user instanceof User ? $user->getId() : (int) $user;
    $id = S('dbo')->fetchValue('SELECT p.id'
                               .' FROM '.T('purchase').' AS p,'
                               .     ' '.T('order').' AS o'
                               .' WHERE p.model_id='.$this->getModelId()
                               .  ' AND p.record_id='.$this->getId()
                               .  ' AND p.order_id=o.id'
                               .  ' AND o.user_id='.$user_id
                               .  ' AND o.status>='.Order::STATUS_PAID);
    return $id >= 1;
  }

  public function isOrdered() {
    $id = S('dbo')->fetchValue('SELECT p.id'
                               .' FROM '.T('purchase').' AS p,'
                               .     ' '.T('order').' AS o'
                               .' WHERE p.model_id='.$this->getModelId()
                               .  ' AND p.record_id='.$this->getId()
                               .  ' AND p.order_id=o.id'
                               .  ' AND o.status>='.Order::STATUS_PAID);
    return $id >= 1;
  }

  public function isItem() { return $this->_nucleus !== null; }

  public function isNew() { return $this->_is_new === true; }

  public function getResources($opts=null) {
    if (!$this->isItem()) throw new ActiveRecordException();
    $opts = (array) $opts;
    if (!isset($opts['order'])) {
      $opts['order'] = array('position' => 'asc');
    }
    $opts['filters']  = (array) $opts['filters'];
    if (empty($opts['filters']['|>=|status'])) {
      $opts['filters']['|>=|status'] = Resource::STATUS_ONLINE;
    }
    $opts['filters'] += array('model_id' => $this->getModelId(),
                              'record_id' => $this->getId());
    return Item::collect('resource', $opts);
  }

  public function setProperty($name, $value, $locale=null) {
    $locale = $this->_locale($locale);
    return $this->setProperties(array($name => $value), $locale);
  }

  public function setProperties($h, $locale=null) {
    $locale = $this->_locale($locale);
    $properties = $this->getPropertiesById();
    $row = array('model_id' => $this->getModelId(),
                 'record_id' => $this->getId(),
                 'locale' => $locale);
    foreach ($h as $name => $value) {
      $property_id = Model::property($this, $name);
      $row['property_id'] = $property_id;
      if (isset($properties[$locale][$property_id])) {
        $this->dbo->update(Item::TABLE_PROPERTY,
                           array('value' => $value,
                                 'updated_at' => DH::now()),
                           array('filters' => $row));
      }
      else {
        $row['value'] = $value;
        $this->dbo->insert(Item::TABLE_PROPERTY, $row);
        unset($row['value']);
      }
      if (property_exists($this->_nucleus, $name)) {
        $this->_nucleus->{$name} = $value;
      }
    }
    $this->_cache['properties'][$locale] = null;
    $this->setMetas(array(Meta::FIELD_UPDATED_AT => DH::now()));
    $this->incrementMeta(Meta::FIELD_REVISION);
    $this->markAsChanged();
    $this->onAlter();
    return $this;
  }

  public function getProperty($key, $locale=null) {
    $locale = $this->_locale($locale);
/*    if ($this->hasAttribute('property_'.$key)) {
      return $this->getAttribute('property_'.$key);
    } */
    if ($this->hasAttribute($key)) {
      return $this->getAttribute($key);
    }
    if ($this->_cache['properties'][$locale] === null) {
      $this->getProperties($locale, false);
    }
    return $this->_cache['properties'][$locale][$key];
  }

  public function getProperties($locale, $inject=true) {
    if ($locale > 0 && $inject === true) {
      $this->_locale = $locale;
    }
    $locale = $this->_locale($locale);
    if ($this->_cache['properties'][$locale] === null) {
      $h = $this->dbo->asHash('SELECT p.name, ip.value'
                                 .' FROM '.Item::TABLE_PROPERTY.' AS ip'
                                 .' JOIN '.T('property').' AS p'
                                 .  ' ON (ip.property_id=p.id)'
                                 .' WHERE ip.model_id='.$this->getModelId()
                                 .  ' AND ip.record_id='.$this->getId()
                                 .  ' AND ip.locale='.intval($locale));
      $this->_cache['properties'][$locale] = $h;
    }
    if ($inject === true) {
      foreach ($this->_cache['properties'][$locale] as $key => $value) {
        $this->_nucleus->{$key} = $value;
      }
    }
    return $this->_cache['properties'][$locale];
  }

  public function getPropertiesById($locale=null) {
    $where = $locale ? ' AND locale='.intval($locale) : '';
    $rs = $this->dbo->query('SELECT *'
                            .' FROM '.Item::TABLE_PROPERTY
                            .' WHERE model_id='.$this->getModelId()
                            .  ' AND record_id='.$this->getId()
                            .$where);
    $properties = array();
    foreach ($rs as $h) {
      $properties[$h['locale']][$h['property_id']] = $h['value'];
    }
    return $properties;
  }

  public function _locale($locale=null) {
    if ($locale >= 1) return $locale;
    if ($this->_locale >= 1) return $this->_locale;
    if ($this->hasAttribute('locale') && $this->locale >= 1) {
      return $this->locale;
    }
    return S('res')->getLocale();
  }

  public function hasAttribute($key, $check_length=false) {
    if (empty($key)) return false;
    $this->checkLoaded(__METHOD__ . '('.$key.')');
    $value = property_exists($this->_nucleus, $key);
    if ($value === false || $check_length === false) {
      return $value;
    }
    return strlen($this->getAttribute($key)) >= 1;
  }

  public function hasAttributes() {
    $attrs = is_array(func_get_arg(0))
           ? func_get_arg(0)
           : func_get_args();
    foreach ($attrs as $attr) {
      if (!$this->hasAttribute($attr)) return false;
    }
    return true;
  }

  public function getAttribute($key) {
    if (!$this->isItem() || empty($key)) return null;
    if (property_exists($this->_nucleus, $key)) {
      return $this->_nucleus->{$key};
    }
    return null;
  }

  public function getAttributes($prefix=null, $trim_flag=false) {
    if (!$this->isItem()) return array();
    if ($prefix === null) {
      return (array) $this->_nucleus;
    }
    $h = array();
    $prefix .= '_';
    $l = strlen($prefix);
    foreach ($this->_nucleus as $key => $value) {
      if (substr($key, 0, $l) != $prefix) continue;
      $key = $trim_flag === true ? substr($key, $l) : $key;
      $h[$key] = $value;
    }
    return $h;
  }

  public function setAttributes(array $row) {
    $this->checkLoaded(__METHOD__);
    $alterations = array();
    foreach ($row as $key => $value) {
      $old = $this->getAttribute($key);
      if (is_array($old) && is_array($value)) {
        $diff = AH::recursiveDiff($old, $value);
        if (empty($diff)) continue ;
      }
      elseif ($value == $old) continue ;
      $alterations[$key] = array($old, $value);
      $this->_nucleus->{$key} = $value;
    }
    $row = $this->castAttributes($row);
    if (empty($alterations)) return $this;
    $this->saveVersion($alterations);
    $opts = array('filters' => array('id' => $this->getId()),
                  'limit' => 1);
    $ret = $this->dbo->update($this->_table, $row, $opts);
    if ($ret === -1) {
      S('log')->error('update failure ['.$this->getModel().']', __METHOD__);
      throw new ActiveRecordException('update failure');
    }
    return $this;
  }

  public function setAttribute($key, $value) {
    $this->checkLoaded(__METHOD__);
    if ($this->getAttribute($key) == $value) {
      return $this;
    }
    $this->setAttributes(array($key => $value));
    return $this;
  }

  public function __get($key) {
    return $this->getAttribute($key);
  }

  public function __set($key, $value) {
//    S('log')->backtrace();
    $val = 'xxx';
    S('log')->error('invalid operation : '.
                    '$'.$this->getModel().' ->'.$key.' = "'.$val.'"; '.
                    'use commit() or setAttribute()',
                    __METHOD__);
  }

  public function __isset($key) { return $this->hasAttribute($key); }

  public function setArguments($args) { $this->_args = $args; }

  protected function getArguments() { return $this->_args; }

  protected function _count($model, $opts=array()) {
    return $this->getItems($model, $opts)->count();
  }

  public function __clone() {
    $this->_nucleus = null;
    $this->_cache = array('objects' => array());
    $this->_is_destroyed = false;
    $this->_is_new = false;
    $this->_opts = array('include' => array());
    $this->_changes = array();
    $this->_args = array('id' => (int) $this->id);
    // _locale
  }

  public function getOne($model, $opts=null) {
    $this->checkLoaded('get - '.$model);
    $opts = (array) $opts;
    if (isset($this->_cache['objects'][$model])) {
      return $this->_cache['objects'][$model];
    }
    if (isset($this->_has_one[$model]) || isset($this->_belongs_to[$model])) {
      if (in_array($model, $this->_opts['include'])) {
        $this->_cache['objects'][$model] = Item::getInstance($model);
        $this->_cache['objects'][$model]->_setNucleus($this->getAttributes($model, true));
        return $this->_cache['objects'][$model];
      }
      $args = isset($this->_has_one[$model])
            ? array($this->_foreign_key => $this->getId())
            : $this->getAttribute($model.'_id');
      $this->_cache['objects'][$model] = Item::getInstance($model, $args, $opts);
      return $this->_cache['objects'][$model];
    }
    $foreign_key = $model.'_id';
    if (Model::hasField($this->getModel(), $foreign_key)) {
      if ($this->getAttribute($foreign_key) < 1) {
        S('log')->error('bad foreign key ('.$foreign_key.') ['.$model.']', __METHOD__);
      }
      $this->_cache['objects'][$model] = Item::getInstance($model,
                                                           $this->getAttribute($foreign_key),
                                                           $opts);
    }
    else {
      $this->_cache['objects'][$model] = Item::getInstance($model,
                                                           array($this->_foreign_key => $this->getId()),
                                                           $opts);
    }
    return $this->_cache['objects'][$model];
  }

  public function getRelationWith($model) {
    $model = $model instanceof ActiveRecord ? $model->getModel() : $model;
    if (isset($this->_has_many[$model])) return $this->_has_many[$model];
    if (isset($this->_belongs_to[$model])) return $this->_belongs_to[$model];
    if (isset($this->_has_one[$model])) return $this->_has_one[$model];
    return null;
  }

  public function getItems($model, $opts=null) {
    $this->checkLoaded(__METHOD__);
    $model = strtolower($model);
    $filters = array();
    $opts['filters'] = (array) $opts['filters'];

    $relation = $this->_has_many[$model];

    $through = $relation['through'];
    if (strlen($through) >= 2) {
      $filter = '';
      if (in_array($this->_foreign_key, $relation['through_cols'])) {
        $filter = $through.'.`'.$this->_foreign_key.'`='.$this->getId();
        // hugues : $facet->getFacets()
        //if (in_array($model.'_id', $relation['through_cols'])) {
        if ($this->_foreign_key != $model.'_id' && in_array($model.'_id', $relation['through_cols'])) {
          $filter .= ' AND '.T($model).'.`id`='.$through.'.`'.$model.'_id`';
        }
        else {
          $filter .= ' AND '.T($model).'.`id`='.$through.'.`record_id`'.
                     ' AND '.$through.'.`model_id`='.Model::id($model);
        }
      }
      else {
        $filter  = $through.'.`record_id`='.$this->getId().' AND '.
	           $through.'.`model_id`='.$this->_model_id;
        $filter .= ' AND '.$through.'.`'.$model.'_id`='.T($model).'.`id`';
      }
      $filters[] = $filter;
      if (is_array($relation['cols'])) {
        $opts['cols'] = $relation['cols'];
      }
      $opts['tables'] = array($through);
    }
    elseif ($relation['foreign_key'] === true) {
      $filters['model_id'] = $this->getModelId();
      $filters['record_id'] = $this->getId();
    }
    else {
      $filters[$this->_foreign_key] = $this->getId();
    }
    if (!isset($opts['order'])) {
      if (is_array($relation['order'])) {
        $opts['order'] = $relation['order'];
      }
      else if (Model::hasField($model, 'position')) {
        $opts['order'] = array('position' => 'ASC');
      }
    }
    foreach (array('metas', 'include') as $key) {
      if (!isset($relation[$key])) continue ;
      $opts[$key] = array_merge((array) $opts[$key], (array) $relation[$key]);
    }
    if (is_array($relation['filters'])) {
      foreach ($relation['filters'] as $key => $value) {
        $filters[$key] = $value;
      }
    }
    $opts['filters'] = empty($opts['filters'])
                     ? $filters
                     : array_merge(array($filters),
                                   array($opts['filters']));
    $collection = Item::collect($model, $opts);
    $collection->setName($this->getName());
    return $collection;
  }

  public function __call($method, $args=array()) {
    $_method = __METHOD__.'::'.$method;
    $method = strtolower($method);
    $snippet = substr($method, 0, 3);
    $is_plural = false;
    switch ($snippet) {
    case 'add':
      $this->checkLoaded($method);
      $model = substr($method, 3);
      if (substr($model, -1) === 's') {
        $model = Model::singularize($model);
        $is_plural = true;
      }
      if ($args[0] instanceof ActiveRecord || $args[0] instanceof Collection) {
        $model = $args[0]->getModel();
        $relation = $this->getRelationWith($model);
        if ($relation === null || $relation['through'] === null) {
          S('log')->error('no relation between '.
                          '['.$this->getModel().'] and '.
                          '['.$model.']', $_method);
          throw new ActiveRecordException('no relation');
        }
        $items = $args[0] instanceof Collection ? $args[0] : array($args[0]);
        $attributes = (array) $args[1];
        foreach ($items as $item) {
          $row  = $attributes;
          $row += array($this->_foreign_key => $this->getId(),
                        $item->_foreign_key => $item->getId());
          $this->dbo->insert($relation['through'], $row,
                             array('modifiers' => 'ignore'));
          $item->onAdd($this);
        }
        return $args[0] instanceof ActiveRecord ? $args[0] : $this;
        break ;
      }
      $args[0][$this->_foreign_key] = $this->getId();
      $class = Item::getInstance($model);
      $class->_cache['objects'][$this->getModel()] = $this;
      $class->commit($args[0]);
      $commit = array();
      if ($this->hasAttribute($model.'_count')) {
        $commit[$model.'_count'] = $this->getAttribute($model.'_count') + 1;
      }
      if (count($commit) >= 1) {
        $this->commit($commit);
      }
      return $class;
      break ;
    case 'rem':
      $this->checkLoaded($method);
      if (!($args[0] instanceof ActiveRecord)) {
        S('log')->error('arg must be an ActiveRecord', $_method);
        return $this;
      }
      $item = $args[0];
      $model = $item->getModel();
      $relation = $this->getRelationWith($model);
      if ($relation === null || $relation['through'] === null) {
        S('log')->error('no relation between '.
                        '['.$this->getModel().'] and '.
                        '['.$item->getModel().']',
			  $_method);
        throw new ActiveRecordException('no relation');
      }
      /*      $row = array($this->_foreign_key => $this->getId(),
       $item->_foreign_key => $item->getId());*/
//      $cols = $relation['through']['through_cols'];
      $where = '';
      if (in_array($this->_foreign_key, $relation['through_cols'])) {
        $where = $this->_foreign_key.'='.$this->getId();
        if (in_array($item->_foreign_key, $relation['through_cols'])) {
          $where .= ' AND '.$item->_foreign_key.'='.$item->getId();
        }
        else {
          $where .= ' AND model_id='.$item->_model_id.
                    ' AND record_id='.$item->getId();
        }
      }
      else {
        $where = 'model_id='.$this->_model_id
           .' AND record_id='.$this->getId()
           .' AND '.$item->_foreign_key.'='.$item->getId();
      }
      $this->dbo->exec('DELETE FROM '.$relation['through']
                       .    ' WHERE '.$where);
      $item->onRemove($this);
      return $this;
      break ;
    case 'has':
      $this->checkLoaded($method);
      if (substr($method, 0, 6) === 'hasany') {
        $model = strtolower(substr($method, 6));
        return call_user_func_array(array($this, '_count'),
                                    array($model, $args[0])) >= 1;
      }
      $model = strtolower(substr($method, 3));
      if ($args[0] instanceof ActiveRecord) {
        $item = $args[0];
        if ($item->getModel() != $model) {
          S('log')->error('invalid item model ['.$item->getModel().']',
                          $_method);
          return $this;
        }
        $model_id = $item->getModelId();
        $record_id = $item->getId();
        //$foreign_key = $this->_foreign_key; modif nico
        $foreign_key = $item->_foreign_key;
      }
      elseif (is_numeric($args[0])) {
        $model_id = Model::id($model);
        $record_id = (int) $args[0];
        $foreign_key = $model.'_id';
      }
      else {
        S('log')->error('arg must be an ActiveRecord or a numeric',
                        $_method);
        return $this;
      }
      $relation = Model::relation($this->getModel(), $model);
      if ($relation === null || $relation['through'] === null) {
        S('log')->error('no relation between '.
                        '['.$this->getModel().'] and '.
                        '['.$model.']',
                        $_method);
        throw new ActiveRecordException('no relation');
      }
      $where = '';
      if (in_array($this->_foreign_key, $relation['through_cols'])) {
        $where = $this->_foreign_key.'='.$this->getId();
        if (in_array($foreign_key, $relation['through_cols'])) {
          $where .= ' AND '.$foreign_key.'='.$record_id;
        }
        else {
          $where .= ' AND model_id='.$model_id.
                    ' AND record_id='.$record_id;
        }
      }
      else {
        $where = 'model_id='.$this->_model_id
           .' AND record_id='.$this->getId()
           .' AND '.$foreign_key.'='.$record_id;
      }
      return $this->dbo->fetchValue('SELECT COUNT(*)'
                                    .' FROM '.$relation['through']
                                    .' WHERE '.$where) >= 1;
      break ;
    case 'cou':
      $this->checkLoaded($method);
      $model = Model::singularize(substr($method, 5));
      return call_user_func_array(array($this, '_count'),
                                  array($model, $args[0]));
      break ;
    case 'get':
      $model = substr($method, 3);
      return substr($model, -1) === 's'
           ? $this->getItems(Model::singularize($model), $args[0])
           : $this->getOne($model, $args[0]);
      break ;
    case 'upd':
      // inherited from sts
      // $a = A::getInstance($a_id);
      // $b = $b_id || B::getInstance($b_id);
      // $arguments = array('quantity' => $quantity);
      // $a->updateB($b, $arguments);
      // maybe require some controls / logs
      $model = strtolower(substr($method, 6));
      list($item, $arguments) = $args;
      if (!$item instanceof $model && is_int($item)) {
        $item = Item::getInstance($model, $item);
      }
      $relation = $this->getRelationWith($item->getModel()) ;
      $filters = array($relation['foreign_key'] => $item->id,
                       strtolower($this->getModel()).'_id' => $this->id);
      return $this->dbo->update($relation['through'],
                                  $arguments,
                                  array('filters' => $filters,
                                        'limit' => 1));

      break ;
    default:
      S('log')->error('unknown method', $_method);
      throw new Exception('method unknown');
      break ;
    }
  }

  /****** STATIC ******/

  public static function collect($opts=null) {
    $model = strtolower(get_called_class());
    return Item::collect($model, $opts);
  }

  public static function id($elt) {
    $model = strtolower(get_called_class());
    return Item::id($model, $elt);
  }

  public static function name($id) {
    if ($id < 1) return '-';
    $model = strtolower(get_called_class());
    return Item::name($model, $id);
  }

  public static function exists($elt) {
    $model = strtolower(get_called_class());
    return Item::exists($model, $elt);
  }

  public static function getInstance($args=true, array $options=array()) {
    $model = strtolower(get_called_class());
    return Item::factory($model, $args, $options);
  }

  public static function getInstanceByKey($key, array $opts=array()) {
    $model = strtolower(get_called_class());
    $item = Item::getInstanceByKey($key, $opts);
    if (!$item->isA($model)) {
      throw new MaliciousException();
    }
    return $item;
  }

}
