<?php

$parameter['id'] = 363;
$parameter['name'] = 'group';
$parameter['has_many'] = array('discussion' => array('dependent' => true),
                               'user' => array('through' => 'core_group_user'));
