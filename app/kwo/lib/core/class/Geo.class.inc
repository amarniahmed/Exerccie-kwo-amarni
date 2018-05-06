<?php

class Geo {

  /* http://derickrethans.nl/spatial-indexes-calculating-distance.html */

  public function distance($latA, $lonA, $latB, $lonB) {
    // convert from degrees to radians
    $latA = deg2rad($latA); $lonA = deg2rad($lonA);
    $latB = deg2rad($latB); $lonB = deg2rad($lonB);
    
    // calculate absolute difference for latitude and longitude
    $dLat = ($latA - $latB);
    $dLon = ($lonA - $lonB);
    
    // do trigonometry magic
    $d = sin($dLat / 2) * sin($dLat / 2) +
         cos($latA) * cos($latB) * sin($dLon / 2) *sin($dLon / 2);
    $d = 2 * asin(sqrt($d));
    return $d * 6371;
  }
  
}