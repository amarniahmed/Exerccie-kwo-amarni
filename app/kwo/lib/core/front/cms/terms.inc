<?php

try { 

  $letter = $req->letter ? $req->letter : 'a'; 
  $letter = strlen($letter) === 1 ? $letter : '0';

  $opts = array('filters' => array('letter' => $letter,
                                   'glossary_id' => intval($req->id ?: 1)),
                'order' => array('name'));
  $ctx->terms = Term::collect($opts); 

  $ctx->selected_term = $req->term;

  $res->useTemplate();

}
catch (Exception $e) {  }