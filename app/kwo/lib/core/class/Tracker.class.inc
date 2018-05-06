<?php

/***
https://developers.google.com/analytics/devguides/collection/gajs/asyncMigrationExamples
Adwords > MyAccount > Tracking > Autotagging
***/

class Tracker {

  const COOKIE_NAME = 'tracker';

  private $cookie = null;
  private $crc = null;
  private $dbo = null;
  private $location = array();
  private $renew = false;
  private $req = null;
  private $ts = null;
  private $ttl = 1800;

  const CATEGORY_ERROR  = 'errors';
  const CATEGORY_PAGE   = 'displays';
  const CATEGORY_EVENT  = 'interactions';

  const TABLE_HIT        = 'core_tracker_hit';
  const TABLE_TMP        = 'core_tracker_hit_tmp';
  const TABLE_ROBOT      = 'core_tracker_robot';
  const TABLE_SEARCH     = 'core_tracker_search';
  const TABLE_EXPRESSION = 'core_tracker_expression';
  const TABLE_VISITOR    = 'core_tracker_visitor';

  private static $tables = array(self::TABLE_HIT, self::TABLE_TMP, 
                                 self::TABLE_ROBOT, self::TABLE_SEARCH, 
                                 self::TABLE_EXPRESSION, self::TABLE_VISITOR,
                                 Visit::TABLE, Report::TABLE);

  const MEDIUM_DIRECT    = '1';
  const MEDIUM_CPM       = '2';
  const MEDIUM_SEARCH    = '3';
  const MEDIUM_LINK      = '4';
  const MEDIUM_CPC       = '5';
  const MEDIUM_CAMPAIGN  = '6';
  const MEDIUM_RSS       = '7';
  const MEDIUM_USER      = '8';
  const MEDIUM_AFFILIATE = '9';
  const MEDIUM_LETTER    = '10';
  const MEDIUM_SOCIAL    = '11';
  const MEDIUM_SHOPPING  = '12';
  const MEDIUM_PARTNER   = '13';
  const MEDIUM_MAIL      = '14';

  public static $mediums = array(self::MEDIUM_CPC       => 'CPC',
                                 self::MEDIUM_CPM       => 'CPM',
                                 self::MEDIUM_DIRECT    => 'accès direct',
                                 self::MEDIUM_AFFILIATE => 'affiliation',
                                 self::MEDIUM_RSS       => 'flux (RSS)',
                                 self::MEDIUM_LETTER    => 'mail transactionnel',
                                 self::MEDIUM_MAIL      => 'mail',
                                 self::MEDIUM_CAMPAIGN  => 'mailing',
                                 self::MEDIUM_SEARCH    => 'moteur de recherche',
                                 self::MEDIUM_USER      => 'parrainage',
                                 self::MEDIUM_SOCIAL    => 'réseau social',
                                 self::MEDIUM_LINK      => 'site affluent',
                                 self::MEDIUM_SHOPPING  => 'shopping',
                                 self::MEDIUM_PARTNER   => 'partenaire');
  const EXPR_CATEGORY = 1;
  const EXPR_ACTION   = 2;
  const EXPR_QUERY    = 15; // Searcher
  const EXPR_SOURCE   = 20; // knxs
  const EXPR_TERM     = 21; // knxt
  const EXPR_CAMPAIGN = 22; // knxc
  const EXPR_ORIGIN   = 30; // Interaction

  public function __construct($renew) {

    $this->dbo = S('dbo');
    $this->req = $req = S('req'); 
    $this->cookie = $cookie = $req->getCookie(self::COOKIE_NAME); 
    $this->ts = time();
    $this->renew = $renew;

    $referer = $req->getClient('referer');
    $this->location = parse_url($referer);
    $url = preg_replace('/knx[acds]id=[[:alnum:]-]+/', '', $referer);
    $url = trim($url, '&?/');
    $this->location['url'] = $url;
    $this->location['params'] = array();
    if (!empty($this->location['query'])) {
      $params = array();
      parse_str($this->location['query'], $params);
      $this->location['params'] = (array) $params;
    }

    if ($cookie->getAttribute('visit_id') > 0) {  
      if ($this->hasExpired() || $this->isNewSource()) { 
        $this->renew();
      }
      elseif (($this->ts - $cookie->getAttribute('ts')) > 5) { 
        $attrs = array('ts' => $this->ts,
                       'd' => date('d'));
        $this->setCookieAttributes($attrs);
      }
    }
    else { 
      $this->init();
    }

    $h = array('visit_id' => (int) $cookie->getAttribute('visit_id'),
               'visit_number' => (int) $cookie->getAttribute('number'),
               'visitor_id' => (int) $cookie->getAttribute('visitor_id'),
               'layout' => (int) $cookie->getAttribute('layout'),
               'knxv' => (int) $cookie->getAttribute('knxv'));
    
    $req->setParameters($h);

  }

  public static $marketing_params = array('utm_source', 'utm_medium', 'utm_term',
                                          'utm_campaign', 'utm_content', 'utm_id',
                                          'knxs', 'knxm', 'knxt', 'knxc', 'knxv',
                                          'knxsid', 'knxcid', 'knxaid', 'knxdid',
                                          'gclid', 'ovchn');

  private function isNewSource() {
    if ($this->renew === false) return false;
    $params = $this->location['params'];
    if (empty($params)) return false;
    $source = '';
    $attrs = array();
    foreach (self::$marketing_params as $key) {
      if (empty($params[$key])) continue ;
      $source .= $key.'='.$params[$key].';';
    }
    if (empty($source)) return false;
    $crc = SH::crc32($source);
    if ($this->cookie->getAttribute('source') == $crc) {
      return false;
    }
    S('log')->trace($source, __METHOD__);
    $attrs['source'] = $crc;
    if ($params['knxsid'] > 0) {
      $attrs['sponsorship_id'] = (int) $params['knxsid'];
    }
//    if ($params['knxv'] > 0) {
    $attrs['knxv'] = (int) $params['knxv'];
    if ($attrs['knxv'] > 0) {
//      S('log')->trace('knxv='.$attrs['knxv'], __METHOD__);
    }
//    } 
    $this->setCookieAttributes($attrs);
    return true;
  }

  private function hasExpired() {  
    if ($this->renew === false) return false;
    $ts = $this->cookie->getAttribute('ts');
    $diff = $this->ts - $ts; 
    $bool = ($diff > $this->ttl || $this->cookie->getAttribute('d') != date('d')); 
    $this->trace('hasExpired ['.date('Y-m-d', $ts).']');
    if ($bool === true) {
      $this->isNewSource();
    }
    return $bool;
  }

  private function init() {
    if ($this->renew === false) return false;
    if ($this->isBot()) return false;
    $row = $this->getDb();
    if ($row['visit_id'] > 0) return ;
    if ($row['visitor_id'] < 1) {
      $this->req->setParameter('is_new_visitor', true);
    }
    $this->isNewSource();
    $this->trackVisit($row['visitor_id']);
  }

  private function renew() { 
    if ($this->renew === false) return false;
    if ($this->isBot()) return false;
    $this->trace('renew');
    $visitor_id = $this->cookie->getAttribute('visitor_id');
    $this->trackVisit($visitor_id);
  }

  private function trackVisit($visitor_id=0) {
    $req = $this->req;
    $cookie = $this->cookie;
    $visitor_id = (int) $visitor_id;

    $referer = self::getReferer($req->getHeader('x-kwo-referer'), 
                                $this->location['params'],
                                $this->location['url']);

    $agent = Report::ua2hash($req->getClient('agent'));

    $number = $cookie->getAttribute('number') + 1;
    
    $layout = $this->getLayout();

    $row = array('locale' => S('res')->getLocale(),
                 'app_id' => $this->req->getAppId(),
                 'layout' => $layout,
                 'number' => $number,
                 'medium' => $referer['medium'],
                 'source' => $referer['source'],
                 'term' => $referer['term'],
                 'campaign' => $referer['campaign'],
                 'ip' => $req->getClient('ip'),
                 'browser' => $agent['browser'],
                 'system' => $agent['system'],
                 'country' => $req->getClient('country'),
                 'created_on' => DH::today(),
                 'created_at' => DH::now(),
                 'url_id' => Url::asId($this->location['url']),
                 'visitor_id' => $visitor_id);

    if (S('ctx')->_user['id'] > 0) {
      $row['user_id'] = (int) S('ctx')->_user['id'];
    }

    if ($cookie->getAttribute('actual') > 0) {
      $row['span'] = ceil(($this->ts - $cookie->getAttribute('actual')) / (24 * 3600));
    }

    $visit_id = $this->dbo->insert(Visit::TABLE, $row, 
                                   array('modifiers' => 'HIGH_PRIORITY'));
    $this->trace();

    if ($visit_id < 1) {
      S('log')->error('invalid visit_id', __METHOD__);
      throw new Exception();
    }

    if ($visitor_id > 0) {
      $ret = $this->dbo->exec('UPDATE '.self::TABLE_VISITOR
                              . ' SET ip='.$req->getClient('ip').','
                              .     ' crc='.$this->getCrc().','
                              .     ' visit_id='.$visit_id.','
                              .     ' created_at=NOW(),'
                              .     ' counter=counter+1'
                              .' WHERE visitor_id='.$visitor_id
                              .' LIMIT 1');
      $this->trace();
      $this->trace($ret, 'MATCHED ('.$this->dbo->matchedRows().')');
      if ($ret < 1) {
        $this->trackVisitor(array('visitor_id' => $visitor_id,
                                  'visit_id' => $visit_id));
      }
    }
    else {
      $visitor_id = $this->trackVisitor(array('visit_id' => $visit_id));
      if ($visitor_id < 1) {
        S('log')->error('invalid visitor_id', __METHOD__);
        throw new Exception();
      }
      $this->dbo->exec('UPDATE '.Visit::TABLE
                       . ' SET visitor_id='.$visitor_id
                       .' WHERE id='.$visit_id
                       .' LIMIT 1');
      $this->trace();
    }

    $this->trace($req->getClient(), __METHOD__);
    $this->trace($cookie->getAttributes(), __METHOD__);

    $this->req->setParameter('is_new_visit', true);

    $attrs = array();
    $attrs['number'] = $number;
    $attrs['previous'] = $cookie->getAttribute('actual') ?: $this->ts;
    $attrs['actual'] = $this->ts;
    $attrs['visit_id'] = $visit_id;
    $attrs['visitor_id'] = $visitor_id;
    $attrs['d'] = date('d');
    $attrs['ts'] = $this->ts;
    $attrs['layout'] = $layout;
    
    $this->setCookieAttributes($attrs);

//    S('log')->trace('new visit ('.$visit_id.')');
  }

  private function getLayout() {
    return 0;
  }

  private function trackVisitor($fields) {
    $fields += array('crc' => $this->getCrc(),
                     'ip' => $this->req->getClient('ip'));
    $visitor_id = $this->dbo->insert(self::TABLE_VISITOR, $fields,
                                     array('modifiers' => 'HIGH_PRIORITY'));
    $this->trace();
    return $fields['visitor_id'] ?: $visitor_id;
  }

  private function getDb($create=false) {
    $h = $this->dbo->fetchHash('SELECT visitor_id, visit_id,'
                               .     ' UNIX_TIMESTAMP(created_at) as ts'
                               .' FROM '.self::TABLE_VISITOR
                               .' WHERE ip='.$this->req->getClient('ip')
                               .  ' AND crc='.$this->getCrc()
                               .' ORDER BY created_at DESC'
                               .' LIMIT 1');
    if (empty($h)) return array();
    $h['visit_id'] = (int) $h['visit_id'];
    $h['visitor_id'] = (int) $h['visitor_id'];
    $h['ts'] = (int) $h['ts'];
    if (($this->ts - $h['ts']) > $this->ttl) {
      $h['visit_id'] = 0;
    }
    if ($h['visit_id'] > 0) {
      $this->dbo->exec('UPDATE '.self::TABLE_VISITOR
                       . ' SET created_at=NOW()'
                       .' WHERE visitor_id='.$h['visitor_id']
                       .' LIMIT 1');
      $this->setCookieAttributes(array('visit_id' => $h['visit_id'],
                                       'visitor_id' => $h['visitor_id']));
    }
    return $h;
  }

  private function setCookieAttributes($attrs) {
    if ($this->renew === false) return false;
    $this->cookie->setAttributes($attrs);
  }

  private function isBot() {

    $req = $this->req;
    $addr = $req->getClient('address');

    $is_bot = false;

    if ($req->getParameter('client_is_proxied') == true) {
      // on detecte le pays .. si pas fr, be ... on refuse ET on logge
    }

    $agent = $req->getClient('agent');
    $host = $req->getClient('host');
    list ($chunk, ) = explode('.', $host);
    $chunk = $agent . $chunk;

    if (strlen($agent) < 32 ||
        strpos($agent, 'http://') !== false ||
        strpos($agent, '@') !== false ||
        strpos($chunk, 'google') !== false ||
        strpos($chunk, 'bot') !== false ||
        strpos($chunk, 'crawler') !== false ||
        strpos($chunk, 'spider') !== false ||
        strpos($host, 'amazonaws.com') !== false ||
        !(strpos($agent, 'mozilla/') === 0 || strpos($agent, 'opera/') === 0)) {
      $this->dbo->insert(self::TABLE_ROBOT,
                         array('uri' => $req->getUri(),
                               'address' => $addr,
                               'robot' => $agent));
      $is_bot = true;
    }
    else {
      $ranges = array('150.70.', '65.52.', '216.104.15.', '199.19.249.');
      foreach ($ranges as $range) {
        if (strpos($addr, $range) === 0) {
          S('log')->trace('maybe a bot');
          $is_bot = true;
          break ;
        }
      }
    }

    $req->setParameter('is_bot', $is_bot);
    if ($is_bot === true) {
      S('log')->disable();
    }
    if ($is_bot === true && $this->renew === true) {
      S('log')->trace('bot [a='.$agent.'] [h='.$host.']',
                      __METHOD__);
    }
    return $is_bot;

  }

  private function getCrc() {
    if ($this->crc !== null) return $this->crc;
    $crc = $this->req->getClient('address');
    $keys = array('HTTP_USER_AGENT', 'HTTP_ACCEPT', 'HTTP_ACCEPT_LANGUAGE');
    foreach ($keys as $key) {
      $crc .= $_SERVER[$key];
    }
    $this->crc = SH::crc32($crc);
    return $this->crc;
  }

  public function track($action, $category, $item=null) {
    $visit_id = (int) $this->req->getVisitId();
    if ($visit_id < 1) return false; 
    $from = array('<', '>', '&', '"');
    $to = array(' ', ' ', ' ', "'");
    $action = str_replace($from, $to, $action);
    $action = strtolower($action);
    $action = trim($action);
    $category = str_replace($from, $to, $category);
    $category = strtolower($category);
    $category = trim($category);
    $user_id = (int) S('ctx')->_user['id'];
    $model_id = Model::id('goal');
    foreach (Goal::asHash() as $id => $regexp) {
      if (preg_match($regexp, $action.'.'.$category) > 0) {
        Conversion::add(array('model_id' => $model_id,
                              'record_id' => $id),
                        array('visit_id' => $visit_id,
                              'user_id' => $user_id,
                              'unique' => true));
      }
    }
    $ret = $this->dbo->insert(self::TABLE_TMP,
                              array('visit_id' => $visit_id,
                                    'action' => $action,
                                    'category' => $category));
    if ($ret === false) {
      S('log')->error($action, __METHOD__);
    }
  }

  private function trace($input=null, $prefix=null) {
    return ;
//    if ($_SERVER['REMOTE_ADDR'] != '192.168.0.46') return ;
    if ($input === null) {
      $this->dbo->debug();
      return ;
    }
    S('log')->debug($input, $prefix);
  }

  public static function getReferer($referer, $params, $url) {
    $engines = array('google' => 'google|aol', 'bing' => 'bing|msn|live', 'yahoo!' => 'yahoo',
                     'orange' => 'voila|lemoteur', 'sfr' => 'sfr', 
                     'lo.st' => 'lo\.st', 'ask' => 'ask', 'yandex' => 'yandex',
                     'hotbot' => 'hotbot', 'lycos' => 'lycos', 'about' => 'about',
                     'baidu' => 'baidu', 'alltheweb' => 'alltheweb', 'bluewin' => 'bluewin',
                     'toile' => 'toile',  'alexa' => 'alexa', 'exalead' => 'exalead',
                     'cherchons' => 'cherchons',
                     'ixquick' => 'ixquick', 'search' => 'search|cherche');

    $tags = array('q', 'query', 'rdata', 'x_query', 'qs', 'encquery', 'qt',
                  'kw', 'text', 'terms', 'wd', 'as_q', 'as_epq', 'searchfor');

    $mail_regexp = 'mail|laposte|courrier|courriel|messagerie|pop|smtp|imp|horde|groups|zimbra';
    $cpm_regexp = 'openad';
    $social_regexp = 'facebook|twitter|scoop.it|linkedin|viadeo|myspace|pinterest|delicious|reddit|quora|forum|netvibes|stackoverflow|disqus|getpocket.com';
    // blogger|blogspot|tumblr|wordpress|
    $fb_params = array('fb_action_types' => 1, 'fb_action_types' => 1, 'fb_ref' => 1);
    
    $referer_parts = parse_url($referer);
    $referer_parts['params'] = array();
    if (strlen($referer_parts['query']) >= 3) {
      parse_str($referer_parts['query'], $referer_parts['params']);
      $referer_parts['params'] = (array) $referer_parts['params'];
      $referer_parts['params'] = array_change_key_case($referer_parts['params'], CASE_LOWER);
    }

    if ($params['knxv'] > 0) {
      try {
        $tv = Trackvar::getInstance((int) $params['knxv'], array('ignore' => true));
        $params['knxm'] = $tv->medium;
        if (empty($params['knxs'])) $params['knxs'] = $tv->source;
        if (empty($params['knxc'])) $params['knxc'] = $tv->campaign;
        if (empty($params['knxt'])) $params['knxt'] = $tv->term;
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
      }
    }
    else {
      $params['knxm'] = strtolower($params['knxm']);
    }

    $medium = self::MEDIUM_DIRECT;
    $source = trim($params['knxs'], ' |');
    $term = $params['knxt'];
    $campaign = $params['knxc'];

    if ($params['knxsid'] >= 1) {
      $medium = self::MEDIUM_USER;
      $id = (int) $params['knxsid'];
      if (Sponsorship::exists($id)) {
        $sponsorship = Sponsorship::getInstance($id);
        $source = '|user|'.$sponsorship->getUser()->getId();
        $term = $url;
      }
    }
    elseif ($params['knxaid'] >= 1) {
      $medium = self::MEDIUM_AFFILIATE;
      $source = '|affiliate|'.intval($params['knxaid']);
      //      error_log($source);
    }
    elseif ($params['knxdid'] >= 1) {
      $medium = self::MEDIUM_LETTER;
      $id = (int) $params['knxdid'];
      if (Delivery::exists($id)) {
        $delivery = Delivery::getInstance($id);
        $source = '|letter|'.$delivery->getLetter()->getId();
        $term = $url;
        Delivery::markAsClicked($id, $url);
      }
    }
    elseif (($params['knxcid'] >= 1 && $params['eid'] >= 1) ||
            preg_match('/('.$mail_regexp.')/', $referer_parts['host']) ||
	    $params['knxm'] == self::MEDIUM_CAMPAIGN ||
            $params['knxm'] == self::MEDIUM_MAIL ||
            $params['knxm'] == 'mail') {
      if ($params['cid'] >= 1 || $params['knxm'] == self::MEDIUM_CAMPAIGN) {
        $medium = self::MEDIUM_CAMPAIGN;
        $source = '|campaign|'.intval($params['knxcid']);
      }
      else {
        $medium = self::MEDIUM_MAIL;
      }
    }
    elseif ($params['knxm'] == self::MEDIUM_CPC ||
            $params['knxm'] == 'cpc' ||
            isset($params['gclid']) || isset($params['ovchn'])) {
      $medium = self::MEDIUM_CPC;
      if (!empty($params['knxs'])) $source = $params['knxs'];
      elseif (array_key_exists('gclid', $params)) $source = 'google adwords';
      elseif (array_key_exists('ovchn', $params)) $source = 'yahoo sponsored search';
      if (isset($referer_parts['params']['q'])) {
        $term = $referer_parts['params']['q'];
      }
    }
    elseif ($params['knxm'] == self::MEDIUM_CPM ||
            $params['knxm'] == 'cpm' ||
            preg_match('/('.$cpm_regexp.')/', $referer_parts['host'])) {
      $medium = self::MEDIUM_CPM;
      $source = $referer_parts['host'];
    }
    elseif ($params['knxm'] == self::MEDIUM_RSS ||
            $params['knxm'] == 'rss' ||
            strpos($referer_parts['path'], 'reader/view') > 0) {
      $medium = self::MEDIUM_SOCIAL;
      $source = $referer_parts['host'];
      if (empty($term) && !empty($referer_parts['path'])) {
        $term = $referer;
      }
      if (empty($campaign)) {
        $campaign = 'rss';
      }
    }
    elseif (preg_match('/('.join('|', $engines).')/', $referer_parts['host'])) {
      $medium = self::MEDIUM_SEARCH;
      $source = $referer_parts['host'];
      foreach ($engines as $k => $regexp) {
        if (preg_match('/('.$regexp.')/', $referer_parts['host'])) {
          $source = $k;
          break ;
        }
      }
      if ($source === 'search') {
        $source = $referer_parts['host'];
      }
      if (count($referer_parts['params']) > 0) {
        if (isset($referer_parts['params']['q'])) {
          $term = $referer_parts['params']['q'];
        }
        elseif (strpos($source, 'yahoo') !== false) {
          $term = $referer_parts['params']['p'];
        }
        else {
          foreach ($tags as $tag) {
            if (array_key_exists($tag, $referer_parts['params'])) {
              $term = $referer_parts['params'][$tag];
              if (!empty($term)) break ;
            }
          }
        }
        if (empty($term)) {
          if ($source == 'google' && !empty($referer_parts['params']['prev'])) {
            $source .= $referer_parts['path'] == '/imgres' ? '-image' : '-translate';
            $prev = urldecode($referer_parts['params']['prev']);
            if ($prev[0] === '/') {
              $prev = 'http://localhost'.$prev;
            }
            $sub = array();
            parse_str(parse_url($prev, PHP_URL_QUERY), $sub);
            if (!empty($sub['q'])) {
              $term = $sub['q'];
            }
          }
        }
        $term = trim($term);
      }
      if ($source == 'plus.google.com') {
        $medium = self::MEDIUM_SOCIAL;
        $source = 'google+';
        if (empty($term) && !empty($referer_parts['path'])) {
          $term = $referer;
        }
      }
      elseif (empty($term)) {
        $term = '(not provided)';
      }
      $term = SH::decodeHtmlEntities($term);
    }
    elseif (isset($params['knxm']) && self::$mediums[$params['knxm']]) {
      $medium = $params['knxm'];
      $source = (string) $params['knxs'];
      $term = (string) $params['knxt'];
      $campaign = (string) $params['knxc'];
    }
    elseif ($medium == self::MEDIUM_DIRECT) {
      if (strpos($referer, 'http://') === 0 || strpos($referer, 'https://') === 0) {
        $medium = self::MEDIUM_LINK;
        if (empty($source)) {
          $source = $referer_parts['host'];
          $a = array_intersect_key($fb_params, $params);
          if (!empty($a)) {
            $medium = self::MEDIUM_SOCIAL;
            $source = 'facebook.com';
          }
          elseif (preg_match(P('app.regexp'), $source)) {
            $medium = self::MEDIUM_DIRECT;
          }
          elseif (empty($source)) {
            S('log')->debug($referer_parts, __METHOD__);
            $source = 'unknown';
          }
          elseif (preg_match('/('.$social_regexp.')/', $source) || 
                  in_array($source, array('t.co', 'fb.me', 'lnkd.in', 'pocket.co'))) {
            $medium = self::MEDIUM_SOCIAL;
            $names = explode('|', $social_regexp);
            foreach ($names as $name) {
              if (strpos($source, $name) !== false) {
                $source = $name;
                break ;
              }
            }
            if ($source == 't.co') $source = 'twitter';
          }
          if (empty($term) && !empty($referer_parts['path'])) {
            $term = $referer;
          }
        }
      }
      else {
        $source = 'browser';
      }
    }

    if (substr($term, 0, 7) === 'http://') {
      $term = preg_replace('#^http://#i', '', $term);
    }

    $source = SH::clean($source, array('strict' => true));
    $term = SH::clean($term, array('strict' => true));
    $campaign = SH::clean($campaign, array('strict' => true));

    $attrs = array();
    $attrs['medium']   = $medium;
    $attrs['source']   = self::str2id($source, self::EXPR_SOURCE);
    $attrs['term']     = self::str2id($term, self::EXPR_TERM);
    $attrs['campaign'] = self::str2id($campaign, self::EXPR_CAMPAIGN);

    // error_log($medium.' - '.$source.' - '.$term.' - '.$campaign."\nurl: ".$referer);

    return $attrs;
  }

  private static $tracker = null;

  public static function getInstance() {
    if (self::$tracker === null) {
      $req = S('req');
      $renew = $req->getAction() === 'track';
      self::$tracker = new Tracker($renew);
    }
    return self::$tracker;
  }

  public static function str2id($str, $type, $create=true) {
    $str = (string) $str;
    if (strlen($str) < 1) return 0;
    $str = substr($str, 0, 128);
    $type = (int) $type;
    $id = (int) S('dbo')->fetchValue('SELECT id'
                                     .' FROM '.self::TABLE_EXPRESSION
                                     .' WHERE type='.$type
                                     .  ' AND name=:name:',
                                     array(':name:' => $str));
    if ($id < 1 && $create == true) {
      $id = S('dbo')->insert(self::TABLE_EXPRESSION,
                             array('type' => $type,
                                   'name' => $str));
    }
    return $id;
  }

  public static function toEvent($action, $category) {
    $req = S('req');
    $default = $req->isXhr() ? Tracker::CATEGORY_EVENT : Tracker::CATEGORY_PAGE;
    if ($action instanceof ActiveRecord && ($visit_id = $req->getVisitId()) > 0) {
      $obj = $action;
      $action = $obj->getName(). ' / '.$obj->getModel();
    }
    elseif ($action === true) {
      if ($category) {
        $action = $req->getAction();
      }
      else {
        $action = $req->getAction();
        if ($default == Tracker::CATEGORY_EVENT) {
          $action .= ' / '.$req->getExtension();
        }
        $category = $default;
      }
    }
    $category = $category ?: $default;
    return array($action, $category);
  }

  public static function onAppReset() {
    $dbo = S('dbo');
    foreach (self::$tables as $table) {
      if (!$dbo->hasTable($table)) {
        S('log')->trace('table unknown ['.$table.']', __METHOD__);
        continue ;
      }
      $dbo->getTable($table)->truncate();
    }
  }

  public static function asSourceLink($source, $label=null) {
    if ($source[0] != '|') return $source;
    $parts = explode('|', trim($source, '|'));
    if (!Model::exists($parts[0])) return $source;
    $source = Item::link(array($parts[0], $parts[1]), $label);
    return $source;
  }

  public static function asSourceItem($source) {
    if ($source[0] != '|') return null;
    $parts = explode('|', trim($source, '|'));
    if (!Model::exists($parts[0])) return null;
    $item = Item::getInstance($parts[0], (int) $parts[1]);
    return $item;
  }
  
}
