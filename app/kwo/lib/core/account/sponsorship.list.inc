<?php

try {

  $opts = array('filters' => array('|>|godson_id' => 0,
                                   'user_id' => $user->getId()),
                'include' => array('email'),
                'reverse' => true);
  $godsons = Sponsorship::collect($opts);

  $opts = array('filters' => array('godson_id' => 0,
                                   'user_id' => $user->getId()),
                'include' => array('email'),
                'reverse' => true);
  $prospects = Sponsorship::collect($opts);


  $ctx->godsons = $godsons;
  $ctx->prospects = $prospects;

  $res->setMenu('sponsorships');
  $res->setTitle(l('account.sponsorships'));


}
catch (Exception $e) { $err->add($e); }