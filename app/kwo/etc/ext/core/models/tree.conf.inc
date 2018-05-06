<?php

$parameter['id'] = 30;
$parameter['name'] = 'tree';
$parameter['label'] = 'arborescence';
$parameter['has_many'] = array('node' => array('structure' => 'tree',
                                               'dependent' => true));