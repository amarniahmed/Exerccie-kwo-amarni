<?php

$parameter['id'] = 100;
$parameter['name'] = 'advert';
$parameter['label'] = 'pub';
$parameter['casts'] = array('spaces' => 'bits');
$parameter['belongs_to'] = 'advertiser';
$parameter['has_many'] = array('space' => array('through' => 'core_advert_space'));