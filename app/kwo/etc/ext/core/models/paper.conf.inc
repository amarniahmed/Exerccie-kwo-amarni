<?php

$parameter['id'] = 401;
$parameter['name'] = 'paper';
$parameter['label'] = 'etude';
$parameter['has_many'] = array('paragraph' => array('structure' => 'tree',
                                                    'dependent' => true));
