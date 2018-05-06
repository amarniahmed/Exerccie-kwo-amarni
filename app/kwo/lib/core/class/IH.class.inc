<?php

class IH
{
  const PROTOCOL_HTTP = 'HTTP';
  
  public static function ping($ip, $port, $timeout=1) {
    $fp = @fsockopen($ip, $port, $errno, $errstr, $timeout);
    if ($fp === false) return false;
    fclose($fp);
    return true;
  }
  
  public static function ip2geo($ip) {
    //http://www.andygup.net/?p=600
    //http://hg.mozilla.org/releases/mozilla-beta/file/28e55051da13/dom/system/NetworkGeolocationProvider.js
    
  }

  public static function isPrivateAddress($address) {
    if (strpos($address, '.') === false) return true;
    return $address === '127.0.0.1' ||
           preg_match('/^(10|172\.(1[6-9]|2[0-9]|30|31)|192\.168)\./', $address) >= 1;
  }

  public static function ip2country($ip, $return_code=false) {
    $ret = (int) S('dbo')->fetchValue('SELECT '.($return_code === true ? 'code' : 'country_id')
                                      .' FROM '.Country::TABLE_IP
                                      .' WHERE from_ip <= inet_aton(:ip:) AND to_ip >= inet_aton(:ip:)',
                                      array(':ip:' => $ip));
    return $ret < 1 ? Country::FR : $ret;
  }

  public static function dnsblquery($ip) {
    $ip_reverse = join('.', array_reverse(explode('.', $ip)));
    foreach (array('zen.spamhaus.org', 'list.dsbl.org') as $dns) {
      if (checkdnsrr($ip_reverse.'.'.$dns, 'A')) return $dns;
    }
    return false;
  }

  public static function isPortOpened($ip, $port, $timeout=3) {
    $fp = @fsockopen($ip, $port, $errno, $errstr, $timeout);
    if (!$fp) return false;
    fclose($fp);
    return true;
  }

  public static function ip2num($ip) {
    return sprintf('%u', ip2long($ip));
  }

  public static function num2ip($num) {
    return long2ip($num);
  }

  public static function ip2host($ip) {
    return gethostbyaddr($ip);
  }

/**
http://www.ripe.net/data-tools/developer-documentation/ripe-database-rest-api/ripe-database-rest-api-documentation/#the-query-services

http://apps.db.ripe.net/whois/lookup/ripe/inetnum/217.174.198.106.xml
http://apps.db.ripe.net/whois/lookup/ripe/inetnum/217.174.198.106
http://apps.db.ripe.net/whois/lookup/ripe/inetnum/217.174.198.106.json

https://apps.db.ripe.net/whois/search.xml?query-string=87.111.249.244&source=ripe

**/
  public static function ip2attrs($ip) {
    $url = 'https://apps.db.ripe.net/whois/search.xml?query-string='.$ip.'&source=ripe';
    $xml = (string) file_get_contents($url);
    if (strpos($xml, 'inetnum') === false) return array();
    $doc = new DOMDocument();
    $doc->formatOutput = true;
    @$doc->loadXML($xml);
    $objects = $doc->getElementsByTagName('object');
    $h = array();
    foreach ($objects as $object) {
      $type = $object->getAttribute('type');
      $h[$type] = array();
      $attrs = $object->getElementsByTagName('attribute');
      foreach ($attrs as $attr) {
        $value = $attr->getAttribute('value');
        if (isset($h[$type][$attr->getAttribute('name')])) {
          $h[$type][$attr->getAttribute('name')] .= "\n" . $value;
        }
        else {
          $link = $attr->getElementsByTagName('link');
          if ($link->length == 1) {
            $link = $link->item(0);
            $value = '<a href="'.$link->getAttribute('xlink:href').'" rel="noreferrer" target="_blank">'.$value.'</a>';
          }
          $h[$type][$attr->getAttribute('name')] = $value;
        }
      }
    }
    return $h;
  }
  
}


/*

function ping($host) {
    $package = "\x08\x00\x19\x2f\x00\x00\x00\x00\x70\x69\x6e\x67";

    // create the socket, the last '1' denotes ICMP  
    $socket = socket_create(AF_INET, SOCK_RAW, 1);
   
    // set socket receive timeout to 1 second 
socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, array("sec" => 1, "usec" => 0));

// connect to socket 
socket_connect($socket, $host, null);

// record start time 
list($start_usec, $start_sec) = explode(" ", microtime());
$start_time = ((float) $start_usec + (float) $start_sec);

    socket_send($socket, $package, strlen($package), 0);

    if(@socket_read($socket, 255)) {
        list($end_usec, $end_sec) = explode(" ", microtime());
        $end_time = ((float) $end_usec + (float) $end_sec);
   
        $total_time = $end_time - $start_time;
       
        return $total_time;
    } else {
        return false;
    }
   
    socket_close($socket);
}

*/