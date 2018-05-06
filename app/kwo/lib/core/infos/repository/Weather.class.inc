<?php

/*

http://www.google.com/ig/api?weather=62200,fr
http://toolbar.google.com/buttons/apis/howto_guide.html#updatedropdown

*/

class Weather
{

  public function __construct () {
  }
  
  public function search ($query) {
    if (empty($query)) return array('error'=>"Aucun r&eacute;sultat.",'result'=>array()); 
    try {
      $content = @file_get_contents("http://xoap.weather.com/search/search?where=".urlencode($query));
      if ($content===false) {
        S('log')->trace ('{'.__METHOD__.'} weather.com api failure [search : '.$query.']');
        return false;
      }
      $xml = new SimpleXMLElement ($content);
      $res = array();
      $num = 0;
      if (!empty($xml)) foreach ($xml as $loc) $res[(string) $xml->loc[$num]['id']] = (string) $xml->loc[$num++];
      $results_arr = array();
      if (count($res) > 0) {
        $results_arr['error'] = null;
      } else {
        $results_arr['error'] = "Aucun r&eacute;sultat.";
      }
      $results_arr['result'] = $res;
      
      return json_encode((object) $results_arr);
    } catch (Exception $e) {
    }
    
  }
  
  public function fetch ($query, $unit="") {
    try {
      if (Valid::isAlphanum($query) && strlen($query) == 8) {
        $content = Cache::request("http://xoap.weather.com/weather/local/".urlencode($query)."?cc=*".(($unit=='C')?'&unit=m':''), null, $query."".$unit);
        $xml = new SimpleXMLElement ($content);
        $res = array();
        if (!empty($xml) && !empty($xml->cc->icon)) {
          $res['icon'] = (string) $xml->cc->icon;
          $res['name'] = (string) $xml->loc->dnam;
          $res['temp'] = ((string) $xml->cc->tmp)."&deg;".((string) $xml->head->ut);
        }
      }
      $results_arr = array();
      $results_arr['error'] = null;
      $results_arr['result'] = $res;
      return json_encode((object) $results_arr);
    } catch (Exception $e) {
    }
  }
  
}
