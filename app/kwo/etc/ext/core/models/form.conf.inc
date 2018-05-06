<?php

$parameter['id'] = 426;
$parameter['name'] = 'form';
$parameter['label'] = 'formulaire';
$parameter['has_many'] = array('submission' => array('dependent' => true),
                               'field' => array('dependent' => true));
$parameter['casts'] = array('fields' => 'hash');
