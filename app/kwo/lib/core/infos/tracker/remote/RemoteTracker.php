<?php

// Leads : 
// cd (commande), ct (contact), ml (mail), fm (form), af (affil), dw (download), c1 (custom1), c2 (custom2)

class RemoteTracker {
  
  //---------------------------------------------------------------------------------------------------

  var $bad_ip_arr       = array('192.200.1.42'); 
  var $cookie_name      = 'tracker';
  var $domain           = 'www.kwo3.inerd';
  var $ttl              = 14400; // 4H
  var $visitor_key_name = '_kvk';

  //---------------------------------------------------------------------------------------------------

  function RemoteTracker () {
    $this->http_user_agent = strtolower($_SERVER['HTTP_USER_AGENT']);
    $this->http_referer    = strtolower($_SERVER['HTTP_REFERER']);
    if ($this->canTrack()===false) {
      $this->valid_flag = false;
      return false; 
    }
    $this->valid_flag = true;
    $this->loadCookie(); 
  }
  
  function canTrack() {
    if (array_key_exists($this->cookie_name,$_COOKIE)) return true;
    if (in_array($_SERVER['REMOTE_ADDR'],$this->bad_ip_arr)) return false;
    if (preg_match('/(bot|slurp|crawl|java|php|spid|seek|walk|gulli|'.
		   'arach|ferret|worm|fetch|jee|indy|track|stealth|'.
		   's-t-o-n-e|sched|scoot|check|appie|libwww|robo|archi|'.
		   'harvest|nutch|blog|feed|check|borg|voyager)/',$this->http_user_agent)) {
      $this->logRobot();
      return false;
    }
    return true;
  }
  
  function loadCookie() {
    if (array_key_exists($this->cookie_name,$_COOKIE)) {
      if (get_magic_quotes_gpc()==1) {
	$cookie_val = stripslashes($_COOKIE[$this->cookie_name]);
      }
      else $cookie_val = $_COOKIE[$this->cookie_name];
      $this->cookie_arr = (array) @unserialize($cookie_val);
    }
    
    if ($this->cookie_arr['visitor_id']<1 || $this->cookie_arr['visit_id']<1) {
      $this->initSig();
      $this->send_flag = true;
      $this->cookie_arr = array();      
      $this->cookie_arr['visit_id']   = $ret['visit_id'];
      $this->cookie_arr['number']  = 1;
      $this->cookie_arr['prev_time']  = 0;
      $this->cookie_arr['last_time']  = time();
      $this->cookie_arr['first_time'] = time();

      if (($arr=$this->inDb())!=false) {
	$this->cookie_arr['visitor_id'] = $arr['visitor_id'];
	$this->cookie_arr['visit_id']   = $arr['visit_id'];
	$this->is_new_visit = false;
	$this->used_db_flag   = true;
      }
      else {
	$this->is_new_visit           = true;
	$this->cookie_arr['visitor_id'] = $this->getVisitorId();
      }
    }
    else {
      if ((time()-$this->cookie_arr['last_time'])>$this->ttl) {
	$this->is_new_visit          = true;
	$this->cookie_arr['number'] = $this->cookie_arr['number']+1;
	$this->cookie_arr['prev_time'] = (int) $this->cookie_arr['last_time'];
	$this->cookie_arr['last_time'] = time();
      }
      else $this->is_new_visit = false;
    }
    
    if ($this->is_new_visit===true && $this->cookie_arr['visitor_id']>=1) {
      $this->cookie_arr['visit_id'] = (int) $this->logVisit();
    }
    $this->sendCookie();
  }

  function getVisitorId() {
    $url_arr['sig']         = $this->sig;
    $url_arr['visitor_key'] = $_SERVER[$this->visitor_key_name];
    $visitor_id = (int) $this->submit('track_visitor',$url_arr);
    return $visitor_id;
  }

  function inDb() {
    $url_arr['sig']             = $this->sig;
    $url_arr['visitor_key']     = $_SERVER[$this->visitor_key_name];
    $ret = $this->submit('get_visitor',$url_arr);
    if (strpos($ret,':')===false) return false;
    $arr = array();
    list($arr['visitor_id'],$arr['visit_id']) = explode(':',$ret);
    return $arr;
  }

  function logRobot() {
    $url_arr['request_uri']     = $_SERVER['REQUEST_URI'];
    $url_arr['http_user_agent'] = $_SERVER['HTTP_USER_AGENT'];
    $this->submit('track_robot',$url_arr);
  }

  function logVisit() {
    if ($this->cookie_arr['visitor_id']<1) return false;
    if (preg_match('/'.$_SERVER['SERVER_NAME'].'/',$this->http_referer) || 
	strpos($this->http_referer,'.')===false) $this->http_referer = '';
    $url_arr['visitor_id']   = $this->cookie_arr['visitor_id'];
    $url_arr['number']    = $this->cookie_arr['number'];
    $url_arr['http_referer'] = $this->http_referer;
    $url_arr['remote_ip']    = $_SERVER['REMOTE_ADDR'];
    $url_arr['user_agent']   = $this->http_user_agent;
    $url_arr['prev_time']    = $this->cookie_arr['prev_time'];
    $url_arr['first_time']   = $this->cookie_arr['first_time'];
    $url_arr['request_uri']  = 'http://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'];
    $visit_id = (int) $this->submit('track_visit',$url_arr);
    return $visit_id;
  }

  function logPage($title,$chapter='',$lead='') {
    if ($this->valid_flag!=true) return false;
    if ($this->cookie_arr['visit_id']<1 || 
	$this->cookie_arr['visitor_id']<1) return false;
    $url_arr['visit_id']   = $this->cookie_arr['visit_id'];
    $url_arr['visitor_id'] = $this->cookie_arr['visitor_id'];
    $url_arr['title']      = $title;
    $url_arr['chapter']    = $chapter;
    $url_arr['lead']       = $lead;
    $this->submit('track_page',$url_arr);
  }

  function submit($action,$arr) {
    foreach ($arr as $k => $v) $arr[$k] = $k.'='.urlencode($v);
    $url = 'http://'.$this->domain.'/front/tracker/'.$action.'?'.join('&',$arr);
    $ret = @file_get_contents($url);
    if ($ret===false) return false;
    $ret = trim($ret);
    return $ret;
  }
  
  function sendCookie() { 
    if ($this->is_new_visit===true || $this->send_flag===true) {
      if (headers_sent()) return error_log('headers already sent'); 
      setcookie($this->cookie_name,serialize($this->cookie_arr),time()+31536000,'/'); 
    }
  }

  function reset() { setcookie($this->cookie_name,'',time()+3600,'/'); }
 
  function initSig() {
    $this->sig .= substr($_SERVER['REMOTE_ADDR'],0,15);
    $this->sig .= substr($_SERVER['HTTP_USER_AGENT'],-100);
    $this->sig .= substr($_SERVER['HTTP_ACCEPT'],0,30);
    $this->sig .= substr($_SERVER['HTTP_ACCEPT_LANGUAGE'],0,10);
  }

}