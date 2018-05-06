<?php

class VarnishClient {

  const METHOD_PURGE = 'PURGE';

  protected $servers = null;
  protected $opts = null;


  public function __construct($servers, $opts) {
    $this->servers = (array) $servers;
    $this->opts = (array) $opts;
    if (!isset($this->opts['purge_method'])) {
      $this->opts['purge_method'] = self::METHOD_PURGE;
    }
  }

  public function invalidate($action) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $this->opts['purge_method']);
    curl_setopt($ch, CURLOPT_HEADER, true);
    curl_setopt($ch, CURLOPT_NOBODY, true);

    foreach ($this->servers as $server) {
      $url = $server.$action;
      if (strpos($url, '://') === false) {
        $url = 'http://'.$url;
      }
      curl_setopt($ch, CURLOPT_URL, $url);
      $ret = curl_exec($ch);
      $info = curl_getinfo($ch);
//@TODO : delete debug
/**/$datas = SH::asSet($ret);
/**/echo '> '.$this->opts['purge_method'].' '.$url."\t\t".$datas[0]."\n";
    }
    curl_close($ch);
  }

  public static function getInstance($servers, $opts=null) {
    return new VarnishClient($servers, $opts);
  }

}