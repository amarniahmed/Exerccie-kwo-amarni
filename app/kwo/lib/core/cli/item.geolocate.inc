<?php

$key = 'ABQIAAAAuc7xYYty63Ys-Y2-dLpZVBTMGjX7bTG0pjI'
      .'hnwtfqxmJuYRcDBQbVyEEUw-Fo1CuG-dUcVwSP2sxdw';

$model = $req->model;

if (!Model::exists($model)) return ;

$filters = array('|<>|address' => '',
                 '|<>|latitude' => 0,
                 '|<>|longitude' => 0);
$opts = array('filters' => $filters);



foreach ($users as $user) {
  if (!$user->hasProfile()) continue;
  $profile = $user->getProfile();
  if ($profile && $profile->latitude == 0 && $profile->longitude == 0) {
    $position = explode(',', 
                        file_get_contents("http://maps.google.com/maps/geo?q=".urlencode($profile->address." ".$profile->address_extra." ".$profile->postal_code." ".$profile->city)."&output=csv&key=".$key));
    if (count($position) && $position[2] != 0 && $position[3] != 0) {
      $user->setProfile(array('latitude' => $position[2],
                              'longitude' => $position[3]));
    }
  }
}
