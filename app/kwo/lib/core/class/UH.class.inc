<?php

class UH
{
  public static function isInternal($url) {
    if (substr($url, 0, 7) != 'http://' && 
        substr($url, 0, 8) != 'https://' && 
        substr($url, 0, 2) != '//') return true;
    // TODO : comparer le domaine avec P('app.domain')
    return false;
  }

  /** http://code.google.com/apis/safebrowsing/developers_guide_v2.html **/
  // 204 = OK : 200 = malware/phishing
  public static function isSafe($url) {
    try {
      $uri = 'https://sb-ssl.google.com/safebrowsing/api/lookup';
      $key = 'ABQIAAAAwvHIwl9-UIfA1-ufUJYWzhRdz-jvlFeYFK3Su4OkcjCv4XuTyg';
      $params = array('client' => 'api',
                      'apikey' => $key,
                      'appver' => '1.5.2',
                      'pver' => '3.0',
                      'url' => $url);
      $res = HttpRequest::getInstance($uri, $params)->send();
      return $res->getStatus() == 204;
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      return true;
    }
  }

}