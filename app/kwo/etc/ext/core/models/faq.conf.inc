<?php

$parameter['id'] = 415;
$parameter['name'] = 'faq';
$parameter['has_many'] = array('component' => array('dependent' => true,
                                                    'order' => array('position')),
                               'section' => array('dependent' => true,
                                                  'order' => array('position')));
