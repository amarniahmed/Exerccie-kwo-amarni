<?php

$parameter['id'] = 432;
$parameter['name'] = 'forum';
$parameter['belongs_to'] = array('column');
$parameter['has_many'] = array('topic' => array('dependent' => true));
