<?php

class HttpClient
{
  const USER_AGENT = 'HttpClient';

  public static function call($url, $params=null, $opts=null) {
    try {
      $opts = (array) $opts;
      $cache_key = null;
      $cache_ttl = $opts['cache_ttl'];
      $hrq = HttpRequest::getInstance($url, $params, $opts);
      if ($cache_ttl && P('cache.disabled') != 1 && !$hrq->isPost()) {
//          (is_array($params) || is_null($params)) && $hrq->getMethod() != 'POST') {
        $cache_key = $opts['cache_key'] ?: $hrq->getKey();
        $content = Cache::get($cache_key);
        if ($content !== false) {
//      S('log')->trace('cache hit ['.$cache_key.']', __METHOD__);
          return $content;
        }
//        return Cache::request($hrq, $opts['cache_ttl']);
      }
      $hrs = $hrq->send();
      if ($opts['debug'] === true) {
        $hrq->debug();
      }
      if (!($hrs instanceof HttpResponse)) {
        if ($opts['debug'] !== true) {
          $hrq->debug();
        }
        throw new Exception('invalid response');
      }
      $content = $hrs->getContent();
      if ($opts['debug'] === true) {
        $hrs->debug();
      }
      if ($opts['decode'] === true) {
        $content = json_decode($content, true);
      }
      if ($cache_ttl && $cache_key && $content &&
          $hrs->getHeader('x-kwo-cache-disable') != 1) {
        Cache::set($cache_key, $content, $cache_ttl);
      }

      return $content;
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      return false;
    }
  }

  public static function json($url, $params=null, $opts=null) {
    $opts = (array) $opts;
    $json = self::call($url, $params, $opts);
    return json_decode($json, true);
  }

  public static function get($url, $params=null, $opts=null) {
    $opts = (array) $opts;
    $opts['method'] = 'get';
    return self::call($url, $params, $opts);
  }

  public static function post($url, $params=null, $opts=null) {
    $opts = (array) $opts;
    $opts['method'] = 'post';
    return self::call($url, $params, $opts);
  }

  public static function getInstance($url=null, $params=null, $opts=null) {
    return new HttpClient($url, $params, $opts);
  }

}





/*  private $request = null;
  private $response = null; */

/*  public function __construct($url, $params, $opts) {
    S('log')->trace('DEPRECATED use HttpClient::call|post|get|json or HttpRequest', __METHOD__);
    if ($url === null) return ;

    $this->url = $url;
    $this->opts = (array) $opts;
    if (!empty($params) && !is_array($params) &&
        (is_string($params) || $params instanceof File)) {
      if (!isset($this->opts['method'])) {
        $this->opts['method'] = 'post';
      }
    }
    $this->params = $params;
    $this->headers = (array) $this->opts['headers'];
  } */

/*  public function send($request) {
    $this->request = $request;
    $this->response = null;
    $debug = $this->request->hasFlag(HttpRequest::FLAG_DEBUG);
    $stream = $this->request->getStream();
    if ($debug === true) {
      $this->request->debug();
    }
    if (empty($stream)) {
      if ($this->request->isInternal() && $debug !== true) {
        $this->request->debug();
      }
      S('log')->error('uri unavailable ['.$this->request->getUri().']', __METHOD__);
      throw new Exception('uri unavailable');
    }
    $this->response = HttpResponse::getInstance($stream);
    if ($debug === true) {
      $this->response->debug();
    }
    if ($this->response->isError()) {
      if ($this->request->isInternal() && $debug !== true) {
        $this->response->debug();
      }
      S('log')->error('response error '.
                      '['.$this->request->getUri().'] '.
                      '['.$this->response->getStatus('line').']', __METHOD__);
      throw new Exception('response error');
    }
    return $this->response;
  } */


  /*

  private $context_options = null;
  private $headers = array();
  private $http_code = null;
  private $is_compressed = null;
  private $metas = null;
  private $opts = null;
  private $params = null;
  private $response_headers = null;
  private $url = null;

  public function setBody($body) { $this->opts['method'] = $this->opts['method'] ?: 'POST'; $this->params = $body; }
  public function setParams($params) { $this->params = $params; }
  public function setHeader($key, $value) { $this->headers[$key] = $value; }
  public function isError() { return $this->http_code > 400; }
  public function getContent() { return $this->fetch(); }
  public function getMetas() { return $this->metas; }
  public function isCompressed() { return $this->is_compressed === true; }
  public function getReturnCode() { return $this->http_code; }

  public function fetch() {
    $stream = $this->prepareStream();

    $response = @stream_get_contents($stream);
    if ($response === false) {
      S('log')->trace('stream_get_contents failure ['.$url.']', __METHOD__);
      return false;
    }
    fclose($stream);

    if ($this->response_headers['content-encoding'] === 'gzip') {
      $this->is_compressed = true;
      $response = SH::gzdecode($response);
      if ($response === false) {
        $parts = parse_url($url);
        file_put_contents('/k/tmp/gzip/'.$parts['host'].'.gz', $datas);
        file_put_contents('/k/tmp/gzip/'.$parts['host'].'.headers', var_export($this->response_headers, true));
        S('log')->error('strange sig ['.$url.']', __METHOD__);
      }
    }

    if ($this->opts['debug'] === true) {
      $this->debug();
    }

    if ($this->opts['as_hash'] === true || $this->opts['decode'] === true) {
      $response = json_decode($response, true);
    }

    return $response;
  }

  public function getStream() {
    if ($this->opts['debug'] === true) {
      $this->debug();
    }
    return $this->prepareStream();
  }

  private function prepareStream() {
    $this->is_compressed = false;
    $this->http_code = null;
    $this->response_headers = array();
    $method = $this->opts['method'] ?: 'GET';
    $method = strtoupper($method);
    $timeout = $this->opts['timeout'] ?: 5;
    $ignore_errors = isset($this->opts['ignore_errors'])
                   ? (bool) $this->opts['ignore_errors']
                   : true;

    $headers = $this->headers;

    $headers['User-Agent'] = $headers['User-Agent'] ?: self::USER_AGENT;

    $url = $this->url;
    if (UH::isInternal($url)) {
      $url = H::url($url);
      $headers['X-Requested-With'] = 'php';
      // TODO : propager les cookies, sessions etc.
    }

    $opts = array('method' => $method,
                  'timeout' => $timeout,
                  'ignore_errors' => $ignore_errors,
                  'max_redirects' => 4,
                  'follow_location' => true);

    if ($method === 'GET' || $method === 'HEAD') {
      if (is_array($this->params) && !empty($this->params)) {
        $url .= '?'.http_build_query($this->params);
      }
    }
    elseif ($method === 'POST' || $method === 'PUT') {
      if (is_array($this->params)) {
        // TODO : faire distinction en fonction de oauth ?
        //        $opts['content'] = $this->opts['content'] ?: self::build_http_query($this->params);
        $opts['content'] = http_build_query($this->params);
        //error_log($opts['content']);
        if (!isset($headers['Content-Type'])) {
          $headers['Content-Type'] = 'application/x-www-form-urlencoded';
        }
      }
      elseif (!empty($this->params)) {
        $opts['content'] = $this->params instanceof File ? $this->params->getContent() : $this->params;
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

    $this->context_options = array('http' => $opts);
    $context = stream_context_create($this->context_options);

    $stream = @fopen($url, 'r', false, $context);
    if ($stream === false) {
      S('log')->trace('url unavailable ['.$url.']', __METHOD__);
      return false;
    }

    $this->metas = @stream_get_meta_data($stream);
    if ($this->metas === false) {
      S('log')->trace('stream_get_meta_data failure ['.$url.']', __METHOD__);
      return false;
    }

    if (is_array($this->metas['wrapper_data'])) {
      $status = $this->metas['wrapper_data'][0];
      if (strtolower(substr($status, 0, 5)) == 'http/') {
        list (, $this->http_code, ) = explode(' ', $status);
      }
      else {
        S('log')->error('header error ['.$status.']', __METHOD__);
      }
      $this->http_code = (int) $this->http_code;
      $n = count($this->metas['wrapper_data']);
      for ($i = 1; $i < $n; $i++) {
        list ($key, $value) = explode(': ', $this->metas['wrapper_data'][$i], 2);
        $this->response_headers[strtolower($key)] = $value;
      }
    }

    return $stream;
  }

  public function debug() {
    S('log')->debug(array('url' => $this->url,
                          'params' => $this->params,
                          'context' => $this->context_options));
    S('log')->debug(array('metas' => $this->metas,
                          'headers' => $this->response_headers));
  }*/
