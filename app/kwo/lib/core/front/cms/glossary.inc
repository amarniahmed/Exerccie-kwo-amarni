<?php

try {
  
  $glossary = Glossary::getInstance(
                                    $req->id >= 1 ? $req->id : 1);
  
  $letters = $glossary->getLetters(); 

  $ctx->letters = array();
  foreach (range('a', 'z') as $letter) {
    $ctx->letters[$letter] = isset($letters[$letter]);
  }
  $ctx->letters['0-9'] = isset($letters['0']);

  if ($req->hasAttribute('term')) {
    $ctx->letter = $req->term[0];
  }
  elseif ($req->hasAttribute('letter')) {
    $ctx->letter = $req->letter;
  }
  else $ctx->letter = 'a';

  $ctx->term = $req->term; 
  $ctx->glossary = $glossary; 
    
  $res->setHead($glossary->title);
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }