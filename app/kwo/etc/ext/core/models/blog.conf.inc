<?php

$parameter['id'] = 428;
$parameter['name'] = 'blog';
$parameter['has_many'] = array('post' => array('dependent' => true),
                               'heading' => array('dependent' => true,
                                                  'order' => array('name')));
