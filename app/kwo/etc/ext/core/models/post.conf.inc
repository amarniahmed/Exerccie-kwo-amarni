<?php

$parameter['id'] = 430;
$parameter['name'] = 'post';
$parameter['label'] = 'billet';
$parameter['belongs_to'] = array('blog');
$parameter['has_many'] = array('heading' => array('through' => true,
                                                  'unique' => true));

