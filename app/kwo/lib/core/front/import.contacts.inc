<?php

try {

  $import = ImportContacts::factory($req->login, $req->pass, 
                                    (int) $req->type);
  $contacts = $import->getContacts();
  
  if (empty($contacts)) {
    $contacts = array();
  }
  
  $emails = array();
  foreach ($contacts as $contact) {
    $emails[] = $contact['email'];
  }
  
  sort($emails);
  
  $ctx->emails = $emails;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }