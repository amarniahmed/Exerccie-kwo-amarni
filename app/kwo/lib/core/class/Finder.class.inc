<?php

class Finder {

  public  $actions = array();
  public  $branch = null;
  private $collection = null;
  public  $cols = array();
  public  $filters = array();
  public  $extra_filters = array();
  public  $has_checkbox = false;
  public  $has_user = false;
  public  $height = 574;
  public  $include = null;
  public  $limit = null;
  public  $metas = null;
  private $model = null;
  public  $opts = null;
  public  $order = null;
  public  $order_reverse = false;
  public  $orders = null;
  public  $overlay = null;
  private $params = null;
  private $resetted = false;
  private $session_key = null;

  public function __construct($params) {
    $req = S('req');

    $this->model = $params['model'] ?: MODEL;
    $this->branch = (array) $params['branch'];
    unset($this->branch[$this->model.'_id']);

    $this->height -= count($this->branch) * 68;

    $this->resetted = $params['reset'] == 1;
    $this->limit = $params['limit'];

    $session = $req->getSession();
    if ($params['finder']) {
      $this->session_key = $params['finder'];
    }
    else {
      $this->session_key = $this->branch;
      ksort($this->session_key);
      $this->session_key = $this->model.'-finder-'.http_build_query($this->session_key);
      $this->session_key = rtrim($this->session_key, '-');
    }

    $finders = (array) $session->getAttribute('finders');
    $finders[$this->model] = $this->session_key;
    if ($this->isResetted()) {
      $params = array();
      $session->setAttribute($this->session_key, $params);
      $session->setAttribute('finders', $finders);
    }
    elseif ($params['submit'] == 1) {
      $session->setAttribute($this->session_key, $params);
      $session->setAttribute('finders', $finders);
    }
    else {
      $params = $session->getAttribute($this->session_key);
      if (empty($params)) {
        $this->resetted = true;
      }
    }

    $this->params = $params;
    $this->loadXml();
    $this->opts = $this->getCollectionOpts();
    $this->adapt();

    if ($this->limit != -1 && $this->isResetted() && !S('admin')->hasFlag(Admin::FLAG_AUTOLIST)) {
      $this->collection = null;
    }
    else {
      $this->collection = Item::collect($this->model, $this->opts);
      //$this->collection->trace();
    }
    // utile pour adapter les filters de la collection
    $this->onFind();
  }

  public function getModel() {
    return $this->model;
  }

  public function getId() {
    return $this->session_key;
  }

  public function isResetted() {
    return $this->resetted === true;
  }

  public function onFind() {
    $class = Item::getInstance($this->model);
/*    if (method_exists($class, 'onCollect')) {
      $class->onCollect($this);
    } */
    if ($this->collection && method_exists($class, 'onFind')) {
      $class->onFind($this);
    }
    return $this;
  }

  public function getPage() {
    return $this->opts['page'] ?: 1;
  }

  public function getCollection() {
//    if ($this->collection) $this->collection->trace();
    return $this->collection;
  }

  public function getEntireCollection() {
/*    $opts = $this->opts;
    unset($opts['page']);
    unset($opts['limit']);
    $collection = Item::collect($this->model, $opts);*/
//    S('log')->debug('ici');
    $collection = clone $this->collection;
    $collection->unconstrain();
    return $collection;
  }

  private function adapt() {
    $dbo = S('dbo');
    $invalid_keys = array();
    foreach ($this->filters as $key => &$filter) {
      if (isset($this->branch[$key]) && $this->branch[$key] >= 1) {
        $filter['type'] = 'hidden';
        $filter['value'] = $this->branch[$key];
      }
      elseif (in_array($filter['type'], array('checkbox', 'radio', 'select')) &&
              empty($filter['options'])) {
        $invalid_keys[] = $key;
      }
    }
    foreach ($invalid_keys as $key) {
      unset($this->filters[$key]);
    }
    $relations = (array) Model::parameter($this->model, 'belongs_to');
    if (isset($this->opts['filters']['properties'])) {
      $keyword = $this->opts['filters']['properties'];
      unset($this->opts['filters']['properties']);
      $set = $dbo->asSet('SELECT DISTINCT record_id'
                         .' FROM '.Item::TABLE_PROPERTY
                         .' WHERE model_id='.Model::id($this->model)
                         .  ' AND value LIKE "%#keyword#%"',
                         array('#keyword#' => $keyword));
      $has_many = (array) Model::parameter($this->model, 'has_many');
      foreach ($has_many as $name => $relation) {
        if ($relation['through']) continue ;
        if (!Model::hasField($name, $this->model.'_id')) continue ;
        $extra = $dbo->asSet('SELECT DISTINCT m.'.$this->model.'_id'
                             .' FROM '.Item::TABLE_PROPERTY.' AS p,'
                             .     ' '.T($name).' AS m'
                             .' WHERE p.model_id='.Model::id($name)
                             .  ' AND p.value LIKE "%#keyword#%"'
                             .  ' AND p.record_id=m.id',
                             array('#keyword#' => $keyword));
        $set = array_merge($set, $extra);
      }

      if (empty($set)) {
        $set = array(0);
      }
      else {
        $set = array_unique($set);
      }

      $this->opts['filters']['id'] = $set;
    }

    if (count($this->branch) < 1) return ;
    foreach ($relations as $model => $relation) {
      $key = $relation['foreign_key'];
      if (isset($this->branch[$key]) && $this->branch[$key] >= 1) {
        if (empty($this->params['filters'][$key])) {
          $this->opts['filters'][T($this->model).'.'.$key] = $this->branch[$key];
        }
        if (isset($this->cols[$model.'_name'])) {
          unset($this->cols[$model.'_name']);
        }
      }
    }
    if (Model::hasOwner($this->model)) {
      $branch_keys = array();
      foreach ($this->branch as $key => $value) {
        if ($value < 1) continue ;
        $branch_keys[] = $key;
      }
      $model_key = $branch_keys[0];
      list ($model, ) = explode('_', $model_key);
      if ($this->branch[$model_key] >= 1) {
        $this->opts['filters']['model_id'] = Model::id($model);
        $this->opts['filters']['record_id'] = $this->branch[$model_key];
      }
    }
  }

  private function getCollectionOpts() {
    $opts = array();
    $filters = array();

    if ($this->limit != -1) {
      if (strlen($this->params['limit']) >= 1) {
        $opts['limit'] = (int) $this->params['limit'];
      }
      else {
        $opts['limit'] = 50;
      }
      $opts['page'] = (int) ($this->params['page'] ?: 1);
    }
    if ($this->include == 'visit') {
      $opts['tables'] = array(Visit::TABLE => 'v');
      $opts['cols'] = array('v.medium', 'v.source', 'v.term', 'v.campaign');
      $filters[] = 'visit_id=v.id';
    }
    else {
      $opts['include'] = strlen($this->include) >= 1 ? SH::asSet($this->include) : null;
    }
    $opts['metas'] = SH::asSet($this->metas);
    if (isset($this->cols['created_at']) && !Model::hasField($this->model, 'created_at')) {
      $opts['metas'][] = 'created_at';
    }

    $order = strlen($this->params['order']) >= 1 ? $this->params['order'] : $this->order;
    $this->order = $order;
    $this->order_reverse = (bool) $this->params['order_reverse'];
    $opts['order'] = array();
    if (strlen($order) >= 1) {
      $chunks = (array) explode(',', $order);
      foreach ($chunks as $chunk) {
        $chunk = trim($chunk);
        list ($field, $direction) = explode(' ', $chunk);
        $direction = trim(strtoupper($direction));
        $direction = $direction === 'DESC' ? $direction : 'ASC';
        if ($this->order_reverse == true) {
          $direction = $direction == 'ASC' ? 'DESC' : 'ASC';
        }
        $opts['order'][$field] = $direction;
      }
    }

    $metas = array('created_at', 'updated_at');
    foreach ($metas as $meta) {
      if (strpos($this->order, $meta) && !in_array($meta, $opts['metas'])) {
        $opts['metas'][] = $meta;
      }
    }

    $opts['filters'] = $this->getCollectionFilters($filters);
//    S('log')->debug($opts);
    return $opts;
  }

  /*

   Les différents filters de cette classe :

   - $this->params['filters'] : filtres transmis dans la requete
     array('id' => '', 'code' => '', 'name' => '', 'app' => DEVEL)
     $this->params correspond à l'attribut $params reçu par le contructeur

   - $filters : filtres pour la collection (retournés par cette mathode).
     Devient $this->opts['filters'] (opts de la Collection)
     array('app' => DEVEL)

   - $this->filters : filtres pour construire l'UI (filtres XML)
     array(
      'id' => array('name' => 'id', 'type' => 'text', 'label' => 'ID', 'value' => ''),
      'app' => array('name' => 'app',
                     'type' => 'select',
                     'options' => array('x' => 'x', 'y' => 'y'),
                     'label' => 'App',
                     'value' => DEVEL)
    )

   - $this->extra_filters : à définir

  */

  private function getCollectionFilters($filters) {
    $values = array();
    $dbo = S('dbo');
    $this->params['filters'] = (array) $this->params['filters'];

    foreach ($this->params['filters'] as $name => $value) {

      $this->filters[$name]['value'] = $value;

      if (!(is_array($value) || strlen($value) >= 1)) continue ;

      if (substr($name, 0, 4) === '|%%|' && strpos($value, ' ') !== false) {
        $value = explode(' ', $value);
      }

      if ($this->filters[$name]['type'] == 'date' && strlen($value) >= 8) {
        $name = '|><|'.$name;
        if (strpos($value, ' ') === false) {
          $pair = array(DH::toDb($value));
          $pair[1] = $pair[0];
        }
        else {
          $pair = explode(' ', $value);
          $pair[0] = DH::toDb($pair[0]);
          $pair[1] = DH::toDb($pair[1]);
        }
        $value = $pair;
      }

      if (!is_array($value)) {
        if ($value[0] == '%' && $name[0] != '|') {
          $name = '|%%|'.$name;
          $value = trim($value, '%');
        }
        elseif (substr($value, -1) == '%' && $name[0] != '|') {
          $name = '|%|'.$name;
          $value = rtrim($value, '%');
        }
      }

      $casts = (array) Model::parameter($this->model, 'casts');
      if ($casts[$name] === 'bits' && $name[0] != '|') {
        if ($this->filters[$name]['type'] == 'hbits') {
          $name = '|hbits|'.$name;
        }
        else {
          $name = '|&|'.$name;
        }
      }
      $key = $name;
      $filters[$key] = $value;
      $values[$name] = $value;

    }

    $has_many = (array) Model::parameter($this->model, 'has_many');
    foreach ($has_many as $name => $relation) {
      $key = $relation['foreign_key'];
      if (empty($this->params['filters'][$key])) continue ;
      $in = (array) $this->params['filters'][$key];
      if (count($in) < 1) continue ;
      $in[] = 0;
      $through = $relation['through'];
      $col = empty($through) ? 'id' : $this->model.'_id';
      $set = $dbo->asSet('SELECT '.$col
                         .' FROM '.$through
                         .' WHERE '.$key.' IN ('.join(',', $in).')');
      unset($filters[$key]);
      $filters['|=|id'] = $set;
      unset($filters['id']);
    }

    $this->extra_filters = (array) $this->params['extra_filters'];

    if ($this->model == 'conversion') {
      $h = array('source' => Tracker::EXPR_SOURCE,
                 'term' => Tracker::EXPR_TERM,
                 'campaign' => Tracker::EXPR_CAMPAIGN);
      foreach ($filters as $key => $value) {
        if ($h[$key] === null || empty($value)) continue ;
        $value = Tracker::str2id($value, $h[$key], false);
        $filters[$key] = $value < 1 ? -1 : $value;
      }
      if (!empty($filters['ip'])) {
        $filters['ip'] = IH::ip2num($filters['ip']);
      }
    }

    if (!empty($this->extra_filters['user_value'])) {
      $field = $this->extra_filters['user_field'];
      $value = $this->extra_filters['user_value'];
      if ($field == 'id' && strpos($value, ',') >= 1) {
        $value = str_replace(' ', '', $value);
        $value = explode(',', $value);
      }
      elseif ($field == 'email' || $field == 'name') {
        $set = $dbo->asSet('SELECT id'
                           .' FROM '.T('user')
                           .' WHERE '.$field.' LIKE "'.$value.'%"');
        $value = $set;
      }
      elseif ($field == 'last_name') {
        $profile = P('profile.user');
        $set = $dbo->asSet('SELECT user_id'
                           .' FROM '.T($profile)
                           .' WHERE last_name LIKE "'.$value.'%"');
        $value = $set;
      }
      if (is_array($value) && count($value) == 1) $value = $value[0];
      $filters[T($this->model).'.user_id'] = $value;
    }

    if (0) {
      S('log')->debug($filters, '$filters');
      S('log')->debug($this->params, '$this->params');
      S('log')->debug($this->filters, '$this->filters');
    }
    // S('log')->debug($values);

    foreach ($this->filters as $name => &$filter) {
      if (isset($values[$name])) {
        $filter['value'] = $values[$name];
      }
    }
    return $filters;
  }

  public function getItemHash($item) {
    $h = array();
    $cols = $this->cols;
    $h['_color'] = 'white';
    if ($item->hasAttribute('status')) {
      $h['_color'] = 'green';
      if (property_exists($item, 'colors')) {
        $h['_color'] = $item::$colors[$item->status];
      }
      elseif ($item->status < 1) {
        $h['_color'] = 'red';
      }
    }
    foreach ($cols as $col) {
      $key = $col['name'];
      $value = $item->hasAttribute($key) ? $item->getAttribute($key) : ' - ';
      if (isset($col['model'])) {
        if ($value < 1) {
          $value = '-';
        }
        else {
          $value = Item::getInstance($col['model'], $value)->getName(BackManager::getLocale());
          //$value = Item::name($col['model'], $value);
        }
      }
      elseif (substr($key, -3) == '_at') {
        $value = H::datetime($value);
        list ($day, $hour) = explode(' ', $value);
        $value = $day.' <small>'.$hour.'</small>';
      }
      elseif (substr($key, -3) == '_on') {
        $value = H::date($value);
      }
      elseif ($key == 'user') {
        if ($item->user_id >= 1) {
          try {
            $user = User::getInstance($item->user_id, array('ignore' => true));
            $value = '<span title="'.$user->name.' ('.$user->id.')">'.$user->email.'</span>';
          }
          catch (Exception $e) { $value = '** user_id '.$item->user_id.' **'; }
        }
        else {
          $value = '* Anonymous *';
        }
      }
      elseif ($key == 'about') {
        $value = explode("\n", $value);
        $value = (string) ucfirst($value[0]);
      }
      elseif ($key == 'item') {
        try {
          $i = $item->getItem();
          $value = $i->getName(BackManager::getLocale());
        }
        catch (Exception $e) {
          if (Model::exists($item->model_id)) {
            $value = '** '.Model::name($item->model_id).' ('.$item->record_id.') **';
          }
          else {
            $value = '** '.$item->model_id.' ('.$item->record_id.') **';
          }
        }
      }
      elseif ($key == 'status') {
        $value = c(l($item::$statuses[$value]));
      }
      elseif ($key == 'type') {
        $value = c(l($item::$types[$value]));
      }
      if ($key == 'model_id') {
        $value = ucfirst(Model::label($value));
      }
      elseif ($key == 'locale') {
        $value = '<img src="'.PIX_URL.'/lang/'.Locale::toCode($value).'.png" />';
      }
      elseif ($key == 'locales') {
        $str = '';
        foreach (S('app')->getBackLocales() as $locale) {
          if ($value & Locale::toNum($locale)) $str .= ', '.Locale::toCode($locale);
        }
        $value = ltrim($str, ', ');
      }
      $h[$key] = $value;
    }
    try {
      if (method_exists($item, 'asFinderHash')) {
        $h = $item->asFinderHash($h);
      }
      return $h;
    }
    catch (Exception $e) {
      S('log')->error($e.' ('.$item->getId().')', __METHOD__);
      return $h;
    }
  }

  private function loadXml() {
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;

    $extension = Model::parameter($this->model, 'extension');
    $file = LIB_PATH.'/'.$extension.'/xml/'.$this->model.'.finder.xml';
    if (!file_exists($file)) {
      $file = LIB_PATH.'/'.CORE.'/xml/item/finder.xml';
    }

    if ($doc->load($file) === false) {
      throw new Exception();
    }

    if ($doc->documentElement->tagName == 'finder') {
      $finder = $doc->documentElement;
    }
    else {
      $finders = $doc->documentElement->getElementsByTagName('finder');
      $finder = $finders->item(0);
    }

    $attributes = self::node2hash($finder);
    foreach ($attributes as $key => $value) {
      $this->{$key} = $value;
    }

    $date_suffixes = array('_at', '_on', '_to');

    $labels = array('name' => l('nom'),
                    'id' => 'ID',
                    'status' => l('statut'));

    $relation = Model::parameter($this->model, 'belongs_to');

    $foreign_keys = array();
    if (!empty($relation)) {
      foreach ($relation as $mdl => $h) {
        $foreign_keys[$h['foreign_key']] = $mdl;
      }
    }

    foreach ($finder->childNodes as $node) {
      if ($node->nodeName == 'filters') {
        foreach ($node->childNodes as $child) {
          if ($child->hasAttribute('label')) {
            $child->setAttribute('label', l($child->getAttribute('label')));
          }
          $type = $child->getAttribute('type');
          $name = $child->getAttribute('name');
          if (in_array($name, array('code', 'name', 'email')) && $name[0] != '|') {
            if (!$child->hasAttribute('label')) {
              $label = $name;
              if (self::$labels[$name]) {
                $label = l(self::$labels[$name]);
              }
              $child->setAttribute('label', $label);
            }
            $name = '|%%|'.$name;
            $child->setAttribute('name', $name);
          }
          if (empty($type)) {
            if ($name == 'flags') {
              if ($this->model == 'abuse') {
                $child->setAttribute('type', 'hbits');
              }
              else {
                $child->setAttribute('type', 'checkbox');
              }
              $child->setAttribute('name', 'flags');
            }
            elseif ($name == 'status') {
              $child->setAttribute('type', 'checkbox');
            }
            elseif ($name == 'locale') {
              $child->setAttribute('type', 'checkbox');
            }
            elseif ($name == 'type') {
              $child->setAttribute('type', 'select');
            }
            elseif (substr($name, -3) == '_id' && Model::exists(substr($name, 0, -3))) {
              $child->setAttribute('type', 'select');
              $model = substr($name, 0, -3);
              $callback = "Model::asHash('".$model."')";
              if (method_exists(c($model), 'asBackHash')) {
                $callback = c($model).'::asBackHash()';
              }
              $child->setAttribute('options', $callback);
              if (!$child->hasAttribute('label')) {
                $child->setAttribute('label', substr($name, 0, -3));
              }
            }
            elseif ($name == 'app') {
              $type = App::count() <= 1 ? 'void' : 'select';
              $child->setAttribute('type', $type);
              $child->setAttribute('label', 'App');
            }
            elseif ($child->hasAttribute('options')) {
              $child->setAttribute('type', 'select');
            }
            elseif ($child->hasAttribute('rows')) {
              $child->setAttribute('type', 'select');
            }
            else {
              $child->setAttribute('type', 'text');
            }
          }
          $type = $child->getAttribute('type');
          if (in_array($type, array('select', 'radio', 'checkbox')) && !$child->hasAttribute('options')) {
            if ($name == 'status') {
              if (property_exists(c($this->model), 'statuses')) {
                $child->setAttribute('options', c($this->model).'::$statuses');
              }
              else {
                $child->setAttribute('type', 'yesno');
              }
            }
            elseif ($name == 'flags') {
              $child->setAttribute('options', c($this->model).'::$flags');
            }
            elseif ($name == 'locale') {
              $child->setAttribute('options', "I18N::locales()");
            }
            elseif ($name == 'type') {
              $child->setAttribute('options', c($this->model).'::$types');
            }
            elseif ($name == 'app') {
              $child->setAttribute('options', 'App::names(true)');
            }
          }

          // BO Localization
          if ($child->hasAttribute('options')) {
            $options = $child->getAttribute('options');
            if (strpos($options, '::$') !== false) {
              $child->setAttribute('options', 'array_map("l", '.$options.')');
            }
          }

          if ($type === 'user') {
            $this->has_user = true;
            continue;
          }
          elseif ($type === 'tag') {
            $child->setAttribute('type', 'text');
            $child->setAttribute('name', '|TAG|');
            $child->setAttribute('label', l('tag'));
          }
          elseif ($type === 'interest') {
            $child->setAttribute('type', 'select');
            $child->setAttribute('name', '|INTEREST|');
            $child->setAttribute('label', l('thèmes'));
            $child->setAttribute('options', "Model::asHash('interest')");
          }
          elseif (isset($foreign_keys[$name])) {
            $child->setAttribute('type', 'select');
            $model = $foreign_keys[$name];
            $child->setAttribute('options', "Model::asHash('".$model."')");
            if (!$child->hasAttribute('label')) {
              $child->setAttribute('label', $model);
            }
          }
          $name = $child->getAttribute('name');
          if (!$child->hasAttribute('label')) {
            $child->setAttribute('label', $labels[$name] ?: $name);
          }
          if ($child->getAttribute('type') != 'void') {
            $this->filters[$name] = self::node2hash($child);
            if (isset($this->params['filters'])) {
              unset($this->filters[$name]['value']);
            }
          }
        }
      }
      elseif ($node->nodeName == 'cols') {
        foreach ($node->childNodes as $child) {
          $name = $child->getAttribute('name');
          if (in_array(substr($name, -3), $date_suffixes) ||
              substr($name, -5) == '_from') {
            $child->setAttribute('format', 'date');
            if (!$child->hasAttribute('width')) {
              $child->setAttribute('width', '120px');
            }
            if (!$child->hasAttribute('label')) {
              $child->setAttribute('label', 'date');
            }
          }
          elseif (substr($name, -3) == '_id') {
            $model = substr($name, 0, -3);
            if (Model::exists($model)) {
              $child->setAttribute('model', $model);
              if (!$child->hasAttribute('label')) {
                $child->setAttribute('label', Model::name($model));
              }
              if (!$child->hasAttribute('width')) {
                $child->setAttribute('width', '180px');
              }
            }
          }
          if (!$child->hasAttribute('label')) {
            if (isset(self::$labels[$name])) {
              $child->setAttribute('label', l(self::$labels[$name]));
            }
            else {
              $child->setAttribute('label', $name);
            }
          }
          else {
            $child->setAttribute('label', c(l($child->getAttribute('label'))));
          }

          if (!$child->hasAttribute('width') && isset(self::$widths[$name])) {
            $child->setAttribute('width', self::$widths[$name].'px');
          }
          if ($name === 'app' && App::count() <= 1) continue ;
          $this->cols[$name] = self::node2hash($child);
        }
      }
      elseif ($node->nodeName == 'actions') {
        $this->has_checkbox = true;
        foreach ($node->childNodes as $child) {
          $h = self::node2hash($child);
          $h['label'] = c(l($h['label']) ?: $h['name']);
          $h['action'] = $h['action'] ?: $h['name'];
          if (!$h['icon']) {
            if ($h['action'] == 'destroy') $h['icon'] = 'times';
            if ($h['action'] == 'export') $h['icon'] = 'download';
            if ($h['action'] == 'validate') $h['icon'] = 'check';
          }
          $h['datas'] = array();
          foreach (array('manager', 'confirm', 'action') as $k) {
            if (!$h[$k]) continue ;
            $h['datas']['data-'.$k] = $k == 'confirm' ? l($h[$k]) : $h[$k];
          }

          if ($h['action'] == 'destroy') {
            $h['datas']['data-empty'] = c(l('veuillez sélectionner les éléments à supprimer'));
            $h['datas']['data-selection'] = l('élément(s) sélectionné(s)');
          }

          $this->actions[$h['name']] = $h;
        }
      }
      elseif ($node->nodeName == 'orders') {
        foreach ($node->childNodes as $child) {
          $h = self::node2hash($child);
          $this->orders[$h['cols']] = $h;
        }
      }
      elseif ($node->nodeName == 'overlay') {
        $this->overlay = $node->nodeValue;
      }
    }
    //S('log')->debug($this->cols);
    //S('log')->debug($this->filters);
  }

  public static $widths = array('app' => 160,
                                'code' => 180,
                                'country_id' => 50,
                                'locale' => 30,
                                'locales' => 60,
                                'model_id' => 140,
                                'phone' => 100,
                                'position' => 90,
                                'status' => 190,
                                'stock' => 70,
                                'tax' => 70,
                                'type' => 150);

  public static $labels = array('phone' => 'tel',
                                'country_id' => 'pays',
                                'locale' => 'L',
                                'locales' => 'Trad',
                                'about' => 'à propos',
                                'model_id' => 'modèle',
                                'name' => 'nom',
                                'title' => 'titre');

  public static function node2hash($node) {
    $map = $node->attributes;
    $h = array();
    $convert = array('options', 'value');
    foreach ($map as $index => $attrs) {
      $h[$attrs->name] = $attrs->value;
      if (in_array($attrs->name, $convert)) {
        $h[$attrs->name] = self::evalString($h[$attrs->name]);
      }
    }
    return $h;
  }

  public static function evalString($str) {
    eval('$ret = '.$str.';');
    return $ret;
  }

  public function asHtml() {
    try {
      if ($_SERVER['REMOTE_ADDR'] == '192.168.0.102') {
        $template = 'core:finder';
      }
      else {
        $template = 'core:finder';
      }
      $args = array();
      $args['finder'] = $this;
      $args['collection'] = $this->getCollection();
      $args['page_count'] = $args['collection'] ? $args['collection']->getPageCount() : 0;
      $html = Template::render($template, $args);
      return $html;
    }
    catch (Exception $e) {
      S('req')->getSession()->removeAttribute($this->session_key);
      S('log')->error($e, __METHOD__);
    }
    return 'error';
  }

  public function debug() {
    S('log')->debug($this->opts);
  }

  /**
  Le fondement de l'utilisation de getInstanceByModel() est de considérer
  que l'on va accéder à la collection du DERNIER finder instancié (pour le
  modèle spécifié). Dans le cas de sous-finder (ex. Blog > Post) plusieurs
  finders "Post" peuvent résider en effet résider mémoire.
  **/
  public static function getInstanceByModel($model) {
    $session = S('req')->getSession();
    $finders = (array) $session->getAttribute('finders');
    $finder = $finders[$model];
    return self::getInstance($finder);
  }

  public static function getInstance($params=null) {
    if (is_string($params)) {
      $key = $params;
      list ($model, ) = explode('-', $key);
      $params = array('model' => $model,
                      'finder' => $params);
    }
    elseif ($params === null) {
      $params = S('req')->getAttributes();
    }
    return new Finder($params);
  }

}
