<?php

try { 
  
  $poll = Poll::getInstance($req->id);
  $choice = Choice::getInstance($req->choice_id);

  $ctx->results = $poll->getResults();
  $ctx->choice = $choice; 
  
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
