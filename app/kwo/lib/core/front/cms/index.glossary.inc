<?php

try {
  
  $glossary = Glossary::getInstance($req->id ? (int) $req->id : 1);
  
  $letters = $glossary->getLetters(); 

  $req->track($glossary->title, 'glossary');

  $ctx->letters = array();
  foreach (range('a', 'z') as $letter) {
    $ctx->letters[$letter] = isset($letters[$letter]);
  }
  $ctx->letters['0-9'] = isset($letters['0']);

  $ctx->terms = $glossary->getTerms(array('order' => array('name')));

  $ctx->glossary = $glossary; 
    
  $res->setHead($glossary->title);
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
