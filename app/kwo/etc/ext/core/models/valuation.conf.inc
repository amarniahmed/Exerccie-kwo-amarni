<?php

$parameter['id'] = 38;
$parameter['name'] = 'valuation';
$parameter['has_many'] = array('criterion' => array('through' => 'core_criterion_valuation',
                                                    'cols' => array('grade' => 'grade')));
