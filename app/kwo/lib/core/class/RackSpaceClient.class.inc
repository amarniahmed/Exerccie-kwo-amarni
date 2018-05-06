<?php

/*  public static function replaceUrl($url) {
    $tmp = explode('.',$url);
    $tmp[1] = 'ssl';
    $url = implode('.',$tmp);
    return str_replace('http://','https://',$url);
  } */


class RackSpaceClient
{
  const URL_UK = 'https://lon.identity.api.rackspacecloud.com/v1.0';
  const URL_US = 'https://identity.api.rackspacecloud.com/v1.0';

  const IOS_URI = 'x-cdn-ios-uri';
  const SSL_URI = 'x-cdn-ssl-uri';
  const STREAMING_URI = 'x-cdn-streaming-uri';
  const HTTP_URI = 'x-cdn-uri';

  const SEGMENT_SIZE = 104857600; //100Mo
  // const SEGMENT_SIZE = 1048576; //1Mo

  private $auth_headers = null;

  function __construct($account_name, $api_key, $account_url) {
    $this->auth_headers = $this->getAuthHeaders($account_name, $api_key, $account_url);
  }

  private function getAuthHeaders($account_name, $api_key, $account_url) {
    $hreq = HttpRequest::getInstance($account_url);
    $hreq->setHeaders(array('X-Auth-User' => $account_name,
                            'X-Auth-Key'  => $api_key));
    $hreq->setMethod('GET');
    $hresp = $hreq->send();

    if ($hresp->isError()) {
      throw new Exception('Authentication failed: ');//.$hresp->debug());
    }
    $headers = $hresp->getHeaders();

    $this->auth_headers = $headers;
    return $headers;
  }

  private function getKey() {
    return $this->auth_headers['x-auth-token'];
  }

  private function getStorageKey() {
    return $this->auth_headers['x-storage-token'];
  }

  private function getStorageUrl() {
    return $this->auth_headers['x-storage-url'];
  }

  private function getManagementUrl() {
    return $this->auth_headers['x-cdn-management-url'];
  }

  public function createContainer($name, $is_public=true) {
    $url = $this->getStorageUrl().'/'.$name;
    $hreq = HttpRequest::getInstance($url);
    $hreq->setHeaders(array('X-Auth-Token' => $this->getKey()));
    $hreq->setMethod('PUT');

    $hresp = $hreq->send();
    if ($hresp->isError()) {
      throw new Exception('container creation has failed: ');//.$hresp->debug());
    }
    if ($is_public) $this->setPublicContainer($name);
  }

  public function setPublicContainer($name) {
    $url = $this->getManagementUrl().'/'.$name;
    $hreq = HttpRequest::getInstance($url);
    $hreq->setHeaders(array('X-Auth-Token'  => $this->getKey(),
                            'X-CDN-Enabled' => 'True'));
    $hreq->setMethod('PUT');
    $hresp = $hreq->send();

    if ($hresp->isError()) {
      throw new Exception('public CDN has failed: ');//.$hresp->debug());
    }
  }

  public function setPrivateContainer($name) {
    $url = $this->getManagementUrl().'/'.$name;
    $hreq = HttpRequest::getInstance($url);
    $hreq->setHeaders(array('X-Auth-Token'  => $this->getKey(),
                            'X-CDN-Enabled' => 'False'));
    $hreq->setMethod('PUT');
    $hresp = $hreq->send();

    if ($hresp->isError()) {
      throw new Exception('private CDN has failed: ');//.$hresp->debug());
    }
  }

  public function listFiles($container, $path, $opts=array()) {
    $params = array('container' => $container,
                    'prefix'    => $path,
                    'format'    => isset($opts['format']) ? $opts['format'] : 'json');

    if (isset($opts['recursive']) && $opts['recursive']) {
      $params['delimiter'] = '/';
    }
    if (isset($opts['limit'])) {
      $params['limit'] = $opts['limit'];
    }

    $url = $this->getStorageUrl().'/'.$params['container'];
    unset($params['container']);

    $hreq = HttpRequest::getInstance($url);
    $hreq->setParams($params);
    $hreq->setHeaders(array('X-Auth-Token' => $this->getKey()));
    $hreq->setMethod('GET');
    $hresp = $hreq->send();

    if ($hresp->isError()) {
      throw new Exception('file listing has failed: ');//.$hresp->debug());
    }
    return ($params['format'] == 'json') ? $hresp->getJson() : $hresp->getXml();
  }

  public function uploadFile($container, $file, $opts=array()) {
    if (!is_a($file, 'File')) {
      throw new Exception('invalid file object');
    }

    if ($file->getSize() > self::SEGMENT_SIZE) {
      $this->uploadLargelFile($container, $file, $opts);
    }
    else {
      $this->uploadSmallFile($container, $file, $opts);
    }
  }

  private function uploadSmallFile($container, $file, $opts) {
    if (isset($opts['dest_name'])) {
      $dest_name = $container.'/'.$opts['dest_name'];
    }
    else {
      $dest_name = $container.'/'.$file->getBaseName();
    }

    $headers = array('X-Auth-Token' => $this->getKey());
    $url = $this->getStorageUrl().'/'.$dest_name;

    $hreq = HttpRequest::getInstance($url);
    $hreq->setHeaders($headers);
    $hreq->setBody($file->getContent());
    $hreq->setMethod('PUT');
    $hresp = $hreq->send();

    if ($hresp->isError()) {
      S('log')->trace('upload failure ['.$url.']', __METHOD__);
      throw new Exception('file uploading has failed (1)');
    }
  }

  private function uploadLargelFile($container, $file, $opts) {
    if (isset($opts['dest_name'])) {
      $dest_name = $container.'/'.$opts['dest_name'];
    }
    else {
      $dest_name = $container.'/'.$file->getBaseName();
    }

    $headers = array('X-Auth-Token' => $this->getKey(),
                     'X-Object-Meta-PIN' => date('YdmHis'));
    $url = $this->getStorageUrl().'/'.$dest_name;

    $stream = $file->getStream();
    $i=0;
    while (!feof($stream)) {
      $part_filename = $url.'/part'.sprintf('%04d', ++$i);
      $part_content = @fread($stream, self::SEGMENT_SIZE);

      $hreq = HttpRequest::getInstance($part_filename);
      $hreq->setHeaders($headers);
      $hreq->setBody($part_content);
      $hreq->setMethod('PUT');
      $hresp = $hreq->send();
      if ($hresp->isError()) {
        S('log')->trace('upload failure (1) ['.$url.']', __METHOD__);
        throw new Exception('file uploading has failed (2): '.$part_filename);
      }
    }
    fclose($stream);

    $headers['Content-Length'] = '0';
    $headers['X-Object-Manifest'] = $container.'/'.$file->getBaseName().'/part';
    $hreq = HttpRequest::getInstance($url);
    $hreq->setHeaders($headers);
    $hreq->setMethod('PUT');
    $hresp = $hreq->send();

    if ($hresp->isError()) {
    S('log')->trace('upload failure (2) ['.$url.']', __METHOD__);
      throw new Exception('file uploading has failed (2): create manifest');
    }
  }

  public function getFile($container, $filename) {
    if ($this->fileExist($container, $filename)) {
      $url = $this->getStorageUrl().'/'.$container.'/'.$filename;

      $hreq = HttpRequest::getInstance($url);
      $hreq->setHeaders(array('X-Auth-Token' => $this->getKey()));
      $hreq->setMethod('GET');
      $hresp = $hreq->send();

      $file = File::getInstance(TMP_PATH.'/'.basename($filename), 'w+');
      $file->write($hresp->getContent());
      return $file;
    }
    return null;
  }

  public function copyFileTo($container_src, $filename_src, $container_dest, $filename_dest) {
    if ($this->fileExist($container_src, $filename_src)) {
      if ($this->containerExist($container_dest)) {
        $url = $this->getStorageUrl().'/'.$container_dest.'/'.$filename_dest;

        $hreq = HttpRequest::getInstance($url);
        $hreq->setHeaders(array('X-Auth-Token'   => $this->getKey(),
                                'X-Copy-From'    => '/'.$container_src.'/'.$filename_src,
                                'Content-Length' => '0'));
        $hreq->setMethod('PUT');
        $hresp = $hreq->send();
        return true;
      }
      S('log')->debug('destination container does not exist');
      return false;
    }
    return false;
  }

  public function moveFileTo($container_src, $filename_src, $container_dest, $filename_dest) {
    $this->copyFileTo($container_src, $filename_src, $container_dest, $filename_dest);
    $this->deleteFile($container_src, $filename_src);
  }

  public function getPublicUri($container, $type_url) {
    $url = $this->getManagementUrl().'/'.$container;

    $hreq = HttpRequest::getInstance($url);
    $hreq->setHeaders(array('X-Auth-Token' => $this->getKey()));
    $hreq->setMethod('HEAD');
    $hresp = $hreq->send();

    if ($hresp->isError()) {
      throw new Exception('get public urls has failed: ');//.$hresp->debug());
    }

    $headers = $hresp->getHeaders();
    return $headers[$type_url];
  }

  public function deleteFile($container, $filename) {
    if ($this->fileExist($container, $filename)) {
      $url = $this->getStorageUrl().'/'.$container.'/'.$filename;

      $hreq = HttpRequest::getInstance($url);
      $hreq->setHeaders(array('X-Auth-Token' => $this->getKey()));
      $hreq->setMethod('DELETE');
      $hresp = $hreq->send();
      return true;
    }
    return false;
  }

  public function deleteContainer($name) {
    return $this->deleteFile($name, '');
  }

  public function fileExist($container, $filename) {
    $url = $this->getStorageUrl().'/'.$container.'/'.$filename;
    $url = trim($url, '/');

    $hreq = HttpRequest::getInstance($url);
    $hreq->setHeaders(array('X-Auth-Token' => $this->getKey()));
    $hreq->setMethod('HEAD');

    try {
      $hresp = $hreq->send();
    }
    catch (Exception $e) { return false; }

    return !$hresp->isError();
  }

  public function containerExist($name) {
    return $this->fileExist($name, '');
  }

  public static function getInstance($account_name, $api_key, $account_url) {
    return new RackSpaceClient($account_name, $api_key, $account_url);
  }

}


/* Response
X-Auth-Token
X-CDN-Management-Url
X-Server-Management-Url
X-Storage-Token
X-Storage-Url

Format: xml ou json

CDN <container> Response:
X-Cdn-Enabled
X-Cdn-Ios-Uri
X-Cdn-Ssl-Uri
X-Cdn-Streaming-Uri
X-Cdn-Uri
X-Log-Retention
*/