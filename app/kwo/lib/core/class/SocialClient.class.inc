<?php

/* https://gist.github.com/jonathanmoore/2640302 */
/* https://github.com/bradvin/social-share-urls */

class SocialClient
{
  const PROVIDER_FACEBOOK = 1;
  const PROVIDER_TWITTER = 2;
  const PROVIDER_GOOGLE = 3;
  const PROVIDER_LINKEDIN = 4;
  const PROVIDER_PINTEREST = 5;

  public static $providers = array(self::PROVIDER_FACEBOOK => 'facebook',
                                   self::PROVIDER_TWITTER => 'twitter',
                                   self::PROVIDER_GOOGLE => 'google',
                                   self::PROVIDER_LINKEDIN => 'linkedin',
                                   self::PROVIDER_PINTEREST => 'pinterest');

  public static function provider2id($name) {
    $providers = array_flip(self::$providers);
    return (int) $providers[$name];
  }

  public function getShareUrl($h, $provider='facebook') {

  }

  public function getUrlShareCount($url, $provider='facebook') {
    if (strpos($url, '//') === false) $url = 'http://' . $url;
    if (in_array($provider, array('googleplus', 'google+', 'google'))) {
      return $this->getGoogleShareCount($url);
    }
    if ($provider == 'twitter') return $this->getTwitterShareCount($url);
    if ($provider == 'pinterest') return $this->getPinterestShareCount($url);
    if ($provider == 'linkedin') return $this->getLinkedInShareCount($url);
    if ($provider == 'stumbleupon') return $this->getStumbleUponShareCount($url);
    return $this->getFaceBookShareCount($url);
  }

  private function getFaceBookShareCount($url) {
    $endpoint = 'http://graph.facebook.com/?ids=' . $url;
    $content = @file_get_contents($endpoint);
    if (empty($content)) return 0;
    $h = json_decode($content, true);
    $n = $h[$url]['likes'] + $h[$url]['shares'];
    return (int) $n;
  }

  private function getTwitterShareCount($url) {
    $endpoint = 'http://urls.api.twitter.com/1/urls/count.json?url='.urlencode($url);
    $content = @file_get_contents($endpoint);
    if (empty($content)) return 0;
    $h = json_decode($content, true);
    return (int) $h['count'];
  }

  private function getPinterestShareCount($url) {
    $endpoint = 'http://api.pinterest.com/v1/urls/count.json?callback=&url='.urlencode($url);
    $content = @file_get_contents($endpoint);
    if (empty($content)) return 0;
    $h = json_decode($content, true);
    return (int) $h['count'];
  }

  private function getLinkedInShareCount($url) {
    $endpoint = 'http://www.linkedin.com/countserv/count/share?format=json&url='.urlencode($url);
    $content = @file_get_contents($endpoint);
    if (empty($content)) return 0;
    $h = json_decode($content, true);
    return (int) $h['count'];
  }

  private function getStumbleUponShareCount($url) {
    $endpoint = 'http://www.stumbleupon.com/services/1.01/badge.getinfo?url='.urlencode($url);
    $content = @file_get_contents($endpoint);
    if (empty($content)) return 0;
    $h = json_decode($content, true);
    return (int) $h['result']['views'];
  }

  private function getGoogleShareCount($url) {
    $endpoint = 'https://clients6.google.com/rpc';
    $params = '[{"method":"pos.plusones.get","id":"p","params":{"nolog":true,"id":"'.$url.'","source":"widget","userId":"@viewer","groupId":"@self"},"jsonrpc":"2.0","key":"p","apiVersion":"v1"}]';
    $opts = array('headers' => array('Content-type' => 'application/json'));
    $content = HttpClient::post($endpoint, $params, $opts);
    if (empty($content)) return 0;
    $h = json_decode($content, true);
    return (int) $h[0]['result']['metadata']['globalCounts']['count'];
  }

  public static function getInstance() {
    return new SocialClient();
  }
}