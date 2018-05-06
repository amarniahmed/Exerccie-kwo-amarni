<?php

/***
$opts : ignore_erros, key, ttl, timeout
***/

class HttpRequest
{
  private $body = null;
  private $flags = null;
  private $headers = array();
  private $method = null;
  private $opts = array();
  private $params = null;
  private $uri = null;
  private $url = null;

  const FLAG_INTERNAL = 1;
  const FLAG_DEBUG = 2;

  const SOCKET_TIMEOUT = 5; // default 60

  public function __construct($url=null, $params=null, $opts=null) {
    $opts = (array) $opts;
    $this->url = $url;
    if (!is_null($params)) {
      if (is_array($params)) {
        $this->params = $params;
      }
      else {
        $this->body = $params;
      }
    }
    $attrs = array('body', 'headers', 'method', 'params');
    foreach ($opts as $key => $value) {
      if ($key === 'params' && $this->params) {
        S('log')->trace('warning, params already set', __METHOD__);
        continue ;
      }
      if (in_array($key, $attrs)) {
        if ($key === 'method') {
          $value = strtoupper($value);
        }
        $this->{$key} = $value;
      }
      else {
        $this->opts[$key] = $value;
      }
    }

    $this->opts['timeout'] = $this->opts['timeout'] ?: self::SOCKET_TIMEOUT;
    $this->opts['ignore_errors'] = isset($this->opts['ignore_errors'])
                                 ? (bool) $this->opts['ignore_errors']
                                 : true;

    $headers = (array) $this->headers;

    $this->headers['User-Agent'] = $this->headers['User-Agent'] ?: HttpClient::USER_AGENT;

    if (UH::isInternal($url)) {
      $this->setFlag(self::FLAG_INTERNAL);
      $this->setInternalHeaders();
    }

    if ($this->opts['debug'] === true) {
      $this->setFlag(self::FLAG_DEBUG);
    }

  }

  public function getOpts() {
    return $this->opts;
  }

  public function getParams() {
    return $this->params;
  }

  public function setParams($params) {
    $this->params = $params;
    $this->body = null;
    return $this;
  }

  public function setUrl($url) {
    $this->url = $url;
    return $this;
  }

  public function getUrl() {
    return $this->url;
  }

  public function getUri() {
    return $this->uri;
  }

  public function setHeaders($headers) {
    foreach ($headers as $key => $value) {
      $this->setHeader($key, $value);
    }
    return $this;
  }

  public function setHeader($name, $value) {
    $this->headers[$name] = $value;
    return $this;
  }

  public function setBody($body) {
    $this->method = 'POST';
    $this->body = $body;
    $this->params = null;
    return $this;
  }

  public function setMethod($method) {
    $this->method = strtoupper($method);
    return $this;
  }

  public function getMethod() {
    if ($this->method) return $this->method;
    $method = 'GET';
    if ($this->body || ($this->params && !is_array($this->params))) {
      $method = 'POST';
    }
    return $method;
  }

  public function isPost() {
    return $this->getMethod() === 'POST';
  }

  public function isInternal() {
    return $this->hasFlag(self::FLAG_INTERNAL);
  }

  public function setFlag($flag) {
    $this->flags = $this->flags | $flag;
  }

  public function hasFlag($flag) {
    return (bool) ($this->flags & $flag);
  }

  private function getContext() {

    $method = $this->method = $this->getMethod();

    $url = $this->url;

    $opts = array('method' => $method,
                  'ignore_errors' => $this->opts['ignore_errors'],
                  'max_redirects' => $this->opts['max_redirects'] ?: 4,
                  'follow_location' => $this->opts['follow_location'] ?: true);

    if (isset($this->opts['timeout'])) {
      $opts['timeout'] = $this->opts['timeout'];
    }

    $headers = $this->headers;

    if ($method === 'POST' || $method === 'PUT') {
      if (is_array($this->params)) {
        $opts['content'] = http_build_query($this->params);
        if (!isset($headers['Content-Type'])) {
          $headers['Content-Type'] = 'application/x-www-form-urlencoded';
        }
      }
      elseif ($this->body || $this->params) {
        if ($this->body) {
          $opts['content'] = $this->body instanceof File ? $this->body->getContent() : $this->body;

        }
        elseif (!empty($this->params)) {
          $opts['content'] = $this->params instanceof File ? $this->params->getContent() : $this->params;

        }
        if (!isset($headers['Content-Type'])) {
          if ($opts['content'][0] === '{') {
            $headers['Content-Type'] = 'application/json';
          }
          elseif ($opts['content'][0] === '<') {
            $headers['Content-Type'] = 'application/xml';
          }
        }
      }
    }

    if (!isset($headers['Accept-Encoding']) && $method === 'GET') {
      $headers['Accept-Encoding'] = 'gzip, deflate';
    }

    foreach ($headers as $key => &$value) {
      $value = $key . ': ' . $value;
    }
    $opts['header'] = join("\r\n", $headers)."\r\n";

    $opts = array('http' => $opts);
//    S('log')->debug($opts);
    return stream_context_create($opts);
  }

  public function getStream() {
    $url = $this->url;
    if (in_array($this->getMethod(), array('GET', 'HEAD')) &&
        is_array($this->params) &&
        !empty($this->params)) {
      $url .= strpos($url, '?') > 1 ? '&' : '?';
      $url .= http_build_query($this->params);
    }
    $this->uri = H::url($url);
    $stream = @fopen($this->uri, 'r', false, $this->getContext());
    return $stream;
  }

  public function debug() {
    S('log')->debug(array('method' => $this->getMethod(),
                          'uri' => $this->uri,
                          'url' => $this->url,
                          'params' => $this->params,
                          'body' => !empty($this->body),
                          'headers' => $this->headers));
  }

  public function getKey() {
    if (!empty($opts['key'])) return $opts['key'];

    $uri = $this->url;
    if (!empty($this->params)) {
      $uri .= '?' . http_build_query($this->params);
    }

    $crc = sprintf('%u', crc32($uri));
    if (strpos($this->url, '//') > 3) {
      $key  = $opts['token'] ?: S('req')->getAction();
      $key .= ':'.$crc;
      return $key;
    }

    $key = '';
    if ($opts['item'] instanceof ActiveRecord) {
      $item = $opts['item'];
      $key = $item->getModelId().':'.$item->getId().':'.$item->getMeta(Meta::FIELD_STATE).':';
    }
    if (empty($opts['token'])) {
      $tmp = trim($this->url, '/');
      $tmp = str_replace('/', '.', $tmp);
      $key .= $tmp;
    }
    else {
      $key .= $opts['token'];
    }
    $key .= ':'.$crc;
    $key .= ':'.S('res')->getLocale();
    if ($opts['ugc'] === true && S('ctx')->_user['id'] >= 1) {
      $key .= ':'.intval(S('ctx')->_user['id']);
    }
    return $key;
  }

  public function send() {
    $response = null;
    $debug = $this->hasFlag(HttpRequest::FLAG_DEBUG);
    $stream = $this->getStream();
    if ($debug === true) {
      $this->debug();
    }
    if (empty($stream)) {
      if ($this->isInternal() && $debug !== true) {
        $this->debug();
      }
      S('log')->error('uri unavailable ['.$this->getUri().']', __METHOD__);
      throw new Exception('uri unavailable');
    }
    $response = HttpResponse::getInstance($stream);
    if ($debug === true) {
      $response->debug();
    }
/*    if ($response->isError()) {
      if ($this->isInternal() && $debug !== true) {
        $response->debug();
      }
      S('log')->error('response error '.
                      '['.$this->getUri().'] '.
                      '['.$response->getStatus('line').']', __METHOD__);
      throw new Exception('response error');
    }*/
//    fclose($stream);
    return $response;
  }

  public function setInternalHeaders() {
    $req = S('req');
    if (!$req->isCli()) {
      $this->headers['Accept'] = $_SERVER['HTTP_ACCEPT'];
      $this->headers['Accept-Language'] = $_SERVER['HTTP_ACCEPT_LANGUAGE'];
      $this->headers['X-Knx-Address-From'] = $req->getClient('address');
    }
    $this->headers['X-Knx-Action-From'] = $req->getAction();
    $this->headers['X-Knx-Scope-From'] = $req->getScope();
    $this->headers['X-Knx-Extension-From'] = $req->getExtension();
    $this->headers['X-Knx-Res-Locale'] = S('res')->getLocale();
    $this->headers['X-Kwo-Requested-With'] = 'php';

    if (P('app.cluster.node.id') > 0) {
      $this->headers['X-Cluster-Node-Id'] = P('app.cluster.node.id');
    }

    if (!$req->isCli()) {
      $cookies = array($req->getCookie(Cookie::NAME)->asString(true),
                       $req->getCookie(Tracker::COOKIE_NAME)->asString(true),
                       'SERVERID='.$_COOKIE['SERVERID'],
                       $req->getParameter('session_id'));
      $this->headers['Cookie'] = join('; ', $cookies);
    }

    $authorization = $req->getHeader('authorization');
    if (!empty($authorization)) {
      $this->headers['Authorization'] = $authorization;
    }
//    S('log')->debug($this->headers);

  }

  public static function getInstance($url=null, $params=null, $opts=null) {
    $opts = (array) $opts;
    return new HttpRequest($url, $params, $opts);
  }

//    $headers['Content-Type'] = 'application/x-www-form-urlencoded; charset=UTF-8';
//    $body = http_build_query(array('toto' => 'titi', 'ssss' => 'xséxs'));

  /*$headers = array('Content-Type' => 'text/plain; charset=UTF-8');
  $parts[] = array('headers' => $headers,
                   'content' => 'coucou');
  $parts[] = array('headers' => $headers,
                   'content' => 'hello'); */
  //https://developers.google.com/youtube/2.0/developers_guide_protocol_direct_uploading?hl=fr#Sending_a_Direct_Upload_API_Request
  public static function postParts($url, $parts, $opts=null) {

    function myfwrite($stream, $body) {
      S('log')->trace($body);
      fwrite($stream, $body);
    }

    $opts = (array) $opts;
    $boundary = SH::boundary();
    $eoh = "\r\n"; $eol = PHP_EOL;
    if ($url[0] === '/') {
      $url = H::url($url);
    }
    $uri = $url;
    $url = parse_url($url);

    $body = '';
    foreach ($parts as $part) {
      $body .= '--'.$boundary . $eol;
      $part['headers'] = (array) $part['headers'];
      foreach ($part['headers'] as $key => $value) {
          $body .= $key . ': ' . $value . $eol;
      }
      $body .= $eol;
      $body .= $part['content'] . $eol;
    }
    $body .= '--' . $boundary . '--';
    $content_length = strlen($body); // ce calcul est indispensable

    $headers = (array) $opts['headers'];
    $headers['Host'] = $url['host'];
    $headers['Content-Type'] = 'multipart/related; boundary="'.$boundary.'"';
    $headers['Connection'] = 'close';
    $headers['Content-Length'] = $content_length;
    foreach ($headers as $key => &$value) {
      $value = $key . ': ' . $value;
    }

    $head = 'POST '.$url['path'].' HTTP/1.1' . $eoh;
    $head .= join($eoh, $headers) . $eoh;
    $head .= $eoh;

    $url['port'] = $url['port'] ?: '80';
    $stream = fsockopen($url['host'], (int) $url['port'], $errno, $errstr, 5);
    if (!$stream) {
      S('log')->error($uri.': '.$errstr.' ('.$errno.')', __METHOD__);
      return null;
    }

    myfwrite($stream, $head);

    myfwrite($stream, $body);

    $content = stream_get_contents($stream);

//    S('log')->trace($content, __METHOD__);
    fclose($stream);
  }

}