<?php

class Visit
{
  protected $dbo = null;
  protected $attributes = null;

  const TABLE = 'core_tracker_visit';

  const FLAG_CONVERSION = 1;
  const FLAG_INTERACTION = 2;
  const FLAG_PROXIED = 4;

  public function __construct($id) {
    if ($id < 1) {
      throw new Exception('invalid visit id');
    }
    $this->dbo = S('dbo');
    $this->attributes = $this->dbo->fetchHash('SELECT *'
                                              .' FROM '.Visit::TABLE
                                              .' WHERE id='.intval($id));
  }

  public function getSummary() {
    $strings = $this->getStrings();
    $h = $this->attributes; 
    $h['country_code'] = Country::code($this->country);
    $h['country'] = ucfirst(Country::name($this->country));
    $h['app'] = App::name($this->app_id);
    $h['url'] = '-';
    if ($this->url_id > 0) {
      $url = Url::getInstance($this->url_id);
      $h['url'] = $url->name;
    }
    $h['ip'] = IH::num2ip($this->ip); 
    $h['host'] = gethostbyaddr($h['ip']);
    $h['path'] = $this->getPath();
    $h['system'] = Report::$browsers[$this->browser].' / '.Report::$systems[$this->system];
    $h['referer'] = $this->getOrigin(true);
    if ($this->layout > 0) {
      $h['layout'] = Layout::name($this->layout);
    }
    if ($this->user_id >= 1) {
      try {
        $user = $this->getUser();
        $h['user'] .= Item::link($user, $user->name).'<br/>';
        $h['user'] .= '<a href="javascript:void(0)" class="button-siblings" data-type="user">'.$user->email.'</a><br/>';
        $h['user'] .= $user->getMeta(Meta::FIELD_CREATED_AT);
        try {
          $h['user'] .= '<br/>'.$user->getVisit()->getOrigin(true);
        }
        catch (Exception $e) {}
        if ($user->hasFlag(User::FLAG_IMAGE)) {
          $h['user'] .= '<div style="margin-top:2px;">'.$user->avatar().'</div>';
        }
      }
      catch (Exception $e) { $h['user'] = 'user unknown ('.$this->user_id.')'; }
    }
    return $h;
  }

  private function getOrigin($as_html=false) {
    $strings = $this->dbo->asHash('SELECT id, name'
                                  .' FROM '.Tracker::TABLE_EXPRESSION
                                  .' WHERE id IN (:campaign:,:source:,:term:)',
                                  array(':campaign:' => $this->campaign, 
                                        ':source:' => $this->source, 
                                        ':term:' => $this->term));
    if (Tracker::MEDIUM_DIRECT == $this->medium) {
      $referer = Tracker::$mediums[$this->medium];
    }
    else {
      $referer = 'medium : '.Tracker::$mediums[$this->medium].'<br/>'; 
      if (!empty($strings[$this->source])) {
        $referer .= 'source : '.Tracker::asSourceLink($strings[$this->source]).'<br/>'; 
      }
      if (!empty($strings[$this->term])) {
        $referer .= 'terme : '.$strings[$this->term].'<br/>';
      }
      if (!empty($strings[$this->campaign])) {
        $referer .= 'campagne : '.$strings[$this->campaign];
      }
    }
    return $referer;
  }

  public function getUser() {
    return User::getInstance($this->user_id);
  }

  public function getId() {
    return $this->id;
  }

  public function getPath() {
    $rs = $this->dbo->query('SELECT *'
                            .' FROM '.Tracker::TABLE_HIT
                            .' WHERE visit_id='.$this->id
                            .' ORDER BY created_at'); 
    $events = $rs->asArray(); 
    if (count($events) < 1) return array();
    $ids = array();
    foreach ($events as $value) {
      $ids[$value['category_id']]++;
      $ids[$value['action_id']]++;
    }
    $names = $this->dbo->asHash('SELECT id, name'
                                .' FROM '.Tracker::TABLE_EXPRESSION
                                .' WHERE id IN ([ids])',
                                array('[ids]' => array_keys($ids)));
    reset($events);
    $h = array();
    foreach ($events as $value) {
      list($date, $time) = explode(' ', $value['created_at']);
      $h[$time] = $names[$value['action_id']].' / '.$names[$value['category_id']];
    }
    return $h;
  }

  public function getAttribute($key) { 
    return $this->attributes[$key];
  }

  public function getAttributes() { 
    return $this->attributes;
  }

  public function setAttribute($key, $value) {
    $opts = array('filters' => array('id' => $this->getId()),
                  'limit' => 1);
    S('dbo')->update(self::TABLE,
                     array($key => $value), 
                     $opts);
    return $this;
  }

  public function increment($key) {
    $value = $this->getAttribute($key) + 1;
    $this->setAttribute($key, $value);
  }

  public function decrement($key) {
    $value = $this->getAttribute($key) - 1;
    $this->setAttribute($key, ($value >= 0 ? $value : 0));
  }

  public function getSiblings($opts=null) {
    $opts = (array) $opts;
    $where = 'visitor_id='.$this->visitor_id;
    if ($opts['filter'] == 'ip') {
      $where = 'ip='.$this->ip;
    }
    elseif ($opts['filter'] == 'user') {
      $where = 'user_id='.$this->user_id;
    }
    elseif ($opts['filter'] == 'source') {
      $where = 'medium='.$this->medium.' AND '.
               'source='.$this->source.' AND '.
               'term='.$this->term.' AND '.
               'campaign='.$this->campaign;
    }
    if ($opts['days'] > 0) {
      $where .= ' AND id<='.$this->id
               .' AND TIMESTAMPDIFF(DAY, created_on, "'.$this->created_on.'") <= '.$opts['days'];
    }
    $columns = join(',', array('id', 'country', 'ip', 'user_id', 'visitor_id', 'duration',
                               'conversions', 'medium', 'source', 'term', 'campaign',
                               'created_on', 'created_at'));
    $rs = $this->dbo->query('SELECT '.$columns
                            .' FROM '.self::TABLE
                            .' WHERE '.$where
                            .' ORDER BY id DESC'
                            .' LIMIT 300'); 
    $set = $countries = array();
    foreach ($rs as $row) {
      $set[$row['source']]++;
      $set[$row['term']]++;
      $set[$row['campaign']]++;
      $countries[$row['country']]++;
    }
    $names = $this->dbo->asHash('SELECT id, name'
                                .' FROM '.Tracker::TABLE_EXPRESSION
                                .' WHERE id IN ([set])',
                                array('[set]' => array_keys($set)));
    $countries = $this->dbo->asHash('SELECT id, code'
                                    .' FROM '.T('country')
                                    .' WHERE id IN ([set])',
                                    array('[set]' => array_keys($countries)));
    $out = array();
    foreach ($rs as $row) {
      $tmp = array();
      $tmp = $row;
      $source = $names[$row['source']]; 
      $tmp['source'] = Tracker::asSourceLink($names[$row['source']]);
      $tmp['term'] = $names[$row['term']];
      $tmp['campaign'] = $names[$row['campaign']];
      $tmp['country'] = $countries[$row['country']];
      $out[] = $tmp;
    }
    return $out;
  }

  private function getStrings($set=null) {
    if ($set === null) {
      $set = $this;
    }
    if ($set instanceof Visit) {
      $visit = $set;
      $set = array($visit->campaign, $visit->source, 
                   $visit->term, $visit->host);
    }
    return S('dbo')->asHash('SELECT id, name'
                            .' FROM '.Tracker::TABLE_EXPRESSION
                            .' WHERE id IN ([set])',
                            array('[set]' => $set));
  }

  public function hasNext($check_association=true) {
    $t = time();
    $session = S('req')->getSession();
    if ($session->getAttribute('visit_max') < 1 || 
        ($t - $session->getAttribute('last_check')) > 600) {
      $n = S('dbo')->fetchValue('SELECT MAX(id) FROM '.self::TABLE);
      $session->setAttribute('visit_max', $n);
      $session->setAttribute('last_check', $t);
    }
    return $this->id < $session->getAttribute('visit_max');
  }

  public function __get($key) { return $this->attributes[$key]; }

  public static function getInstance($id) {
    return new Visit($id);
  }

  public static function link($visit, $label=null) {
    if ($visit instanceof Visit) {
      $visit = $visit->id;
    }
    $label = $label === null ? $visit : (string) $label;
    $attrs = array('on' => array('click' => 'new kwo.dialogs.Visit(this)'),
                   'data' => array('visit-id' => $visit));
    return H::elt('a', $label, $attrs);
  } 

}