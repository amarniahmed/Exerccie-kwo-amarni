<?php

$parameter['id'] = 812;
$parameter['name'] = 'execution';
$parameter['belongs_to'] = array('workflow');
$parameter['has_many'] = array('process' => array('dependent' => true));
$parameter['casts'] = array('inputs' => 'hash');