<?php

/**
Ordre (pour classes, scripts, styles, etc.)
1/ defaults( ex. prototype etc.)
2/ P('SCOPE.head')
3/ $res->addScript
4/ H::head($opts)

$this->metas[]   = array(key1 => val1, key2 => val2)

https://developers.google.com/analytics/devguides/collection/upgrade/reference/gajs-analyticsjs

**/

class HtmlDocument
{
  private $scope = null;
  private $extension = null;
  private $action = null;

  private $locale = null;
  private $suffix = null;

  private $space = null;

  private $events = array();
  private $interactions = array();

  private $charset = null;
  private $defer = null;
  private $doctype = null;
  private $lang = null;
  private $prefix = null;
  private $title = null;

  private $classes = array();
  private $feeds = array();
  private $icons = array();
  private $links = array();
  private $metas = array();
  private $namespaces = array();
  private $scripts = array();
  private $styles = array();
  private $vars = array();

  public function __construct() {
    $req = S('req');

    $this->scope = $req->getScope();
    $this->extension = P('app.extension');
    $this->action = $req->getAction();

    $this->locale = S('res')->getLocale();
    $this->suffix = P('app.head.version') ? '?'.intval(P('app.head.version')) : '';

    $this->setClasses();
    $this->setLinks();
    $this->setMetas();
    $this->setNameSpaces();
    $this->setScripts();
    $this->setStyles();

    $this->doctype = 'HTML5';

    if ($req->isAccount()) {
      $this->loadParams(P('front.head'));
    }

    $this->loadParams(P($this->scope . '.head'));

  }

  public function loadParams($params) {
    $keys = array('classes', 'feeds', 'icons', 'links', 
                  'metas', 'namespaces',
                  'scripts', 'styles', 'vars');

    $params = (array) $params;
    foreach ($keys as $key) {
      $params[$key] = (array) $params[$key];
    }

    foreach ($params['classes'] as $elt)         $this->addClass($elt);
    foreach ($params['feeds'] as $name => $url)  $this->addFeed($name, $url);
    foreach ($params['icons'] as $elt)           $this->addIcon($elt);
    foreach ($params['links'] as $elt)           $this->addLink($elt);
    foreach ($params['metas'] as $key => $value) $this->addMeta($key, $value);
    foreach ($params['namespaces'] as $key => $value) $this->addNameSpace($key, $value);
    foreach ($params['scripts'] as $elt)         $this->addScript($elt);
    foreach ($params['styles'] as $elt)          $this->addStyle($elt);
    foreach ($params['vars'] as $key => $value)  $this->addVar($key, $value);

    if ($params['charset']) $this->setCharset($params['charset']);
    if ($params['doctype']) $this->setDoctype($params['doctype']);
    if ($params['lang'])    $this->setLang($params['lang']);
    if ($params['prefix'])  $this->setPrefix($params['prefix']);
    if ($params['title'])   $this->setTitle($params['title']);

  }

  public function addClass($class_name) { $this->classes[] = $class_name; }
  public function addFeed($url, $name)  { $this->feeds[$name] = $url; }
  public function addIcon($icon)        { $this->icons[] = $icon; }
  public function addLink($link)        { $this->links[] = $link; }
  public function addMeta($key, $value) { $this->metas[$key] = is_array($value) ? join(', ', $value) : $value; }
  public function addNameSpace($key, $value) { $this->namespaces[$key] = $value; }
  public function addScript($script)    { $this->scripts[] = $script; }
  public function addStyle($style)      { $this->styles[] = $style; }
  public function addVar($key, $value)  { $this->vars[$key] = $value; }
  public function addEvent($elt)        { $this->events[] = $elt; }
  public function addInteraction($elt)  { $this->interactions[] = $elt; }

  public function setCharset($charset) { $this->charset = $charset; }
  public function setDoctype($doctype) { $this->doctype = $doctype; }
  public function setLang($lang)       { $this->lang = $lang; }
  public function setPrefix($prefix)   { $this->prefix = $prefix; }
  public function setTitle($title)     { $this->title = $title; }
  public function setSpace($space)     { $this->space = $space; }

  public function setClasses() {
    $this->addClass('scope-'.$this->scope);
    $this->addClass('extension-'.$this->extension);
    $this->addClass('action-'.str_replace('.', '-', $this->action));
    if (S('req')->isTraced()) {
      $this->addClass('page-traced');
    }
  }

  public function setStyles() {
    $scope = $this->scope;
    $extension = $this->extension;
    $action = $this->action;

    $this->addStyle(WEB_PATH.'/'.$extension.'/styles/master.css');

    if ($scope == 'account') {
      $this->addStyle(WEB_PATH.'/'.$extension.'/styles/front.css');
    }
    $this->addStyle(WEB_PATH.'/'.$extension.'/styles/'.$scope.'.css');
    $this->addStyle(WEB_PATH.'/'.$extension.'/styles/'.$scope.'/'.$action.'.css');

//    $files = array('viewport.css', 'ie.css');
    $files = array('viewport.css');
    $dir = $scope == 'account' ? 'front' : $scope;
    foreach ($files as $file) {
      $this->addStyle(WEB_PATH.'/'.$extension.'/styles/'.$dir.'/'.$file);
    }

    // TODO : a supprimer par la suite
    if ($scope === 'front' || $scope === 'middle' || $scope === 'account') {
      $this->addStyle(WEB_PATH.'/'.$extension.'/styles/'.$action.'.css');
    }

  }

  public function setScripts() {
    $scope = $this->scope;
    $extension = $this->extension;
    $action = $this->action;

    if ($scope !== 'back' && !P('app.head.prototype.disable')) $this->addScript(WEB_PATH.'/core/scripts/vendors/prototype.js');
    if ($scope !== 'back' && !P('app.head.kwo.disable')) $this->addScript(WEB_PATH.'/core/scripts/kwo.js');

    $this->addScript(WEB_PATH.'/'.$extension.'/scripts/master.js');

    if ($scope == 'account') {
      $this->addScript(WEB_PATH.'/'.$extension.'/scripts/front.js');
    }
    $this->addScript(WEB_PATH.'/'.$extension.'/scripts/'.$scope.'.js');

    $files = array($action.'.js');
    foreach ($files as $file) {
      $this->addScript(WEB_PATH.'/'.$extension.'/scripts/'.$scope.'/'.$file);
    }

    // TODO : a supprimer par la suite
    if ($scope === 'front' || $scope === 'middle' || $scope === 'account') {
      $this->addScript(WEB_PATH.'/'.$extension.'/scripts/'.$action.'.js');
    }

  }

  public function setMetas() {
    if (P('google.verification')) {
      $this->addMeta('google-site-verification', P('google.verification'));
    }
    $this->addMeta('charset', $this->charset ?: 'utf-8');
  }

  public function setNameSpaces() {
    /* <html xmlns="http://www.w3.org/1999/xhtml"
             xmlns:og="http://ogp.me/ns#"
             xmlns:fb="https://www.facebook.com/2008/fbml"> */
  }

  public function setLinks() {
    $favicon = 'favicon.png';
    $link = array('rel' => 'shortcut icon',
                  'href' => WEB_URL.'/'.$this->extension.'/images/'.$favicon.$this->suffix,
                  'type' => 'image/x-icon');
    $this->addLink($link);
    $link = array('rel' => 'alternate',
                  'href' => null,
                  'type' => 'application/rss+xml',
                  'title' => null);
    foreach ($this->feeds as $name => $url) {
      $link['href'] = $url;
      $link['title'] = $name;
      $this->addLink($link);
    }
  }

  private function isRemote($path) {
    return ($path[1] == '/' || $path[4] == ':' || $path[5] == ':');
  }

  public function getDoctypePart() {
    $doctype = $this->doctype ?: 'HTML5';
    if ($doctype == 'HTML5') {
      $html = '<!doctype html>';
    }
    else {
      if ($doctype == 'HTML4') {
        $parts = array('HTML', 'PUBLIC',
                       '"-//W3C//DTD HTML 4.01//EN"',
                       '"http://www.w3.org/TR/html4/strict.dtd"');
      }
      else {
        $parts = array('HTML', 'PUBLIC',
                       '"-//W3C//DTD XHTML 1.0 Transitional//EN"',
                       '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"');
      }
      $html = '<!DOCTYPE '.join(' ', $parts).'>';
    }
    return $html;
  }

  public function getHtmlPart() {
    if ($this->doctype == 'HTML5') {
      $lang = $this->lang ?: I18N::$langs[$this->locale];
      $tag = 'html lang="'.$lang.'"';
      foreach ($this->namespaces as $ns => $url) {
        $tag .= ' xmlns:og="'.$url.'"';
      }
//      $tag .= '>';
      $lines = array();
      $lines[] = '<!--[if IE 7]><'.$tag.' class="lt-ie10 ie7"><![endif]-->';
      $lines[] = '<!--[if IE 8]><'.$tag.' class="lt-ie10 ie8"><![endif]-->';
      $lines[] = '<!--[if IE 9]><'.$tag.' class="lt-ie10 ie9"><![endif]-->';
      $lines[] = '<!--[if gt IE 9]><!--><'.$tag.'><!--<![endif]-->';
      $html = join("\n", $lines);
    }
    elseif ($this->doctype == 'HTML4') {
      $html = '<html>';
    }
    else {
      $html = '<html xmlns="http://www.w3.org/1999/xhtml">';
    }
    return $html;
  }

  public function getHeadPart() {
    if ($this->prefix) {
      $html = '<head prefix="'.$this->prefix.'">';
    }
    else {
      $html = '<head>';
    }
    return $html;
  }

  public function getScriptPart() {
    $script = '  var _scope = "'.$this->scope.'";'."\n";
    if (count($this->vars) > 0) {
      $vars = array();
      foreach ($this->vars as $key => $value) {
        if (is_numeric($value)) {
        }
        else {
          $value = '"'.$value.'"';
        }
        $vars[] = $key.' = '.$value;
      }
      $script .= '  var '.join(', ', $vars).';'."\n";
    }
    if (!empty($this->events)) {
      $events = array();
      foreach ($this->events as $event) {
        $parts = array('"'.$event[0].'"', '"'.$event[1].'"', AH::seal($event));
        $events[] = '['.join(',', $parts).']';
      }
      $script .= '  var _events = ['.join(', ', $events).'];'."\n";
    }
    if (!empty($this->interactions)) {
      $interactions = array();
      foreach ($this->interactions as $interaction) {
        $parts = array('"'.$interaction[0].'"', '"'.$interaction[1].'"', '"'.$interaction[3].'"', AH::seal($interaction));
        $interactions[] = '['.join(',', $parts).']';
      }
      $script .= '  var _interactions = ['.join(', ', $interactions).'];'."\n";
    }
    
    $html  = ' <script type="text/javascript">'."\n";
    $html .= $script;
    $html .= ' </script>';
    
    if (P('google.analytics.account')) {
      $domain = P('google.analytics.domain') ?: 'auto';
      $lines = array();
      $lines[] = ' <script>';
      $lines[] = '  (function(i,s,o,g,r,a,m){i["GoogleAnalyticsObject"]=r;i[r]=i[r]||function(){';
      $lines[] = '  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),';
      $lines[] = '  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)';
      $lines[] = '  })(window,document,"script","//www.google-analytics.com/analytics.js","ga");';
      $lines[] = '  ga("create", "'.P('google.analytics.account').'", "'.$domain.'");';
      $lines[] = '  ga("send", "pageview");';
      $lines[] = ' </script>';
      $html .= "\n".join("\n", $lines);
    }

    return $html;
  }

  public function getBodyPart() {
    return '</head>'."\n\n".'<body class="'.join(' ', $this->classes).'">'."\n\n";
  }

  private function getTitlePart() {
    $title = $this->title ?: P('app.name');
    if (strpos($title, '’') !== false) {
      $title = preg_replace('/’/u', "'", $title);
    }
    $html = ' <title>'.h(c($title)).'</title>';
    return $html;
  }

  private function getStylesPart() {
    $styles = array();
    foreach ($this->styles as $style) {
      $path = $style;
      $url = $ts = null;
      if ($this->isRemote($path)) {
        $url = $path;
      }
      else {
        if ($path[0] == '/') {
          $url = $path;
          $path = ltrim($path, '/');
        }
        else {
          $url = '/'.$path;
        }
        $ts = (int) @filemtime($path);
        if ($ts < 1) {
          //S('log')->error('invalid style path ['.$path.']', __METHOD__);
          continue ;
        }
        if ($this->suffix) {
          $url .= $this->suffix;
        }
//      d($style.':'.date('Y-m-d H:i:s', filemtime(trim($style, '/'))));
      }
      $styles[] = array($path, $url, $ts);
    }
    $styles = $this->cacheAssets($styles);
    $lines = array();
    $attrs = array('rel' => 'stylesheet',
                   'href' => null,
                   'type' => 'text/css',
                   'media' => null);
    foreach ($styles as $style) {
      $attrs['href'] = $style[1];
      $attrs['media'] = strpos($style[0], 'print.') !== false ? 'print' : 'screen';
      $lines[] = ' <link '.AH::asAttributes($attrs, true).' />';
    }
    $html = join("\n", $lines);
    return $html;
  }

  private function getScriptsPart() {
    $scripts = array();
    foreach ($this->scripts as $script) {
      $path = $script;
      $url = $ts = null;
      if ($this->isRemote($path)) {
        $url = $path;
      }
      else {
        if ($path[0] == '/') {
          $url = $path;
          $path = ltrim($path, '/');
        }
        else {
          $url = '/'.$path;
        }
        $ts = (int) @filemtime($path);
        if ($ts < 1) {
          //S('log')->error('invalid script path ['.$path.']', __METHOD__);
          continue ;
        }
        if ($this->suffix) {
          $url .= $this->suffix;
        }
      }
      $scripts[] = array($path, $url, $ts);
    }
    $scripts = $this->cacheAssets($scripts);
    $lines = array();
    $attrs = array('src' => null,
                   'type' => 'text/javascript');
    foreach ($scripts as $script) {
      $attrs['src'] = $script[1];
      if ($script[2] >= 1 && !P('app.head.defer.disable')) $attrs['defer'] = 'defer';
      $lines[] = ' <script '.AH::asAttributes($attrs, true).'></script>';
      unset($attrs['defer']);
    }
    $html = join("\n", $lines);
    return $html;
  }

  /* 
     $assets
     scripts[] = array(path, url, ts)
     styles[]  = array(path, url, ts)
  */
  private function cacheAssets($assets) {

    $ext = 'js';
    if (strpos($assets[0][0], '.css') > 2) {
      $ext = 'css';
    }

    $path = WEB_PATH.'/'.$this->extension.'/cache';
    if (!is_writable($path)) return $assets;

    $action = $this->action;
    $chunk = (string) P('app.head.version');
    foreach ($assets as $asset) {
      if ($asset[2] < 1) continue ;
      $chunk .= $asset[0];
    }
    $key = 0;
    for ($i = 0; $i < strlen($chunk); $i++) {
      $key += ord($chunk[$i]);
    }
    $cache = $path.'/'.$this->scope.'.'.$action.'.'.$key.'.'.$ext;
    $max = (int) @filemtime($cache);
    $update = $max < 1;
    $locals = $tmp = array();
    
    foreach ($assets as $asset) {
      if ($ext === 'js' && strpos($asset[0], '/'.CORE.'/scripts/') > 0) {
        $tmp[] = $asset;
        continue ;
      }
      if ($asset[2] < 1) {
        $tmp[] = $asset;
        continue ;
      }
      if ($asset[2] > $max) {
        $update = true;
      }
      $locals[] = $asset;
    }
    if ($update === true) {
      $fd = fopen($cache, 'w');
      foreach ($locals as $asset) {
        $file = $asset[0];
        $content  = '/***============= '.$file.' =============***/' ."\n";
        $content .= file_get_contents($file) . "\n\n";
        fwrite($fd, $content);
      }
      fclose($fd);
      $tmp[] = array($cache, '/'.$cache.$this->suffix, time());
    }
    else {
      $tmp[] = array($cache, '/'.$cache.$this->suffix, time());
    }
    return $tmp;
  }

  public function getMetasPart() {
    $lines = array();
    foreach ($this->metas as $key => $value) {
      if (empty($value) || $key === 'title') continue ;
      $attrs = array();
      if (strpos($key, ':') > 0) {
        $attrs['property'] = $key;
      }
      else {
        $attrs['name'] = $key;
      }
      $attrs['content'] = $value;
      $lines[] = ' <meta '.AH::asAttributes($attrs, true).' />';
    }
    sort($lines);
    return join("\n", $lines);
  }

  private function getLinksPart() {
    $lines = array();
    foreach ($this->links as $link) {
      $lines[] = ' '.H::elt('link', null, $link);
    }
    sort($lines);
    return join("\n", $lines);
  }

  private function getIePart() {
    $lines = array();
    $lines[] = ' <!--[if IE]>';
    $lines[] = ' <link rel="stylesheet" href="'.WEB_URL.'/'.$this->extension.'/styles/ie.css'.$this->suffix.'" type="text/css" />';
    $lines[] = ' <script> (function() { var html5 = ("abbr,article,aside,audio,canvas,datalist,details," + "figure,footer,header,hgroup,mark,menu,meter,nav,output," + "progress,section,time,video").split(","); for (var i = 0; i < html5.length; i++) { document.createElement(html5[i]); } try { document.execCommand("BackgroundImageCache", false, true); } catch(e) {} })(); </script>';
    $lines[] = ' <![endif]-->';
    return join("\n", $lines);
  }

  public function getHead($opts=null) {
    $this->loadParams($opts);
    S('app')->hook('onHtmlDocumentBuild', array('doc' => $this));
    $this->cacheAssets();
    $parts  = array();
    $parts[] = $this->getDoctypePart();
    $parts[] = $this->getHtmlPart();
    $parts[] = $this->getHeadPart();
    $parts[] = $this->getTitlePart();
    $parts[] = $this->getStylesPart();
    $parts[] = $this->getLinksPart();
    $parts[] = $this->getMetasPart();
    $parts[] = $this->getScriptPart();
    $parts[] = $this->getScriptsPart();
    if (S('req')->isIe()) {
      $parts[] = $this->getIePart();
    }
    $parts[] = $this->getBodyPart();
    if (S('req')->isFront()) {
      S('res')->track();
    }
    return join("\n", $parts);
  }

  public function getFoot($opts) {
    $opts = (array) $opts;
    $html = '';
    if ($opts['debug'] === true) {
      $values = S('ctx')->getAttributes('debug');
      $values['_content'] = 'hidden';
      $html .= Template::render(CORE.':foot.debug', array('context' => $values));
    }

    $html .= "\n".'</body></html>';
    return $html;
  }

  private static $singleton = null;

  public static function getInstance($opts=null) {
    if (!self::$singleton) {
      $opts = (array) $opts;
      self::$singleton = new HtmlDocument($opts);
    }
    return self::$singleton;
  }

  public static function clean() {
    $files = glob(WEB_PATH.'/*/cache/*');
    $files = $files ?: array();
    $now = time();
    $ttl = 6 * 60 * 60;
    foreach ($files as $file) {
      if (!file_exists($file)) continue ;
      $ts = filemtime($file);
      if (($now - $ts) > $ttl) {
        unlink($file);
      }
    }
  }

}
