<?php

try {

  $count = 5;

  $emails = (array) $req->emails;

  $email_count = count($emails);

  for ($i = 1; $i <= $count; $i++) {
    $emails[] = '';
  }

  $ctx->emails = $emails;
  $ctx->email_count = $email_count;

  $res->setMenu('sponsorships');
  $res->setTitle(l('account.sponsorship.send'));

}
catch (Exception $e) { $err->add($e); }