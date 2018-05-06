<?php

class Item
{
  const TABLE_DISCOUNT = 'core_item_discount';
  const TABLE_DRAFT = 'core_draft';
  const TABLE_DATA = 'core_item_data';
  const TABLE_LOG = 'core_item_log';
  const TABLE_NODE = 'core_item_node';
  const TABLE_PING = 'core_ping';
  const TABLE_PROPERTY = 'core_item_property';
//  const TABLE_TRANSLATION = 'core_translation';
  const TABLE_REVISION = 'core_item_revision';
  const TABLE_SEO = 'core_item_seo';
  const TABLE_TAG = 'core_item_tag';
  const TABLE_TRACER = 'core_item_tracer';

  const STATUS_OFFLINE = 0;
  const STATUS_ONLINE = 1;

  const LOG_TYPE_IMPRESSION = 1;
  const LOG_TYPE_CLICK = 2;
  const LOG_TYPE_CRAWL = 3;
  const LOG_TYPE_DOWNLOAD = 4;

  const DATA_ATTRIBUTES = 1;
  const DATA_PARAMETERS = 2;
  const DATA_NOTE = 11; // admin
  const DATA_COMMENT = 12; // user
  const DATA_ABOUT = 13; // translator
  const DATA_TWEET_ID = 101;
  const DATA_TUMBLR_ID = 102;
  const DATA_COMMENT_ID = 103;

  public static $datas = array('note' => self::DATA_NOTE,
                               'comment' => self::DATA_COMMENT,
                               'about' => self::DATA_ABOUT);

  const DATA_CUSTOM1 = 201;
  const DATA_CUSTOM2 = 202;
  const DATA_CUSTOM3 = 203;

  public static function log($item=null, $type=null) {
    if (P('app.load') > 0) return ;
    $type = $type > 1 ? $type : self::LOG_TYPE_IMPRESSION;
    if (is_null($item)) {
       $model_id = $record_id = 0;
    }
    else {
      if ($item instanceof ActiveRecord) {
        $model_id = $item->getModelId();
        $record_id = $item->getId();
      }
      elseif (is_array($item)) {
        $model_id = $item['model_id'];
        $record_id = $item['record_id'];
      }
      if ($model_id < 1 || $record_id < 1) {
        S('log')->error('invalid values', __METHOD__);
        return ;
      }
    }
    $type = $type >= 1 ? $type : 1;
    $dbo = S('dbo');
    $ret = $dbo->exec('UPDATE IGNORE '.self::TABLE_LOG
                      .' SET counter=counter+1'
                      .' WHERE model_id='.$model_id
                      .  ' AND record_id='.$record_id
                      .  ' AND type='.$type
                      .  ' AND created_on=CURDATE()');
    if ($ret < 1) {
      $dbo->insert(self::TABLE_LOG,
                   array('model_id' => $model_id,
                         'record_id' => $record_id,
                         'counter' => 1,
                         'type' => $type,
                         'created_on' => DH::now()),
                   array('modifiers' => 'ignore'));
    }

  }

  public static function getPropertiesById($item, $locale=null) {
    $where = $locale ? ' AND locale='.intval($locale) : '';
    $rs = $item->dbo->query('SELECT *'
                            .' FROM '.Item::TABLE_PROPERTY
                            .' WHERE model_id='.$item->getModelId()
                            .  ' AND record_id='.$item->getId()
                            .$where);
    $properties = array();
    foreach ($rs as $hash) {
      $properties[$hash['locale']][$hash['property_id']] = $hash['value'];
    }
    return $properties;
  }

  public static function getInstanceByKey($key, array $opts=array()) {
    if (strpos($key, '-') < 1) {
      S('log')->trace('invalid key ['.$key.']', __METHOD__);
      throw new ClassRecordException('invalid key');
    }
    list ($model_id, $record_id, $hash) = explode('-', $key);
    if ($model_id < 1 || $record_id < 1) {
      S('log')->trace('invalid key ['.$key.']', __METHOD__);
      throw new ClassRecordException('invalid key');
    }
    if (!S('req')->isBack() && $hash != SH::crypt($model_id . $record_id)) {
      S('log')->trace('crypt error, invalid key ['.$key.']', __METHOD__);
      throw new ClassRecordException('invalid key');
    }
    return self::factory(Model::name($model_id), $record_id, $opts);
  }

  public static function getInstance($model, $args=true, array $opts=array()) {
    if (is_numeric($model)) {
      $model = Model::name($model);
      if ($model === null) {
        S('log')->error('invalid model_id ('.$model.') ['.gettype($model).']',
                        __METHOD__);
        throw new ClassRecordException('invalid model');
      }
    }
    else {
      $model = strtolower($model);
    }
    return self::factory($model, $args, $opts);
  }

  public static function collect($model, $opts=null) {
    return Collection::getInstance($model, $opts);
  }

  public static function id($model, $elt) {
    return (int) self::exists($model, $elt);
  }

  /*
sqrt(x * x + y * y)
where x = 69.1 * (lat2 - lat1)
and y = 53.0 * (lon2 - lon1)
*/

  // todo : $opts['autoscale'] true, opts['filters']
  public static function collectByDistance($model, $location, $distance, $opts=null) {
    if ($distance <= 0) return Collection::getEmptyInstance($model);
    $model = Model::name($model);
    $item = $location instanceof ActiveRecord ? $location : null;
    $lat = $location->latitude;
    $lng = $location->longitude;
    $tmp = $distance / abs(cos(deg2rad($lat)) * 69);
    $lng1 = $lng - $tmp;
    $lng2 = $lng + $tmp;
    $tmp = $distance / 69;
    $lat1 = $lat - $tmp;
    $lat2 = $lat + $tmp;
    // d : meter
    $select .= '(7912 * ASIN(SQRT(POWER(SIN(('.$lat.' - t.latitude) * 0.00872664625997), 2) + COS('.$lat.' * 0.0174532925) * cos(t.latitude * 0.0174532925) * POWER(SIN(('.$lng.' - t.longitude) * 0.00872664625997), 2))))/0.00062137 as d';
    $where .= ' AND (t.longitude BETWEEN '.$lng1.' AND '.$lng2.') AND (t.latitude BETWEEN '.$lat1.' AND '.$lat2.')';
    if ($item) {
      $where .= ' AND t.id<>'.$item->id;
    }
    if (Model::hasField($model, 'status')) {
      $where .= ' AND t.status > 0';
    }
    $order = ' ORDER BY d';
    $limit = $limit ?: 10;
    $sql = 'SELECT *, '.$select.' FROM '.T($model).' AS t WHERE 1'.$where.$order.' LIMIT '.$limit;
    $rs = S('dbo')->query($sql);
    if ($rs)
    return $rs->asCollection($model);
  }

  public static $names = array();
  public static function name($model, $id) {
    if ($id < 1) return '-';
    $model_id = Model::id($model);
    $id = (int) $id;
    $key = $model_id.'-'.$id;
    $name = self::$names[$key];
    if ($name) {
      return $name;
    }
    $name = S('dbo')->fetchValue('SELECT name'
                                 .' FROM '.T($model)
                                 .' WHERE id='.$id
                                 .' LIMIT 1');
    if ($name != false && $name != null) {
      self::$names[$key] = $name;
      if (count(self::$names) >= 200) {
        S('log')->warn('item names cache is growing too much ['.$model.']',
                       __METHOD__);
      }
    }
    return $name;
  }

  public static function exists($model, $elt) {
    if ($elt instanceof ActiveRecord) {
      $where = 'model_id='.$elt->getModelId().' AND record_id='.$elt->getId();
    }
    elseif (is_array($elt)) {
      $bindings = array();
      $where = '1';
      foreach ($elt as $key => $value) {
        $where .= ' AND '.$key.'=:'.$key.':';
        $bindings[':'.$key.':'] = $value;
      }
    }
    elseif (preg_match('/^\d*$/', $elt) == 1) {
      $where = 'id='.intval($elt);
      $bindings = null;
    }
    else {
      $where = 'code=:code:';
      $bindings = array(':code:' => $elt);
    }
    $id = S('dbo')->fetchValue('SELECT id'
                               .' FROM '.T($model)
                               .' WHERE '.$where
                               .' LIMIT 1',
                               $bindings);
    return $id > 0 ? intval($id) : false;
  }

  public static function asRelationRows($item, $models, $table) {
    $dbo = S('dbo');
    $models = SH::asSet($models);
    $rels = array();
    $has_status = false;
    $sortable = false;
    $order = '';
    $rel_table = $dbo->getTable($table);
    $rel_fields = $rel_table->getFields();
    $cleanable = !in_array('id', $rel_fields);
    if (in_array('position', $rel_fields)) {
      $sortable = true;
      $order = ' ORDER BY position';
    }
    if (in_array('status', $rel_fields)) {
      $has_status = true;
    }
    $mixed = false;
    $where = $item->getModel().'_id='.$item->getId();
    if (substr($table, -5) != '_item') {
      $model = $models[0];
      if (T($model) == $table) {
        $key = 'id';
      }
      else {
        $key = $model.'_id';
      }
      if (!$dbo->getTable($table)->hasField($item->getModel().'_id')) {
        $where = 'model_id='.$item->getModelId().' AND record_id='.$item->getId();
      }
      $rows = $dbo->query('SELECT *'
			  .' FROM '.$table
			  .' WHERE '.$where
			  .$order);
      foreach ($rows as $row) {
        $rels[] = array('item' => array($model, $row[$key]),
			'row' => $row);
      }
    }
    else {
      $mixed = true;
      $rows = $dbo->query('SELECT *'
			  .' FROM '.$table
			  .' WHERE '.$where
			  .$order);
      foreach ($rows as $row) {
        $rels[] = array('item' => array($row['model_id'],
                                        $row['record_id']),
			'row' => $row);
      }
    }
    foreach ($rels as $index => $rel) {
      try {
	//	S('log')->debug($rel);
	$o = Item::getInstance($rel['item'][0], $rel['item'][1],
			       array('ignore' => true));
	$rels[$index]['item'] = $o;
      }
      catch (Exception $e) {
	S('log')->error('['.$item->getModel().'] '.
			'('.$item->getId().') : '.
			'invalid relation '.
			'['.Model::name($rel['item'][0]).'] '.
			'('.$rel['item'][1].') | '.
			$table,
			__METHOD__);
	if ($cleanable) {
	  if ($mixed) {
	    $conditions = 'model_id='.Model::id($rel['item'][0])
	            .' AND record_id='.$rel['item'][1];
	  }
	  else {
	    $field  = Model::name($rel['item'][0]);
	    $field .= '_id';
	    $conditions = $field.'='.$rel['item'][1];
	  }
	  $dbo->exec('DELETE FROM '.$table
		     .    ' WHERE '.$where
		     .      ' AND '.$conditions
		     .' LIMIT 1');
	  $dbo->debug();
	}
	$rel['item'] = null;
      }
    }
    if (count($rels) < 1) return '';
    if (count($rels) === 1) $sortable = 1;
    $html = '<ul class="'.($sortable?'sortable':'').'" id="att-'.$item->getKey().'-'.microtime(true).'">';
    $attrs = array('id' => '', 'class' => '',
                   'data' => array('model' => '',
                                   'item' => '',
                                   'caller-item' => $item->getKey()));
    foreach ($rels as $rel) {
      if ($rel['item'] === null) continue ;

      $label = $mixed ? ucfirst($rel['item']->getModel()).' : ' : '';

      if (method_exists($item, 'asRelationAttributes')) {
        $ra = $item->asRelationAttributes($rel['item'], $rel['row']);
        $destroyable = isset($ra['destroyable']) ? (bool) $ra['destroyable'] : true;
        $label .= $ra['label'];
      }
      else {
        $destroyable = true;
        $label .= $item->asRelationRow($rel['item'], $rel['row']);
      }

      $attrs['id'] = 'xxx_'.$rel['item']->getKey();
      $attrs['data']['model'] = $rel['item']->getModel();
      $attrs['data']['item'] = $rel['item']->getKey();
      $attrs['class'] = $rel['item']->isOnline() ? array() : array('offline');
      if ($has_status === true &&
          $rel['row']['status'] < 1 &&
          !in_array('offline', $attrs['class'])) {
        $attrs['class'][] = 'offline';
      }
      $html .= H::elt('li', null, $attrs);
      if ($destroyable === true) {
        $html .= '<span class="destroy fa fa-times"></span>';
      }
      if ($sortable === true) {
        $classes = array('handle');
        $html .= '<span class="'.join(' ', $classes).' fa fa-bars" draggable="true"></span> ';
      }
      $title = trim(h(strip_tags($label)));
      $html .= '<span class="content" title="'.$title.'"><span class="overflow">'.$label.'</span></span>';
      $html .= '</li>';
    }
    $html .= '</ul>';
    return $html;
  }

  // http://www.hixie.ch/specs/pingback/pingback
  // http://wordpress.org/development/
  public static function ping($item) {
    $doc = new DOMDocument();
    @$doc->loadHTML($item->content);
    $nodes = $doc->getElementsByTagName('A');
    if ($nodes->length < 1) return ;
    $opts = array('http' => array('method' => 'HEAD'));
    $context = stream_context_create($opts);
    foreach ($nodes as $node) {
      $url = $node->getAttribute('href');
      if (!Valid::isUrl($url)) continue ;
      if (!($handle = @fopen($url, 'r', false, $context))) continue ;
      $metas = stream_get_meta_data($handle);
      $headers = (array) $metas['wrapper_data'];
      $ping_url = null;
      foreach ($headers as $header) {
        list ($name, $value) = explode(': ', $header);
        if ($name == 'X-Pingback') $ping_url = $value;
      }
      if (empty($ping_url)) {
        $html = stream_get_contents($handle);
        $page = new DOMDocument();
        $page->preserveWhiteSpace = false;
        @$page->loadHTML($item->content);
        $links = $page->getElementsByTagName('link');
        if ($nodes->length >= 1) {
          foreach ($nodes as $node) {
            if ($node->hasAttribute('rel') &&
                $node->hasAttribute('href') &&
                $node->getAttribute('href') == 'pingback') {
              $ping_url = $node->getAttribute('href');
            }
          }
        }
      }
      // si vide vérifier dans <HEAD> : <link rel="pingback" href="http://bob.example.net/xmlrpcserver">
      if ($ping_url === null) continue ;
      $client = RpcClient::getInstance($ping_url);
      $client->call('pingback.ping',
                    array('sourceURI' => H::url($item->asUrl()),
                          'targetURI' => $url));
      fclose($handle);
    }
  }

  public static function clean($models=null) {
    if (empty($models)) return ;
    $dbo = S('dbo');
    foreach (P('item_tables') as $table) {
      if (!$dbo->hasTable($table, false)) continue ;
      $column = $table === Meta::TABLE ? Meta::FIELD_MODEL : 'model_id';
      try {
        $dbo->exec('DELETE FROM '.$table
                   .    ' WHERE '.$column.' IN ('.join(',', $models).')');
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
  }

  public static function asSourceName($source) {
    if ($source[0] != '|') return $source;
    list (, $model, $id) = explode('|', $source);
    if (!Model::exists($model)) return $source;
    try {
      $item = Item::getInstance($model, (int) $id, array('ignore' => true));
      if (method_exists($item, 'asSourceName')) {
        $name = $item->asSourcetName();
      }
      else {
        $name = $item->getName().' ('.$id.')';
      }
      return $name;
    }
    catch (Exception $e) {
      return '** '.c($model).' ('.$id.') **';
    }
  }

  public static function asSource($item) {
    $name = '|'.$item->getModel().'|'.$item->getId();
    $id = S('dbo')->fetchValue('SELECT id'
                               .' FROM '.Tracker::TABLE_EXPRESSION
                               .' WHERE name="'.$name.'"'
                               .  ' AND type='.Tracker::EXPR_SOURCE);
    if ($id < 1) {
      $row = array('name' => $name,
                   'type' => Tracker::EXPR_SOURCE);
      $id = S('dbo')->insert(Tracker::TABLE_EXPRESSION, $row);
    }
    return $id;
  }

  public static function asMedium($item) {
    $medium = '';
    switch ($item->getModel()) {
      case 'affiliate':
        $medium = Tracker::MEDIUM_AFFILIATE;
        break;
      case 'campaign':
        $medium = Tracker::MEDIUM_CAMPAIGN;
        break;
      case 'user':
        $medium = Tracker::MEDIUM_USER;
        break;
      case 'letter':
        $medium = Tracker::MEDIUM_LETTER;
        break;
    }
    return $medium;
  }

  public static function edit($item, $opts=null) {
    $opts = (array) $opts;

    $tag = $opts['tag'] ?: 'a';
    $attrs = array('data' => array('item' => $item->getKey()),
                   'on' => array('click' => 'kwo.ux.onItemEdit(this)'));
    if ($opts['callback']) {
      $attrs['data']['callback'] = $opts['callback'];
    }
    else {
      if ($opts['container']) {
        $attrs['data']['container'] = $opts['container'];
      }
      $view = $opts['view'] ?: 'preview';
      $attrs['data']['view'] = $view;
    }
    $html = $item->asView($view);
    if (empty($html)) {
      $attrs['data']['on'] = null;
      $html = '** '.c(Mode::name($item)).' ('.$item->getId().') **';
    }
    return H::elt($tag, $html, $attrs);
  }

  public static function link($item, $label=null) {
    if (is_array($item)) {
      if (isset($item['model_id'])) {
        $model_id = (int) Model::id($item['model_id']);
        $record_id = (int) $item['record_id'];
      }
      else {
        $model_id = (int) Model::id($item[0]);
        $record_id = (int) $item[1];
      }
      $ik = self::asKey($item);
      if ($label === true) {
        try {
          $item = Item::getInstance($model_id, $record_id, array('ignore' => true));
        }
        catch (Exception $e) {
//          S('log')->error($e, __METHOD__);
        }
      }
    }
    else {
      $model_id = $item->getModelId();
      $record_id = $item->getId();
      $ik = $item->getKey();
    }
    if ($label === true) {
      if ($item instanceof ActiveRecord) {
        $label = $item->getName();
      }
      elseif (Model::exists($model_id)) {
        $label = '** '.Model::name($model_id).' **';
      }
    }
    $label = $label ?: c(Model::name($model_id));
    $label .= ' ('.$record_id.')';
    $attrs = array('data' => array('item' => $ik),
                   'on' => array('click' => 'new kwo.dialogs.ItemEditor(this)'));
    return H::elt('a', $label, $attrs);
  }

  public static function asKey($item) {
    if (is_array($item)) {
      $model_id = (int) Model::id($item[0]);
      $record_id = (int) $item[1];
    }
    else {
      $model_id = $item->getModelId();
      $record_id = $item->getId();
    }
    return join('-', array($model_id, $record_id,
                           SH::crypt($model_id . $record_id)));
  }

  public static function factory($model, $args=true, array $options=array()) {
    if (func_num_args() < 1) {
      S('log')->error('arguments required', __METHOD__);
      throw new ClassRecordException('arguments required');
    }

    if (empty($args)) {
      S('log')->error('invalid argument ['.$model.']', __METHOD__);
      S('log')->debug($args, 'argument=');
      // S('log')->backtrace();
      throw new ClassRecordException('invalid argument');
    }

    if (empty($model)) {
//      S('log')->backtrace();
      throw new ClassRecordException('empty class');
    }

    $classname = ucfirst($model);
    $instance = new $classname;
    $instance->_model = $model;
    $params = Model::parameters($model);
    if ($params === null) {
      S('log')->error('model not found ['.$model.']', __METHOD__);
      throw new ClassRecordException('model not found');
    }

    $model_id = $params['id'];
    $instance->_model_id = $model_id;
    $instance->_table = $params['table'];

    foreach (array('belongs_to', 'has_one', 'has_many') as $relation) {
      $instance->{'_'.$relation} = (array) $params[$relation];
    }

    $instance->_foreign_key = isset($params['foreign_key'])
                            ? $params['foreign_key']
                            : $model.'_id';

    $instance->_opts = array('include' => array(),
                             'exclude' => array(),
                             'properties' => array(),
                             'metas' => array());

    if (count($options) >= 1) {
      if (isset($options['ignore'])) {
        $instance->_verbose = ! (bool) $options['ignore'];
      }
      $instance->_opts['include'] = (array) $options['include'];
      if (Model::hasProperties($model) && isset($options['properties'])) {
        $instance->_opts['properties'] = $options['properties'];
      }
      $instance->_opts['metas'] = $options['metas'] === true
                                ? array('*')
                                : (array) $options['metas'];
    }
    if (!is_array($args) && !is_bool($args)) {
      if (is_int($args) || preg_match('/^\d*$/', $args) == 1) {
        $args = (int) $args;
        if ($args < 1) {
          S('log')->error('invalid numeric argument ('.$args.') ['.$model.']',
                          __METHOD__);
          throw new ClassRecordException('invalid numeric argument');
        }
        $args = array('id' => $args);
      }
      elseif (is_string($args) && !empty($args)) {
        $args = array('code' => $args);
      }
    }
    if (is_array($args) && count($args) >= 1) {
      $instance->setArguments($args);
      $instance->instantiate();
    }
    if (is_object($options['nucleus'])) {
      $instance->_setNucleus($options['nucleus']);
    }
    if ($instance->id > 0 && !isset($options['locale']) && Model::isLocalized($model)) {
      $req = S('req');
      if ($req->isFront() || $req->isMiddle() || $req->isAccount()) {
        if (P('app.autoload_properties') === true) {
          $options['locale'] = true;
        }
      }
    }
    if ($instance->id > 0 && !empty($options['locale'])) {
      $options['locale'] = $options['locale'] === true
                         ? S('res')->getLocale()
                         : (int) $options['locale'];
      $instance->_locale = $options['locale'];
      $instance->getProperties((int) $options['locale']);
    }
    return $instance;
  }

}



//insert into Item::TABLE_LOG (model_id, record_id, created_on) values (0,0,'2011-09-16') on duplicate key update counter=counter+1