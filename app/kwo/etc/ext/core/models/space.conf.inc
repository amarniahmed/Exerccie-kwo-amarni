<?php

$parameter['id'] = 103;
$parameter['name'] = 'space';
$parameter['label'] = 'emplacement';
$parameter['has_many'] = array('advert' => array('through' => 'core_advert_space'));