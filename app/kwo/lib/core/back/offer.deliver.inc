<?php

try {

  $offer = Offer::getInstance((int) $req->id);

  $offer->deliver();

  $res->sendMessage(c(l('le message a bien été envoyé aux destinataires de tests')));

}
catch (Exception $e) { $err->add($e); }