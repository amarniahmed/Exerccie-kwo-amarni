<?php

// X-Frame-Options: deny

final class H
{
  private static $locale = 0;
  private static $currency = null;

  private static $scripts = null;

  // TODO : à supprimer
  public static function head($opts=null) {

//    if (S('req')->getExtension() == DEVEL) {
    $doc = S('res')->getHtmlDocument();
    return $doc->getHead($opts);
//    }


    $req = S('req');
    $opts = (array) $opts;
    $opts['ie'] = isset($opts['ie']) ? $opts['ie'] : true;
    $opts['doctype'] = $opts['doctype'] ?: 'HTML5';
    $opts['defer'] = isset($opts['defer']) ? $opts['defer'] : true;
    $res = S('res');
    $ctx = S('ctx');
    $ha = $res->getHead();
    $ext = P('app.extension');
    $lang = $ha['lang'] ?: I18N::$langs[self::locale()];
    $title = $ha['title'] ?: P('app.name');
    $html  = ' <title>'.h(c($title)).'</title>'."\n";
    if ($opts['doctype'] === 'HTML5') {
      $html .= ' <meta charset="utf-8">'."\n";
    }
    else {
      $html .= ' <meta http-equiv="Content-Language" content="'.$lang.'" />'."\n";
      $html .= ' <meta http-equiv="Content-Script-Type" content="text/javascript" />'."\n";
      $html .= ' <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />'."\n";
    }

    foreach (array('scripts', 'styles', 'feeds', 'links', 'metas', 'classes') as $value) {
      $opts[$value] = (array) $opts[$value];
    }

    $scopes = array('front', 'account', 'middle');
    $scope = $req->getScope();

    $scripts = array(WEB_URL.'/core/scripts/prototype.js',
                     WEB_URL.'/core/scripts/kwo.js');
    $script = WEB_PATH.'/'.$ext.'/scripts/master.js';
    if (file_exists($script)) {
      $scripts[] = '/'.$script;
    }
    if ($scope === 'front' || $scope === 'account') {
      $script = WEB_PATH.'/'.$ext.'/scripts/front.js';
      if (file_exists($script)) {
        $scripts[] = '/'.$script;
      }
      if ($scope === 'account') {
        $script = WEB_PATH.'/'.$ext.'/scripts/account.js';
        if (file_exists($script)) {
          $scripts[] = '/'.$script;
        }
      }
    }
    elseif ($scope === 'middle') {
      $script = WEB_PATH.'/'.$ext.'/scripts/middle.js';
      if (file_exists($script)) {
        $scripts[] = '/'.$script;
      }
    }
    if ($scope === 'front' || $scope === 'middle' || $scope === 'account') {
      $script = WEB_PATH.'/'.$ext.'/scripts/'.$req->getAction().'.js';
      if (file_exists($script)) {
        $scripts[] = '/'.$script;
      }
    }

    $opts['scripts'] = array_merge($opts['scripts'], $ha['scripts']);
    foreach ($opts['scripts'] as $key => $script) {
      $script = strpos($script, '/') === false
              ? WEB_URL.'/'.$ext.'/scripts/'.$script.'.js'
              : $script;
      $opts['scripts'][$key] = $script;
    }
    $scripts = array_merge($scripts, $opts['scripts'], (array) P($scope.'.scripts'));

    //STYLES
    $styles = array();
    $style = WEB_PATH.'/'.$ext.'/styles/master.css';
    if (file_exists($style)) {
      $styles[] = '/'.$style;
    }
    if ($scope === 'front' || $scope === 'account') {
      $style = WEB_PATH.'/'.$ext.'/styles/front.css';
      if (file_exists($style)) {
        $styles[] = '/'.$style;
      }
      if ($scope === 'account') {
        $style = WEB_PATH.'/'.$ext.'/styles/account.css';
        if (file_exists($style)) {
          $styles[] = '/'.$style;
        }
      }
    }
    elseif ($scope === 'middle') {
      $style = WEB_PATH.'/'.$ext.'/styles/middle.css';
      if (file_exists($style)) {
        $styles[] = '/'.$style;
      }
    }
    if ($scope === 'front' || $scope === 'middle' || $scope === 'account') {
      $style = WEB_PATH.'/'.$ext.'/styles/'.$req->getAction().'.css';
      if (file_exists($style)) {
        $styles[] = '/'.$style;
      }
    }
    $opts['styles'] = array_merge($opts['styles'], $ha['styles']);
    foreach ($opts['styles'] as $key => $style) {
      $style = strpos($style, '/') === false
             ? WEB_URL.'/'.$ext.'/styles/'.$style.'.css'
             : $style;
      $opts['styles'][$key] = $style;
    }
    if (file_exists(WEB_PATH.'/'.$ext.'/styles/'.$scope.'/viewport.css')) {
      $styles[] = WEB_URL.'/'.$ext.'/styles/'.$scope.'/viewport.css';
    }
    elseif (file_exists(WEB_PATH.'/'.$ext.'/styles/viewport.'.$scope.'.css')) {
      $styles[] = WEB_URL.'/'.$ext.'/styles/viewport.'.$scope.'.css';
    }
    $styles = array_merge($styles, $opts['styles'], (array) P($scope.'.styles'));
    foreach ($styles as $style) {
//      d($style.':'.date('Y-m-d H:i:s', filemtime(trim($style, '/'))));
    }
    //END STYLES

    $metas = array_merge($opts['metas'], $ha['metas'], (array) P('front.metas'));
    if (P('google.verification')) {
      $metas['google-site-verification'] = P('google.verification');
    }
    $properties = array();
    foreach ($metas as $key => $value) {
      if (empty($value) || $key === 'title') continue ;
      if (strpos($key, ':') > 0) {
        $properties[$key] = $value;
        continue ;
      }
      $html .= ' <meta name="'.$key.'" content="'.h($value).'" />'."\n";
    }

    foreach ($properties as $key => $value) {
      $html .= ' <meta property="'.$key.'" content="'.h($value).'" />'."\n";
    }

    $head_suffix = P('app.head.version') > 0 ? '?'.P('app.head.version') : '';

    foreach ($styles as $style) {
      $media = strpos($style, 'print.') !== false ? 'print' : 'screen';
      if (strpos($style, '//') === false) {
        $style .= $head_suffix;
      }
      $html .= ' <link rel="stylesheet" href="'.$style.'" type="text/css" media="'.$media.'" />'."\n";
    }

    function cmp($a, $b) {
      if ($a['rel'] == $b['rel']) {
        return 0;
      }
      return ($a['rel'] < $b['rel']) ? -1 : 1;
    }

    $feeds = array_merge($opts['feeds'], $ha['feeds'], (array) P('front.feeds'));
    foreach ($feeds as $name => $url) {
      $ha['links'][] = array('rel' => 'alternate',
                             'href' => $url, 'type' =>
                             'application/rss+xml',
                             'title' => $name);
//      $html .= ' <link rel="alternate" href="'.$url.'" type="application/rss+xml" title="'.$name.'" />'."\n";
    }

    $links = array_merge($opts['links'], $ha['links'], (array) P('front.links'));
    usort($links, 'cmp');
    foreach ($links as $h) {
      $params = '';
      foreach ($h as $key => $value) {
        $params .= ' '.$key.'="'.$value.'"';
      }
      $html .= ' <link'.$params.' />'."\n";
    }

    $favicon = $opts['favicon'] ?: 'favicon.png';
    $html .= ' <link rel="shortcut icon" href="'.WEB_URL.'/'.$ext.'/images/'.$favicon.$head_suffix.'" type="image/x-icon" />'."\n";
    //$html .= ' <link rel="icon" href="'.WEB_URL.'/'.$ext.'/images/'.$favicon.$head_suffix.'" type="image/png" />'."\n";

    if ($req->isIe() && !empty($opts['ie'])) {
      if ($opts['ie'] === true) {
        $style = WEB_PATH.'/'.$ext.'/styles/ie.css';
        $path = file_exists($style) ? $ext : CORE;
      }
      else {
        $path = $opts['ie'];
      }
      $html .= ' <!--[if IE]>
 <link rel="stylesheet" href="'.WEB_URL.'/'.$path.'/styles/ie.css'.$head_suffix.'" type="text/css" />
 <![endif]-->'."\n";
      $html .= ' <!--[if IE 9]>
 <link rel="stylesheet" href="'.WEB_URL.'/'.$path.'/styles/ie9.css'.$head_suffix.'" type="text/css" />
 <![endif]-->'."\n";
      $html .= ' <!--[if IE 8]>
 <link rel="stylesheet" href="'.WEB_URL.'/'.$path.'/styles/ie8.css'.$head_suffix.'" type="text/css" />
 <![endif]-->'."\n";
      $html .= ' <!--[if IE 7]>
 <link rel="stylesheet" href="'.WEB_URL.'/'.$path.'/styles/ie7.css'.$head_suffix.'" type="text/css" />
 <![endif]-->'."\n";
      $html .= ' <!--[if IE 6]>
 <link rel="stylesheet" href="'.WEB_URL.'/'.$path.'/styles/ie6.css'.$head_suffix.'" type="text/css" />
 <![endif]-->'."\n";
      $html .= ' <!--[if IE]> <script> (function() { var html5 = ("abbr,article,aside,audio,canvas,datalist,details," + "figure,footer,header,hgroup,mark,menu,meter,nav,output," + "progress,section,time,video").split(","); for (var i = 0; i < html5.length; i++) { document.createElement(html5[i]); } try { document.execCommand("BackgroundImageCache", false, true); } catch(e) {} })(); </script> <![endif]-->'."\n";
    }

    $html .= ' <script type="text/javascript">'."\n";
//    $html .= '  var _user_id = '.intval($ctx->_user['id']).', ';
//    $html .= '_locale = '.self::locale().', ';
//    $html  = '  var _extension = "'.P('app.extension').'", ';
//    $html .= '_scope = "'.$req->getScope().'", _defer = '.intval($opts['defer']).';'."\n";
    $html .= '  var _scope = "'.$req->getScope().'";'."\n";
    if (is_array($opts['vars']) && count($opts['vars']) > 0) {
      $vars = array();
      foreach ($opts['vars'] as $key => $value) {
        if (is_numeric($value)) {
        }
        else {
          $value = '"'.$value.'"';
        }
        $vars[] = $key.' = '.$value;
      }
      $html .= '  var '.join(', ', $vars).';'."\n";
    }
    if (!empty($ha['events'])) {
      $events = array();
      $html .= '  var _events = [';
      foreach ($ha['events'] as $event) {
        $events[] = '["'.$event[0].'","'.$event[1].'",'.AH::seal($event).']';
      }
      $html .= join(', ', $events);
      $html .= '];'."\n";
    }
    if (!empty($ha['intents'])) {
      $intents = array();
      $html .= '  var _intents = [';
      foreach ($ha['intents'] as $intent) {
        $intents[] = '["'.$intent[0].'","'.$intent[1].'","'.$intent[2].'",'.
          '"'.$intent[3].'",'.AH::seal($intent).']';
      }
      $html .= join(', ', $intents);
      $html .= '];'."\n";
    }
    if (P('google.analytics.account')) {
      $html .= '  var _gaq = _gaq || []; _gaq.push(["_setAccount", "'.P('google.analytics.account').'"]);';
      if (P('google.analytics.domain')) {
        $html .= ' _gaq.push(["_setDomainName", "'.P('google.analytics.domain').'"]);';
      }
      $html .= "\n";
    }
    $html .= ' </script>'."\n";

    self::$scripts = '';
    foreach ($scripts as $script) {
      if (strpos($script, '//') === false) {
        $script .= $head_suffix;
      }
      $defer = '';
      if ($opts['defer'] === true) {
        $defer = ' defer="defer"';
      }
      self::$scripts .= ' <script src="'.$script.'"'.$defer.' type="text/javascript"></script>'."\n";
      //self::$scripts .= ' <script src="'.$script.'" defer="defer" type="text/javascript"></script>'."\n";
    }
    if (!($opts['script_loading'] === false)) {
      $html .= self::$scripts;
      self::$scripts = null;
    }

    if ($opts['doctype'] == 'HTML5') {
      $head  = '<!doctype html>'."\n";
      $head .= '<!--[if IE 7]><html lang="'.$lang.'" class="lt-ie10 ie7"><![endif]-->'."\n";
      $head .= '<!--[if IE 8]><html lang="'.$lang.'" class="lt-ie10 ie8"><![endif]-->'."\n";
      $head .= '<!--[if IE 9]><html lang="'.$lang.'" class="lt-ie10 ie9"><![endif]-->'."\n";
      $head .= '<!--[if gt IE 9]><!--><html lang="'.$lang.'"><![endif]-->'."\n";
    }
    elseif ($opts['doctype'] == 'HTML4') {
      $head  = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" ';
      $head .= '"http://www.w3.org/TR/html4/strict.dtd">'."\n";
      $head .= '<html>'."\n\n";
    }
    else {
      $head  = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" ';
      $head .= '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">'."\n";
      $head .= '<html xmlns="http://www.w3.org/1999/xhtml">'."\n\n";
    }

    if ($ha['prefix']) {
      $head .= '<head prefix="'.$ha['prefix'].'">'."\n";
    }
    else {
      $head .= '<head>'."\n";
    }

    $head .= $html;
    $head .= '</head>'."\n";

    $ha['classes'][] = 'scope-'.$req->getScope();
    $ha['classes'][] = 'extension-'.$req->getExtension();
    $ha['classes'][] = 'action-'.str_replace('.', '-', $req->getAction());
    foreach ($opts['classes'] as $value) {
      $ha['classes'][] = $value;
    }
//    if ($opts['defer'] === true) {
//      $ha['classes'][] = 'scripts-deferred';
//    }
    if (S('req')->isTraced()) {
      $ha['classes'][] = 'page-traced';
    }
    $head .= "\n".'<body class="'.join(' ', $ha['classes']).'">'."\n\n";
    $html = $head;
    return $html;
  }

  public static function styles($input, $action, $version) {
    $styles = array();
    $styles = $input;
    if (false) {
      foreach ($input as $style) {
        filemtime(trim($style, '/'));
      }
    }
    return $styles;
  }

  public static function foot($opts=null) {

//    if (S('req')->getExtension() == DEVEL) {
    $doc = S('res')->getHtmlDocument();
    return $doc->getFoot($opts);
//    }

    $opts = (array) $opts;
    $html = '';
    if ($opts['debug'] === true) {
      $values = S('ctx')->getAttributes('debug');
      $values['_content'] = 'hidden';
      $html .= Template::render(CORE.':foot.debug', array('context' => $values));
    }
    if (self::$scripts) {
      $html .= self::$scripts;
    }
    if (P('google.analytics.account')) {
      $html .= <<<EOT
<script type="text/javascript">
_gaq.push(['_trackPageview']);
_gaq.push(['_setDetectFlash', false]);
_gaq.push(['_setAllowHash', false]);
(function() {
  var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
  ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
  var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
 })();
</script>

EOT;
    }
    $html .= "\n".'</body></html>';
    return $html;
  }

  public static function identity($item) {
    $locale = S('res')->getLocale();
    $identity = $item instanceof ActiveRecord ? $item->asIdentityHash($locale) : $item;
    $html = '<table class="item-identity"><tr>'."\n";
    if (!empty($identity['image'])) {
      $html .= '<td class="identity-image">'."\n";
      $html .= ' <img src="'.$identity['image'].'" class="item" />'."\n";
      $html .= '</td>'."\n";
    }
    $html .= '<td>'."\n";
    $html .= ' <strong>'.c($identity['name']).'</strong>'."\n";
    $html .= ' <p>'.c($identity['description']).'</p>'."\n";
    $html .= '</td>'."\n";
    $html .= '</tr></table>'."\n";
    return $html;
  }

  public static function breadcrumb($opts=null) {
    $opts = (array) $opts;
    $separator = $opts['separator'] ? $opts['separator'] : ' > ';
    $res = S('res');
    $breadcrumb = S('ctx')->_breadcrumb;
    if (is_array($opts['prefix']) && is_array($breadcrumb)) {
      $breadcrumb = $opts['prefix'] + $breadcrumb;
    }
    if ($breadcrumb === null) return '';
    $links = array();
    $count = count($breadcrumb);
    $i = 1;
    foreach ($breadcrumb as $label => $url) {
      if (is_int($label)) {
        if ($url instanceof ActiveRecord) {
          $label = $url->getTitle();
          $url = $url->asUrl();
        }
        else {
          $label = $url;
          $url = '/';
        }
      }
      if ($url instanceof ActiveRecord) {
        $url = $url->asUrl();
      }
      $label = ucfirst($label);
      $label = strpos($label, '<') !== false ? h($label) : $label;
      if ($i === $count) {
        $links[] = '<span>'.$label.'</span>';
        break ;
      }
      $links[] = '<a href="'.$url.'">'.$label.'</a>';
      $i++;
    }
    $html .= join($separator, $links);
    return $html;
  }

  public static function error($msg) {
   $template = P('template.error') ? P('template.error') : 'error';
    return Template::render($template, array('_error_msg' => $msg));
  }

  public static function avatar($hash) {
    $hash = (array) ($hash instanceof ActiveRecord ? $hash->getAttributes() : $hash);
    if (($hash['user_flags'] & User::FLAG_IMAGE) > 0) {
      $path = '/'.USR_PATH.'/'.$hash['user_path'].'/_a.jpg';
    }
    else {
      $path = PIX_URL.'/avatar.gif';
    }
    return '<img src="'.$path.'" class="avatar" />';
  }

  public static function button($label, $opts=null) {
    $type = $opts['type'] ?: 'submit';
    $class = 'button';
    $class .= $opts['class'] ? ' '.$opts['class'] : '';
    return '<input type="'.$type.'" class="'.$class.'" value="'.ucfirst($label).'" />';
  }

  public static function ellipse($content, $opts=array()) {
    $content = strip_tags(html_entity_decode($content, ENT_QUOTES, 'UTF-8'));
    $opts['length'] = $opts['length'] ? $opts['length'] : 80;
    return mb_strlen($content, 'UTF-8') < $opts['length']
         ? $content
         : mb_substr($content, 0, $opts['length'], 'UTF-8').'&hellip;';
  }

  public static function user($user) {
    if ($user instanceof ActiveRecord) {
      if ($user->isA(Model::id('user'))) {
        $user_id = $user->getId();
        $user_name = $user->name;
      }
      else {
        $user_id = $user->user_id;
        $user_name = $user->user_name;
      }
    }
    else {
      $user_name = $user;
    }
    return '<span class="kwo-user">'.$user_name.'</span>';
  }

  public static function country($country_id, $locale=null) {
    $locale = $locale >= 1 ? $locale : self::locale();
    $column = 'name'.($locale === I18N::LOCALE_FR ? '_fr' : '_en');
    return S('dbo')->fetchValue('SELECT '.$column
                                .' FROM '.T('country')
                                .' WHERE id='.intval($country_id));
  }

  // Hugues : ajout de $décimals
  public static function amount($sum, $currency_id=null, $decimals=2) {
    $currency = array('rate' => 1,
                      'symbol' => '&euro;',
                      'decimal_point' => ',');

    if ((int) $currency_id >= 1) {
      // mluberry : prise en compte de l'affichage d'une autre devise (forcée)
      $c = Currency::getInstance(array('id' => $currency_id, 'status' => 1));
      $currency = array('rate' => $c->rate,
                        'symbol' => $c->symbol_right ?: $c->code,
                        'decimal_point' => $c->decimal_point);
    }
    else {
      if (!is_array(self::$currency)) {
        if (!S('req')->isHttp()) {
          self::$currency = $currency;
        }
        else {
          $currency_id = S('req')->getCookie()->currency_id;
          if ($currency_id >= 1) {
            try {
              $c = Currency::getInstance(array('id' => $currency_id,
                                               'status' => 1));
              $currency = array('rate' => $c->rate,
                                'symbol' => $c->symbol_right ?: $c->code,
                                'decimal_point' => $c->decimal_point);
            }
            catch (Exception $e) {}
          }
          self::$currency = $currency;
        }
      }
      $currency = self::$currency;
    }
    $value = number_format(($sum * $currency['rate']), $decimals, $currency['decimal_point'], ' ');
    if ($currency['symbol'] === 'USD') {
      $ret = '$'.$value;
    }
    else {
      $ret = $value.' '.$currency['symbol'];
    }
    return $ret;
  }

  public static function decimal($val, $dec=2) {
    return number_format($val, 2, '.', '');
  }

  public static function phone($phone, $opts=null) {
    if (strlen($phone) != 10) return $phone;
    return preg_replace('/(\d{2})/', '$1 ', $phone);
  }

  public static function date($date, $opts=null) {
    if (strlen($date) < 6 || substr($date, 0, 4) === '0000') return '';
    $opts = (array) $opts;
    $locale = $opts['locale'] ?: self::locale();
    $h = DH::asHash($date);
    if ($locale == I18N::LOCALE_FR) {
      $html = $h['day'].'/'.$h['month'].'/'.$h['year'];
    }
    else {
      $html = $h['year'].'-'.$h['month'].'-'.$h['day'];
    }
    if ($opts['user_friendly'] === true) {
      if (DH::isToday($h)) $html = l('today');
      elseif (DH::isYesterday($h)) $html = l('yesterday');
    }
    return $html;
  }

  public static function datetime($date, $opts=null) {
    if (strlen($date) < 6 || substr($date, 0, 4) === '0000') return '';
    $opts = (array) $opts;
    $locale = $opts['locale'] ?: self::locale();
    $h = DH::asHash($date);
    if ($locale == I18N::LOCALE_FR) {
      $html = $h['day'].'/'.$h['month'].'/'.$h['year'];
    }
    else {
      $html = $h['year'].'-'.$h['month'].'-'.$h['day'];
    }
    if (!empty($h['hour'])) {
      $html .= ' '.$h['hour'].':'.$h['min'];
    }
    return $html;
  }

  public static function when($date, $opts=null) {
    if (strlen($date) < 6 || substr($date, 0, 4) === '0000') return '';
    $opts = (array) $opts;
    $locale = $opts['locale'] ?: self::locale();
    $h = DH::asHash($date);
    if ($locale == I18N::LOCALE_FR) {
      $html = $h['day'].'/'.$h['month'].'/'.$h['year'].' '.$h['hour'].':'.$h['min'];
    }
    else {
      $html = $h['year'].'-'.$h['month'].'-'.$h['day'].' '.$h['hour'].':'.$h['min'];
    }
    if (DH::isToday($h)) {
      $n = time() - $h['ts'];
      if ($n < 60) {
        $n = $n >= 1 ? $n : 1;
        $html = l('when.seconds.ago', array('n' => $n));
      }
      elseif ($n < 3600) {
        $html = l('when.minutes.ago', array('n' => floor($n / 60)));
      }
      elseif ($n < 43200) {
        $html = l('when.hours.ago', array('n' => floor($n / 3600)));
      }
      else {
        $html = l('when.today', array('time' => $h['hour'].':'.$h['min']));
      }
    }
    elseif (DH::isYesterday($h)) {
      $html = l('when.yesterday', array('time' => $h['hour'].':'.$h['min']));
    }
    return $html;
  }

  public static function datenice($date) {
    $locale = self::locale();
    $time = strtotime($date);
    $html  = '';
    if ($locale == I18N::LOCALE_EN) {
      $html .= ucfirst(I18N::$months[$locale]['long'][date('n', $time)]);
      $html .= date(' j', $time);
      $html .= ', ';
      $html .= date(' Y ', $time);
      $h = date('H:i', $time);
      if ($h != '00:00') {
        $html .= ' at '.$h;
      }
    }
    else {
      $html .= I18N::$days[$locale]['long'][date('N', $time)];
      $html .= date(' j ', $time);
      $html .= ucfirst(I18N::$months[$locale]['long'][date('n', $time)]);
      $html .= date(' Y', $time);
      $h = date('H:i', $time);
      if ($h != '00:00') {
        $html .= ', '.$h;
      }
    }
    return $html;
  }

  public static function age($date) {
    if (strlen($date) < 6 || substr($date, 0, 4) === '0000') return '';
    $h = DH::asHash($date);
    $today = array('month'=>date('n'), 'day'=>date('j'), 'year'=>date('Y'));
    $years = $today['year'] - $h['year'];
    if ($today['month'] <= $h['month']) {
      if ($h['month'] == $today['month']) {
        if ($h['day'] > $today['day']) {
          $years--;
        }
      }
      else {
        $years--;
      }
    }
    return $years;
  }

  public static function locale() {
    if (self::$locale < 1) {
      self::$locale = S('res')->getLocale();
    }
    return self::$locale;
  }

  public static function escapeHtml($str) {
    return h($str);
  }

  //  http://www.thespanner.co.uk/2011/09/12/protecting-against-xss/
  public static function escapeJs($input) {
    if (strlen($input) < 1) return '';
    $output = strtr($input,
                    array('\\' => '\\\\', "'" => "\\'", '"' => '\\"',
                          "\r" => '\\r', "\n" => '\\n', '</' => '<\/'));
    //$output = preg_replace("/[^\\x01-\\x7F]/", "", $output);
    return $output;
  }

  public static function get($url, $params=null, $opts=null) {
    if ($params instanceof ActiveRecord) {
      $params = array('id' => $params->getId());
    }
    elseif ($params === true) {
      $params = S('ctx')->getAttributes();
    }
    if ($opts instanceof ActiveRecord) {
      $opts = array('item' => $opts);
    }
    $opts = (array) $opts;
    $opts['cache_ttl'] = intval($opts['ttl']) > 0 ? $opts['ttl'] : 0;
    return HttpClient::call($url, $params, $opts);
  }

  public static function inc($templates, $bindings=true) {
    return Template::render($templates, $bindings);
  }

  public static function options($h, $default='') {
    $html = '';
    $h = $h instanceof Collection ? $h->asHash('id', 'name') : $h;
    $default = $default instanceof ActiveRecord ? $default->getId() : $default;
    foreach ($h as $key => $value) {
      $html .= ' <option value="'.$key.'"'.($default == $key ? ' selected="selected"' : '').'>'.$value.'</option>'."\n";
    }
    return $html;
  }

/*  public static function onload($js) {
    return self::trigger($js);
  }*/

  public static function trigger($js, $opts=null) {
    $opts = (array) $opts;
    $opts['class'] = (array) $opts['class'];
    $opts['class'][] = 'js-loader';
    $opts['on'] = (array) $opts['on'];
    $opts['on']['load'] = $js;
    return self::elt('img', null, $opts);
  }

  private static $voids = array('input', 'button', 'link', 'meta', 'img');

  public static function elt($tag, $content, $attrs=null) {
    if (is_array($tag)) {
      $html = '';
      foreach ($tag as $t) {
        $html .= self::elt($t['tag'], $t['content'], $t['attributes']);
      }
      return $html;
    }
    $attrs = (array) $attrs;
    $is_void = false;
    $tag = strtolower($tag);
    if (in_array($tag, self::$voids)) {
      $is_void = true;
      if ($tag == 'img') {
        if (!empty($content)) {
          $attrs['src'] = $content;
          $content = '';
        }
        if (empty($attrs['src'])) {
          $attrs['src'] = S('req')->isBack() ? SH::dataImage() : PIX_URL.'/empty.gif';
        }
      }
      else if (!empty($content)) {
        if ($tag == 'input' && $attrs['type'] == 'image') {
          $attrs['src'] = $content;
        }
        else {
          $attrs['value'] = $content;
        }
      }
      if ($tag == 'input') {
        $attrs['type'] = $attrs['type'] ?: 'text';
      }
    }
    if ($tag == 'a' && !isset($attrs['href'])) {
      $attrs['href'] = 'javascript:void(0)';
    }
    if (empty($attrs['class'])) {
      unset($attrs['class']);
    }
    if (empty($attrs['style'])) {
      unset($attrs['style']);
    }
    $attrs = AH::asAttributes($attrs);
    if (!empty($attrs)) {
      $attrs = ' '.$attrs;
    }
    $html = '<'.$tag.$attrs;
    if ($is_void === true) {
      $html .= ' />';
    }
    else {
      if ($content === null) {
        $html .= '>';
      }
      else {
        $html .= '>'.$content.'</'.$tag.'>';
      }
    }
    return $html;
  }

  public static function tag($name, $attrs) {
    if (!empty($attrs)) {
      $attrs = ' '.AH::asAttributes($attrs);
    }
    return '<' . $name . $attrs . '>';
  }

  public static function asDataValues($h) {
    $value = $h;
    if (is_array($value)) {
      $value = json_encode($h);
    }
    $value = htmlspecialchars($value);
    return $value;
  }

  public static function link($label, $url, $params=null, $attrs=null) {
    $attrs = (array) $attrs;
    $url = self::url($url, $params);
    $attrs['href'] = $url;
    $label = $label ?: $url;
    return self::elt('a', $label, $attrs);
  }

  public static function url($action='/', $params=null, $opts=null) {
    $params = (array) $params;
    $opts = (array) $opts;
    $locale = $locale ?: self::locale();
    if ($action instanceof ActiveRecord) {
      S('log')->trace('DEPRECATED : first arg can not be an item, you should use u()', __METHOD__);
      $action = $action->asUrl($locale);
    }
    $url = trim($action);
    $chunk = substr($url, 0, 8);
    if (strpos($chunk, '//') === false) {
      $url = $url[0] != '/' ? '/'.$url : $url;
      $prefix  = P('app.url.secure') ? 'https://' : 'http://';
      if (P('app.url.user')) {
        $prefix .= P('app.url.user');
        if (P('app.url.password')) {
          $prefix .= ':' . P('app.url.password');
        }
        $prefix .= '@';
      }
      $prefix .= P('app.domain');
      $prefix .= P('app.url.port') ? ':' . P('app.url.port') : '';
      $url = $prefix . $url;
    }
    if (count($params) >= 1) {
      $sep = strpos($action, '?') === false ? '?' : '&';
      $url .= $sep . http_build_query($params);
    }
    return $url;
  }

  public static function countries($country_id=0, $locale=null) {
    $country_id = $country_id ?: I18N::COUNTRY_FR;
    $locale = $locale ?: self::locale();
    $suffix = $locale === I18N::LOCALE_FR ? '_fr' : '_en';
    $countries = S('dbo')->asHash('SELECT id, name'.$suffix
                                  .' FROM '.T('country')
                                  .' WHERE status=1'
                                  .' ORDER BY name'.$suffix);
    $ret = ''."\n";
    foreach ($countries as $id => $name) {
      $selected = $country_id == $id ? ' selected="selected"' : '';
      $ret .= ' <option value="'.$id.'"'.$selected.'>'.c(strtolower($name)).'</option>'."\n";
    }
    return $ret;
  }

  public static function birthdate($value=null, $opts=array()) {
    /* opts : id, name, class */
    $name = $opts['name'] ? $opts['name'] : 'birth_date';
    $name = ' name="'.$name.'"';
    $id = $opts['id'] ? ' id="'.$opts['id'].'"' : '';
    //$onchange = ' onchange="Kwo.BirthDate.change(this)"';
    $onchange = ' onchange="Kwo.DateManager.birthdate(this)"';
    $h = DH::asHash($value);
    $h = $h ? $h : array();
    $value = '';
    if (!empty($h)) {
      $value = ' value="'.$h['year'].'-'.$h['month'].'-'.$h['day'].'"';
    }
    $html = '<input type="hidden"'.$name.$id.$value.' class="birthdate_input" />';
    $html_day = '<select class="kwo-birthdate birthdate-day"'.$onchange.'>';
    $html_day .= empty($h) ? '<option value="0">--</option>'."\n" : '';
    foreach (range(1, 31) as $i) {
      $selected = $i == $h['day'] ? ' selected="selected"' : '';
      $html_day .= '<option value="'.$i.'"'.$selected.'>'.$i.'</option>';
    }
    $html_day .= '</select>';
    $html_month = '<select class="kwo-birthdate birthdate-month"'.$onchange.'>';
    $html_month .= empty($h) ? ' <option value="0">----</option>'."\n" : '';
    foreach (range(1, 12) as $i) {
      $selected = $i == $h['month'] ? ' selected="selected"' : '';

      $html_month .= '<option value="'.$i.'"'.$selected.'>'.I18N::month($i, self::locale()).'</option>';
    }
    $html_month .= '</select>';
    $html_year = '<select class="kwo-birthdate birthdate-year"'.$onchange.'>';
    $year = date('Y');
    $html_year .= empty($h) ? '<option value="0">----</option>'."\n" : '';
    foreach (range($year - 5, $year - 95) as $i) {
      $selected = $i == $h['year'] ? ' selected="selected"' : '';

      $html_year .= '<option value="'.$i.'"'.$selected.'>'.$i.'</option>';
    }
    $html_year .= '</select>';
    if (self::locale() == I18N::LOCALE_FR) {
      $html .= $html_day.' '."\n".$html_month.' '."\n".$html_year;
    }
    else {
      $html .= $html_year.' '."\n".$html_month.' '."\n".$html_day;
    }
    return "\n".$html;
  }

  public static function debug($var) {
    return var_export($var, true);
  }

}
