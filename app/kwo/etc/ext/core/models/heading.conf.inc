<?php

$parameter['id'] = 429;
$parameter['name'] = 'heading';
$parameter['label'] = 'catégorie';
$parameter['belongs_to'] = array('blog');
$parameter['has_many'] = array('post' => array('through' => true));
