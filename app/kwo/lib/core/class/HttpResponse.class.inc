<?php

class HttpResponse
{
  private $content = null;
  private $flags = 0;
  private $headers = null;
  private $metas = null;
  private $status = null;
  private $stream = null;

  const FLAG_COMPRESSED = 1;

  public function __construct($stream) {
    $this->stream = $stream;
    $this->metas = @stream_get_meta_data($stream);
    if ($this->metas === false) {
      throw new Exception('stream_get_meta_data failure');
    }
    if (is_array($this->metas['wrapper_data'])) {
      $status = strtolower($this->metas['wrapper_data'][0]);
      if (substr($status, 0, 5) != 'http/') {
        S('log')->error('header error ['.$status.']', __METHOD__);
        return false;
      }
      $parts = explode(' ', $status, 3);
      $this->status['line'] = $status;
      $this->status['version'] = $parts[0];
      $this->status['code'] = (int) $parts[1];
      $this->status['reason'] = $parts[2];
      $n = count($this->metas['wrapper_data']);
      for ($i = 1; $i < $n; $i++) {
        list ($key, $value) = explode(': ', $this->metas['wrapper_data'][$i], 2);
        $this->headers[strtolower($key)] = $value;
      }
    }
  }

  public function getContent() {
    if (!is_null($this->content)) return $this->content;
    $this->content = @stream_get_contents($this->stream);
    if ($this->headers['content-encoding'] === 'gzip') {
      $this->flags = $this->flags | self::FLAG_COMPRESSED;
      $this->content = SH::gzdecode($this->content);
    }
    $this->content = trim($this->content);
    return $this->content;
  }

  public function debug() {
    S('log')->debug(array('status' => $this->getStatus('line'),
                          'metas' => $this->metas,
                          'headers' => $this->headers));
  }

  public function getJson() {
    $content = $this->getContent();
    $json = json_decode($content, true);
    if ($json === null) {
      switch (json_last_error()) {
        case JSON_ERROR_DEPTH: $msg = 'max stack depth exceeded'; break;
        case JSON_ERROR_STATE_MISMATCH: $msg = 'the modes mismatch'; break;
        case JSON_ERROR_CTRL_CHAR: $msg = 'unexpected ctrl character'; break;
        case JSON_ERROR_SYNTAX: $msg = 'syntax error, malformed JSON'; break;
        case JSON_ERROR_UTF8: $msg = 'malformed UTF-8 characters'; break;
        default: $msg = 'unknown error'; break;
      }
      throw new Exception('json error ['.$msg.']');
    }
    return $json;
  }

  public function getXml() {
    $content = $this->getContent();
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    $doc->load($content);
    return $doc;
  }

  public function getStream() { return $this->stream; }
  public function getMetas() { return $this->metas; }
  public function getHeaders() { return $this->headers; }

  public function getHeader($header) {
    return $this->headers[$header];
  }

  public function getStatus($key='code') {
    return $this->status[$key];
  }

  public function isError() {
    return $this->getStatus() > 400;
  }

  public function isCompressed() {
    return $this->flags & self::FLAG_COMPRESSED;
  }

  public static function getInstance($stream) {
    return new HttpResponse($stream);
  }
  
  /* http://en.wikipedia.org/wiki/List_of_HTTP_status_codes */
  public static $statuses = array(100 => 'Continue',
                                  101 => 'Switching Protocols',
                                  200 => 'OK',
                                  201 => 'Created',
                                  202 => 'Accepted',
                                  203 => 'Non-Authoritative Information',
                                  204 => 'No Content',
                                  205 => 'Reset Content',
                                  206 => 'Partial Content',
                                  300 => 'Multiple Choices',
                                  301 => 'Moved Permanently',
                                  302 => 'Found',
                                  303 => 'See Other',
                                  304 => 'Not Modified',
                                  305 => 'Use Proxy',
                                  307 => 'Temporary Redirect',
                                  400 => 'Bad Request',
                                  401 => 'Unauthorized',
                                  402 => 'Payment Required',
                                  403 => 'Forbidden',
                                  404 => 'Not Found',
                                  405 => 'Method Not Allowed',
                                  406 => 'Not Acceptable',
                                  407 => 'Proxy Authentication Required',
                                  408 => 'Request Time-out',
                                  409 => 'Conflict',
                                  410 => 'Gone',
                                  411 => 'Length Required',
                                  412 => 'Precondition Failed',
                                  413 => 'Request Entity Too Large',
                                  414 => 'Request-URI Too Large',
                                  415 => 'Unsupported Media Type',
                                  416 => 'Requested range not satisfiable',
                                  417 => 'Expectation Failed',
                                  500 => 'Internal Server Error',
                                  501 => 'Not Implemented',
                                  502 => 'Bad Gateway',
                                  503 => 'Service Unavailable',
                                  504 => 'Gateway Time-out');

}

/***
1xx (Informational): Request received, continuing process.
2xx (Successful): The action was successfully received, understood, and accepted.
3xx (Redirection): Further action needs to be taken in order to complete the request.
4xx (Client Error): The request contains bad syntax or cannot be fulfilled.
5xx (Server Error): The server failed to fulfil an apparently valid request.
***/