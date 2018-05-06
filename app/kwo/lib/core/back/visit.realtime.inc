<?php

try {

  $blocks = array(array('label' => l('sources'), 'class' => 'mediums'),
                  array('label' => l('réseaux sociaux'), 'class' => 'socials'),
                  array('label' => l('sites référents'), 'class' => 'referers'),
                  array('label' => l('mots clés'), 'class' => 'queries'),
                  array('label' => l('pays'), 'class' => 'countries'),
                  array('label' => l('langues'), 'class' => 'locales'),
                  array('label' => l('pages actives'), 'class' => 'pages'),
                  array('label' => l('utilisateurs'), 'class' => 'users'));

  $ctx->blocks = $blocks;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }