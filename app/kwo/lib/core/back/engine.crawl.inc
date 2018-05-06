<?php

try {

  $n = Engine::crawl();

  $res->sendMessage(c(l('document(s) indexé(s)')).' : '.$n);

}
catch (Exception $e) { $err->add($e); }