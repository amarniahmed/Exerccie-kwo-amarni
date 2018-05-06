<?php

try {

  $opts = array('reverse' => true,
                'metas' => array('created_at'),
                'filters' => array('|>|status' => 0,
                                   '|<|status' => 10,
                                   'recipient_model_id' => $user->getModelId(),
                                   'recipient_record_id' => $user->getId()));
  $invitations = Invitation::collect($opts);

  $ctx->invitations = $invitations;

  $res->setMenu('invitations');
  $res->setTitle(l('account.invitations'));

}
catch (Exception $e) { $err->add($e); }
