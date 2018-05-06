<?php

class Response
{
  const ENCODING_UTF8  = 1;

  const FORMAT_HTTP    = 1;
  const FORMAT_JSON    = 2;
  const FORMAT_PHP     = 3;
  const FORMAT_TEXT    = 4;
  const FORMAT_XML     = 5;
  const FORMAT_XMLRPC  = 6;
  const FORMAT_JSONRPC = 7;
  const FORMAT_SOAP    = 8;

  public static $formats = array('text'    => self::FORMAT_TEXT,
                                 'jsonrpc' => self::FORMAT_JSONRPC,
                                 'json'    => self::FORMAT_JSON,
                                 'xmlrpc'  => self::FORMAT_XMLRPC,
                                 'php'     => self::FORMAT_PHP,
                                 'xml'     => self::FORMAT_XML,
                                 'soap'    => self::FORMAT_SOAP,
                                 'http'    => self::FORMAT_HTTP);

  private $item = null;

  private $cache_key     = null;
  private $cache_ttl     = null;
  private $content       = null;
  private $ctx           = null;
  private $debug_level   = false;
  private $encoding      = null;
  private $err           = null;
  private $format        = null;
  private $http_headers  = array();
  private $http_status   = 0;
  private $is_buffered   = false;
  private $is_closed     = false;
  private $locale        = 0;
  private $req           = null;
  private $templates     = array();
  private $xml_file      = null;

  public function __construct() {
    $this->open();
    $this->ctx = S('ctx');
    $this->req = S('req');
    $this->err = S('err');

    $req = $this->req;

    $locale = $req->getParameter('res_locale') ?: $req->getLocale();

    $this->setLocale($locale);

    if ($req->isApi()) {
      $this->setHeader('Access-Control-Allow-Origin', '*');
      $this->setHeader('Access-Control-Allow-Methods', 'POST,GET,OPTIONS');
      $this->setHeader('Access-Control-Allow-Headers', 'X-Prototype-Version, X-Requested-With');
      $this->setHeader('Access-Control-Expose-Headers', 'x-json');
      $format = $req->getParameter('api_format');
      if ($format == Request::FORMAT_XMLRPC) {
        $this->setFormat(self::FORMAT_XMLRPC);
      }
      elseif ($format == Request::FORMAT_JSONRPC) {
        $this->setFormat(self::FORMAT_JSONRPC);
      }
      elseif ($format == Request::FORMAT_JSON) {
        $this->setFormat(self::FORMAT_JSONRPC);
      }
      elseif ($format == Request::FORMAT_SOAP) {
        $this->setFormat(self::FORMAT_SOAP);
      }
      elseif ($format == Request::FORMAT_XML) {
        $this->setFormat(self::FORMAT_XML);
      }
      else {
        $this->setFormat(self::FORMAT_JSONRPC);
      }
      return ;
    }

    $this->setFormat(self::FORMAT_TEXT);

    if ($req->isCli()) {
      $this->close();
      return ;
    }

    if ($req->hasSession()) {
      $session = $req->getSession();
      if ($session->hasAttribute('flash')) {
        $this->ctx->setAttribute('_flash', $session->getAttribute('flash'));
        $session->removeAttribute('flash');
        if ($session->isEmpty()) {
          $session->invalidate();
        }
      }
    }

    $this->ctx->setAttribute('_locale', Locale::toCode($this->locale));
    $this->ctx->setAttribute('_locale_id', $this->locale);

    if ($req->isFront() || $req->isAccount()) {
      $this->templates[] = P('front.template');
    }
    elseif ($req->isMiddle()) {
      $this->templates[] = P('middle.template');
    }
    elseif ($req->isBack()) {
      $this->templates[] = 'core:skeleton';
    }

    foreach ($this->templates as $i => $template) {
      if (strpos($template, ':') > 0) continue ;
      $this->templates[$i] = P('app.extension') . ':' . $template;
    }

    if ($req->hasHeader('x-kwo-requested-with')) {
      $this->resetTemplates();
      if ($req->isXhr()) {
        if ($req->isXhr(Request::XHR_UPDATE)) {
          $this->setContentType('txt', 'utf-8');
          $this->setFormat(self::FORMAT_TEXT);
        }
        else {
          $this->setContentType('js');
          $this->setFormat(self::FORMAT_JSON);
        }
      }
      else {
        $this->setContentType('txt');
        $this->setFormat(self::FORMAT_TEXT);
      }
    }

    if ($req->hasAttribute('kof', true)) {
      $this->setFormat(self::format($req->getAttribute('kof')));
    }
    elseif ($req->isHttp() && $req->hasHeader('x-knx-kof')) {
      $this->setFormat(self::format($req->getHeader('x-knx-kof')));
    }

    if (P('front.breadcrumb') === true) {
      $this->breadcrumb = array();
    }

    if ($req->hasParameter('noindex')) {
      $this->addMeta('robots', 'NOINDEX');
    }

  }

  public function getHtmlDocument() {
    return HtmlDocument::getInstance();
  }

  private function open() {
    $this->is_buffered = ob_start();
  }

  public function render() {

    if ($this->isClosed() || !$this->isBuffered()) return ;

    $type = null;
    if ($this->format == self::FORMAT_JSON ||
        $this->format == self::FORMAT_JSONRPC) {
      $type = 'json';
    }
    elseif ($this->format == self::FORMAT_XML ||
            $this->format == self::FORMAT_XMLRPC ||
            $this->format == self::FORMAT_SOAP) {
      $type = 'xml';
    }
    elseif ($this->format == self::FORMAT_PHP) {
      $type = 'txt';
    }
    if ($type != null) {
      $this->setContentType($type);
    }

    $this->content = ob_get_clean();

    switch ($this->format) {
      case self::FORMAT_TEXT:    $this->renderText();    break;
      case self::FORMAT_HTTP:    $this->renderHttp();    break;
      case self::FORMAT_JSON:    $this->renderJson();    break;
      case self::FORMAT_XMLRPC:  $this->renderXmlRpc();  break;
      case self::FORMAT_JSONRPC: $this->renderJsonRpc(); break;
      case self::FORMAT_XML:     $this->renderXml();     break;
      case self::FORMAT_PHP:     $this->renderPhp();     break;
      case self::FORMAT_SOAP:    $this->renderSoap();    break;
      default:
        S('log')->trace('no format', __METHOD__);
        $this->renderText();
    }
  }

  private function renderText() {
    if ($this->xml_file !== null && $this->err->count() < 1) {
      $this->setContent(UiManager::getInstance($this->xml_file)->render());
    }
    $this->content = $this->getContent();

    $this->ctx->setContent($this->content);
    if ($this->err->count() >= 1) {
      $req = $this->req;
      $this->ctx->_error_msg .= join('<br/>', $this->err->getMessages());
      if ($req->isFront() || $req->isAccount() || $req->isMiddle()) {
        $this->setHead('Oops!');

        if ($req->isFront()) {
          $template = P('front.template.error') ?: 'error';
        }
        if ($req->isAccount()) {
          $template = P('account.template.error') ?: $template;
        }
        elseif ($req->isMiddle()) {
          $template = P('middle.template.error') ?: $template;
        }
        $this->useTemplates($template);
      }
      elseif ($req->isBack()) {
        $this->useTemplates('core:error');
      }
    }
    if (!empty($this->templates)) {
      $this->content = Template::render($this->templates, true);
    }
    if (P('app.shortcodes') === true &&
        ($this->req->isFront() || $this->req->isAccount())) {
      $this->content = ShortCode::replace($this->content);
    }
    return $this->commit($this->content);
  }

  private function renderJson() {
    $results = array();
    $error_code = $this->err->getCode();
    if ($error_code != 0) {
      $results['msg'] = $this->err->getMessages();
    }
    foreach ($this->ctx->getPublicAttributes() as $key => $obj) {
      $results[$key] = is_object($obj) ? get_object_vars($obj) : $obj;
    }
    $h = array('error' => $error_code,
               'result' => $results);
    $out = json_encode($h);
    if ($this->req->hasAttribute('callback')) {
      $out = $this->req->callback.'('.$out.')';
      S('log')->trace($out);
    }
    $this->commit($out);
  }

  private function renderXmlRpc() {
    $error_code = $this->err->getCode();
    if ($error_code != 0) {
      $response = xmlrpc_encode(array('faultCode' => $error_code,
                                      'faultString' => join(',', $this->err->getMessages())));
      $response = str_replace(array('<fault>','</fault>'),
                              array("<methodResponse>\n<fault>","</fault>\n</methodResponse>"),
                              $response);
    }
    else {
      $response = xmlrpc_encode($this->ctx->getPublicAttributes());
      $response = str_replace(array('<params>','</params>'),
                              array("<methodResponse>\n<params>","</params>\n</methodResponse>"),
                              $response);
    }
    $this->commit($response);
  }

  private function renderJsonRpc() {
    $h = array('jsonrpc' => '2.0');
    $error_code = $this->err->getCode();
    if ($error_code != 0) {
      $h['error'] = array('code' => $error_code,
                          'message' => $this->err->getMessage());
      $h['id'] = null;
    }
    else {
      $h['result'] = $this->ctx->getPublicAttributes();
      $h['id'] = $this->req->getParameter('api_request_id');
    }
    $response = json_encode($h);
    $this->commit($response);
  }

  private function renderXml() {
    $opts = array('tag' => 'response',
                  'attrs' => array('error' => 0));
    if ($this->err->count() >= 1) {
      $opts['attrs']['error'] = $this->err->getCode();
      $attrs = array('message' => join(',', $this->err->getMessages()));
    }
    else {
      $attrs = $this->ctx->getPublicAttributes();
    }
    //S('log')->debug($attrs);
    $this->commit(AH::asXml($attrs, $opts));
  }

  private function renderPhp() {
    $this->commit(AH::encode($this->ctx->getPublicAttributes()));
  }

  private function renderSoap() {
    $this->commit('');
  }

  private function renderHttp() {
    $this->processHeaders();
  }

  public function setContentType($type, $charset=null) {
    $type = strtolower($type);
    if (in_array($type, array('css', 'rss', 'xml'))) {
      $this->resetTemplates();
    }
    $type = strlen($type) <= 4 ? Mime::$registry[$type]['mime'] : $type;
    if (!empty($charset)) {
      $type .= '; charset='.$charset;
    }
    $this->setHeader('Content-Type', $type);
    return $this;
  }

  public function setEncoding($encoding) {
    $this->encoding = $encoding;
  }

  public function getFormat() {
    return $this->format;
  }

  public function isFormat($format) {
    return $this->format == $format;
  }

  public function setFormat($format) {
    if (!is_numeric($format)) {
      $format = strtolower($format);
      $format = (int) self::$formats[$format];
    }
    else {
      $format = (int) $format;
      if (!in_array($format, self::$formats)) {
        $format = 0;
      }
    }
    if ($format < 1) {
      S('log')->error('invalid response format ['.$format.']', __METHOD__);
      return ;
    }
    $this->format = $format;
  }

  public function setHead($arg=null) {
    $item = null;
    $h = array();
    $doc = $this->getHtmlDocument();

    if ($arg instanceof ActiveRecord) {
      $item = $arg;
      $this->track($item);
      $h = $item->asHeadHash($this->getLocale());
    }
    elseif (is_array($arg)) $h = $arg;
    elseif (is_string($arg)) $h['title'] = $arg;

    $keys = array('feeds', 'icons', 'links', 'metas', 'namespaces', 'urls');
    foreach ($keys as $key) $h[$key] = (array) $h[$key];

    if ($h['latitude'] != 0 || $h['longitude'] != 0) {
      $h['geo.position'] = $h['latitude'].';'.$h['longitude'];
    }

    if (!empty($h['title'])) $doc->setTitle($h['title']);
    if (!empty($h['description'])) $doc->addMeta('description', c($h['description']));
    if (!empty($h['keywords'])) $doc->addMeta('keywords', $h['keywords']);
    if (!empty($h['prefix'])) $doc->setPrefix($h['prefix']);
    foreach ($h['metas'] as $key => $value) $doc->addMeta($key, $value);
    foreach ($h['namespaces'] as $key => $value) $doc->addNameSpace($key, $value);
    foreach ($h['icons'] as $value) $doc->addIcon($value);
    foreach ($h['feeds'] as $value) $doc->addFeed($value);
    foreach ($h['links'] as $attrs) $doc->addLink($attrs);
    foreach ($h['urls'] as $lang => $url) {
      $doc->addLink(array('rel' => 'alternate', 'href' => $url, 'hreflang' => $lang));
    }
    if (!empty($h['url'])) $doc->addLink(array('rel' => 'canonical', 'href' => $h['url']));

    $this->item = $item;
    return $this;
  }

  public function addClass($class_name) {
    $this->getHtmlDocument()->addClass($class_name);
    return $this;
  }

  public function addScript($script) {
    $script = strpos($script, '/') === false
            ? WEB_URL.'/'.P('app.extension').'/scripts/'.$script.'.js'
            : $script;
    $this->getHtmlDocument()->addScript($script);
    return $this;
  }

  public function addFeed($url, $name='feed') {
    $this->getHtmlDocument()->addFeed($url, $name);
    return $this;
  }

  public function addStyle($style) {
    $this->getHtmlDocument()->addStyle($style);
    return $this;
  }

  public function addMeta($key, $value) {
    $this->getHtmlDocument()->addMeta($key, $value);
    return $this;
  }

  public function addNameSpace($key, $value) {
    $this->getHtmlDocument()->addMeta($key, $value);
    return $this;
  }

  public function addVar($key, $value) {
    $this->getHtmlDocument()->addVar($key, $value);
    return $this;
  }

  public function addLink($attrs) {
    $this->getHtmlDocument()->addLink($attrs);
    return $this;
  }

  public function getLocale() {
    return $this->locale;
  }

  public function setLocale($locale) {
    $app = S('app');
    $locale = (int) $locale;
    $locales = $app->getLocales();
    if (!in_array($locale, $locales)) {
      $locale = $app->getLocale();
    }
    $this->locale = Locale::toNum($locale);
    return $this->locale;
  }

  public function setFlash($str) {
    if ($this->req->isCli()) {
      S('log')->error('operation forbidden', __METHOD__);
      return ;
    }
    $this->req->getSession()->setAttribute('flash', $str);
  }

  public function setBreadcrumb(array $h) {
    if ($this->breadcrumb === null) return ;
    $breadcrumb = array();
    foreach ($h as $label => $url) {
      if ($url === null) {
        $breadcrumb = array();
        continue ;
      }
      $breadcrumb[$label] = $url;
    }
    $this->ctx->setAttribute('_breadcrumb', $breadcrumb);
  }

  public function setMenu($name=null) {
    if (is_null($name)) {
      $name = array($this->req->getExtension(),
                    $this->req->getAction());
    }
    elseif (func_num_args() > 1) {
      $name = func_get_args();
    }
    else {
      $name = (array) $name;
    }
    $this->ctx->setAttribute('_menu', $name);
  }

  public function setSpace($name=null) {
    $this->space = $name ?: $this->req->getExtension();
    $this->getHtmlDocument()->addVar('_space', $this->space);
  }

  public function setTitle($title) {
    $title = $title instanceof ActiveRecord ? $title->getTitle() : $title;
    $this->ctx->setAttribute('_title', $title);
  }

  public function setWarning($msg) {
    $this->ctx->_warnings[] = $msg;
  }

  public function getHeader($key) {
    return $this->http_headers[$key];
  }

  public function setHeader($key, $value) {
    if ($this->isClosed()) {
      header($key.': '.$value);
      return ;
    }
    $this->http_headers[$key] = $value;
  }

  public function resetHeader($key) {
    $this->setHeader($key, '');
  }

  public function useTemplate($template=true) {
    $action = $this->req->getAction();
    if ($this->req->isBack()) {
      $this->templates[] = $template === true ? $action : $template;
      return ;
    }
    if ($template !== true) {
      $this->useTemplates($template);
      return ;
    }
    $templates = P('front.templates');
    $template = $templates[$this->req->getExtension()][$action];
    $this->templates = array_merge($this->templates, (array) ($template ?: $action));
  }

  public function useTemplates() {
    $templates = func_num_args() >= 1 && is_array(func_get_arg(0))
               ? func_get_arg(0)
               : func_get_args();
    foreach ($templates as $template) {
      if ($template === null) {
        $this->resetTemplates();
        continue ;
      }
      elseif ($template === true || empty($template)) {
        $action = $this->req->getAction();
        $h = P('front.templates');
        $template = $h[$this->req->getExtension()][$action];
        $template = $template ?: $action;
      }
      $this->templates[] = $template;
    }
    return $this;
  }

  public function resetTemplates() {
    $this->templates = array();
  }

  public function useXml($filename=null) {
    $this->xml_file = $filename ?: $this->req->getAction();
  }

  public function getXmlFile() {
    return $this->xml_file;
  }

  public function close() {
    if ($this->isBuffered()) {
      ob_end_clean();
    }
    $this->is_buffered = false;
    $this->is_closed = true;
  }

  public function isClosed() {
    return $this->is_closed;
  }

  public function isBuffered() {
    return $this->is_buffered;
  }

  public function write($content) {
    if (is_object($content)) {
      if ($content instanceof DOMDocument) {
        $content = $content->saveXML();
      }
      elseif (method_exists($content, 'asHtml')) {
        if ($content->locales > 0) {
          $content = $content->asHtml($this->getLocale());
        }
        else {
          $content = $content->asHtml();
        }
      }
      else if (method_exists($content, 'asText')) {
        if ($content->locales > 0) {
          $content = $content->asText($this->getLocale());
        }
        else {
          $content = $content->asText();
        }
      }
      $chunk = substr($content, 0, 8);
      if (stripos($chunk, '<?xml') !== false &&
          preg_match('/xml|rss/', $this->getHeader('Content-Type')) === 0) {
        $this->setContentType('xml');
      }
    }
    //    if ($this->isClosed()) {
    if ($this->isClosed() || $this->isBuffered()) {
      print($content);
      return ;
    }
    /*    if ($this->is_buffered === true) {
      print($content);
      return ;
      }*/
    $this->appendContent($content);
  }

  public function reset() {
    $this->resetTemplates();
    $this->setContent('');
  }

  private function getContent() {
    return $this->content;
  }

  private function setContent($content) {
    $this->content = $content;
  }

  private function appendContent($content) {
    $this->content .= $content;
    //S('log')->trace($this->content, __METHOD__);
  }

  public function disableCache() {
    $this->cache_key = null;
    $this->cache_ttl = null;
//    $this->cache_hit = false;

/*    $this->resetHeader('X-Kwo-Cache-Key');
    $this->resetHeader('X-Kwo-Cache-Ttl');
    $this->resetHeader('X-Kwo-Cached-By'); */
    $this->setHeader('X-Kwo-Cache-Disable', 1);
  }

  public function enableCache() {
    $this->resetHeader('X-Kwo-Cache-Disable');
  }

  public function isCached($opts=null) {
    $req = $this->req;
    if ($req->isBack()) return false;
    if (P('cache.ttl') < 1) return false;
    if ($req->hasAttribute('dnc')) return false;
    if (P('cache.disabled') == 1 || $this->err->count() >= 1 || $req->isTraced()) {
      return false;
    }

    $item = null;
    if ($opts instanceof ActiveRecord) {
      $item = $opts;
      $ttl = true;
      $opts = array();
    }
    else {
      $opts = (array) $opts;
      $ttl = $opts['ttl'];
      $ttl = is_null($ttl) ? true : $ttl;
      if (intval($ttl) <= 0) return false;
      $item = $opts['item'];
    }

    if (!empty($opts['key'])) {
      $key = $opts['key'];
    }
    else {
      $key = '';
      if ($item) {
        if (is_array($item)) {
          $item = Item::getInstance((int) $item['model_id'],
                                    (int) $item['record_id']);
        }
        $key = $item->getModelId().':'.$item->getId().':'.$item->getMeta(Meta::FIELD_STATE).':';
      }
      $key .= $req->getExtension().'.'.$req->getAction();
      $key .= ':'.$req->getChecksum();
      $key .= ':'.$this->getLocale();
      if ($opts['ugc'] === true) {
        $user_id = (int) $this->ctx->_user['id'];
        if ($user_id >= 1) {
          $key .= ':'.$user_id;
        }
      }
    }

    $content = Cache::get($key);
    $this->cache_key = $key;
    $this->cache_ttl = $ttl;
    //S('log')->debug($content, $key);
    if ($content !== false) {
//      $this->cache_hit = true;
//      d('cache_hit');
/*      if (false) {
        $this->setHeader('X-Kwo-Cache-Key', $this->cache_key);
        $this->setHeader('X-Kwo-Cache-Ttl', $this->cache_ttl);
      }
      else {
        $this->setHeader('X-Kwo-Cached-By', 'kwo');
      } */
      $this->write($content);
      return true;
    }
    else {
//      d('cache_miss');
    }
    return false;
  }

  private function commit($content) {
    if (Platform::hasSingleton('session')) {
      S('session')->sync();
    }
    if (!$this->req->isCli() && !$this->isClosed()) {
      $this->processHeaders();
    }
    if (in_array($this->http_status, array(301, 303))) {
      return ;
    }
    if ($this->req->isRpc()) {
      print($content);
      return ;
    }
    if ($this->getHeader('X-Kwo-Cache-Disable') != 1 && !empty($this->cache_key)) {
      Cache::set($this->cache_key, $content, $this->cache_ttl);
    }
    print($content);
  }

  private function processHeaders() {
    if (headers_sent($file, $line) === true) {
      S('log')->error('headers already sent (file='.$file.', line='.$line.')',
                      __METHOD__);
      return false;
    }
    if ($this->req->isFront() && $this->err->count() >= 1 &&
        !$this->req->isXhr() && !$this->req->isRpc()) {
      $this->http_status = 404;
    }
    if ($this->http_status > 0) {
      $status_line = join(' ',
                          array('HTTP/1.1',
                                $this->http_status,
                                HttpResponse::$statuses[$this->http_status]));
      header($status_line, true, $this->http_status);
    }
    foreach (Cookie::$stack as $cookie_name => $cookie) {
      $cookie->commit();
    }
    foreach ($this->http_headers as $key => $value) {
      header($key.':'.$value);
    }
  }

  public function sendAuthenticationForm($opts=array(), $return_string=false) {
    $ctx = S('ctx');
    if ($this->req->isXhr(Request::XHR_EXEC)) {
      S('err')->add(l('error.auth'), Error::CODE_AUTH_FAILURE);
      return ;
    }
    if (isset($opts['callback'])) {
      $ctx->setAttribute('callback', $opts['callback']);
    }
    $filters = array('|&|flags' => Notice::FLAG_SIGNUP,
                     'app' => array('', S('app')->getName()),
                     '|>|status' => 0);
    $opts = array('filters' => $filters,
                  'order' => array('position'));
    $ctx->setAttribute('_notices', Notice::collect($opts));
    $opts = array('filters' => array('|>|status' => 0),
                  'order' => array('position'),
                  'properties' => array('label'));
    $ctx->setAttribute('_origins', Origin::collect($opts));
    $ctx->setAttribute('_mode', $this->req->mode === 'signup' ? 'signup' : 'login');
    $templates = P('front.templates');
    $template = $templates['social']['signup'] ?: 'signup';
    if ($return_string === false) {
      $this->useTemplates($template);
      $this->setHead(l('login'));
      return ;
    }
    return Template::render($template, true);
  }

  public function sendPartialContent($file, $from, $to) {
    $this->close();
    $offset = (int) $from;
    $to = (int) $to;
    $size = $file->getSize();
    if ($to < 1) {
      $to = ($size - 1);
    }
    $length = ($to - $offset) + 1;
    if ($length > (1048576 * 4)) {
      $length = 1048576 * 4;
    }
    $stream = $file->getStream();
    if (fseek($stream, $offset) === -1) {
      throw new Exception('invalid operation', __METHOD__);
    }
    $chunk = fread($stream, $length);
    $length = strlen($chunk);
    $this->setHeader('Content-Length', $length);
    $this->setHeader('Content-Range', 'bytes '.$offset.'-'.(($offset + $length) - 1).'/'.$size);
    $this->processHeaders();
    print($chunk);
    $this->http_status = 206;
  }

  public function sendData($input, $opts=array()) {
    $this->close();
    $this->processHeaders();

    if ($input instanceof File && empty($opts['filename'])) {
      $opts['filename'] = $input->getBaseName();
    }

    if ((is_object($input) && $input instanceof Iterator) || is_array($input)) {
      $as_hash = false;
      if (is_object($input)) {
        $as_hash = $input instanceof Collection && !$input->isMapped();
        $fields = $input->getFields();
      }
      else {
        $elt = current($input);
        $fields = array_keys($elt);
      }
      $filename = $opts['filename'] ?: (method_exists($input, 'asDataFileName') ? $input->asDataFileName() : 'export');
      $csv = CsvWriter::getInstance(array('filename' => $filename,
                                          'columns' => $fields,
                                          'encoding' => $opts['encoding'],
                                          'separator' => $opts['separator']));
      foreach ($input as $row) {
        $row = $as_hash ? $row->asHash() : $row;
        $csv->append($row);
      }
      return ;
    }
    if (strlen($opts['filename']) > 3) {
      DownloadManager::file($input, $opts);
      return ;
    }
    if (is_resource($input)) {
      while (!feof($input)) {
        print(fread($input, 8192));
      }
      fclose($input);
      return ;
    }
    print($input);
  }

  public function sendMessage($msg) {
    $this->ctx->setAttribute('callback_message', $msg);
  }

  public function sendRedirect($url='/', $params=null, $opts=null) {
    $req = $this->req;
    $opts = (array) $opts;
    if (Platform::hasSingleton('session')) {
      S('session')->sync();
    }
    $url = $url instanceof ActiveRecord ? $url->asUrl() : $url;
    $url = empty($url) ? '/' : $url;
    if ($req->isBack() && strstr($url, '/') === false) {
      $url = '/'.$req->getScope().'/'.$req->GetExtension().'/'.$url;
    }
    if (is_array($params) && !empty($params)) {
      $url .= strpos($url, '?') >= 1 ? '&' : '?';
      $url .= http_build_query($params);
    }
    if ($req->isXhr()) {
      $this->ctx->setAttribute('callback_url', $url);
      return ;
    }
    $this->setFormat(self::FORMAT_HTTP);
    $this->setHeader('Location', $url);
    $this->http_status = $opts['permanent'] === true ? 301 : 303;
  }

  public function sendOk() {
    $this->setFormat(self::FORMAT_HTTP);
    $this->http_status = 204;
  }

  public function sendHttpNotModified($etag) {
    $this->setFormat(self::FORMAT_HTTP);
    $this->resetHeader('Expires');
    $this->resetHeader('Cache-Control');
    $this->setHeader('Etag', $etag);
    $this->http_status = 304;
  }

  public function cache($opts=null) {
    $opts = (array) $opts;
    $ttl = $opts['ttl'] > 1 ? $opts['ttl'] : 300;
    $etag = (time() + $ttl).'-'.intval($this->ctx->_user['id']);
    $this->setHeader('Etag', $etag);
    $this->setHeader('Cache-Control', 'max-age='.$ttl);
    $this->setHeader('Expires', gmdate('r', time() + $ttl));
  }

  public function setExpireTime($ttl=3600) {
    $this->resetHeader('Pragma');
    $this->setHeader('Cache-Control', 'max-age='.$ttl.', must-revalidate');
    $this->setHeader('Expires', gmdate('D, d M Y H:i:s', (time() + $ttl)).' GMT');
  }

  // on trace l'affichage de la réponse : compatible varnish
  public function track($action=true, $category=null, $value=null) {
    $doc = $this->getHtmlDocument();
    if (is_array($action) || $action instanceof ActiveRecord) {
      if ($action instanceof ActiveRecord) {
        $action = array($action->getModelId(), $action->getId());
      }
      $category = $category ?: Interaction::TYPE_DISPLAY;
      $doc->addInteraction(array($action[0], $action[1], $category, (int) $value));
    }
    else {
      if ($category === null && $this->item instanceof ActiveRecord) {
        $category = Model::name($this->item);
      }
      list ($action, $category) = Tracker::toEvent($action, $category);
      $doc->addEvent(array($action, $category));
    }
  }

  // TODO : à supprimer
/*  public function getHead() { return $this->head; } */

  public function debug() { $this->debug_level = true; }
  public function trace() { S('log')->trace($this->http_status, __METHOD__); }

  public function __destruct() {
    if ($this->debug_level === true) {
      S('log')->debug(apache_response_headers(), __METHOD__);
      if ($this->http_status !== null) {
        S('log')->trace('http status code : '.$this->http_status,
                        __METHOD__);
      }
    }
  }

  public static function format($format) {
    if (!is_numeric($format)) {
      $formats = self::$formats;
      $format = $formats[$format];
    }
    if (!Request::$formats[$format]) {
      $format = self::FORMAT_TEXT;
    }
    return $format;
  }

  public static function getInstance() {
    if (Platform::hasSingleton('res')) return ;
    Platform::addSingleton('res', new Response());
  }

}
