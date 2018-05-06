<?php

try {
  
  $item = Item::getInstance(MODEL, $req->id);
  
  $key = "ABQIAAAAuc7xYYty63Ys-Y2-dLpZVBRq0Y5SixoKzZOxi6xwInvPgVakQBRavCj1qRQigVHDfHx5TrtjK9EcXw";
  
  $position = explode(',',file_get_contents("http://maps.google.com/maps/geo?q=".urlencode($item->address." ".$item->city." ".$item->postal_code)."&output=csv&key=".$key));
  if (count($position) && $position[2] != 0 && $position[3] != 0) {
    $item->commit(array('latitude'=>$position[2], 'longitude'=>$position[3]));
  }
  
}
catch (Exception $e) { $err->add($e); }
