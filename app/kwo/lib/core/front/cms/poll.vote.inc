<?php

try {

  $choice_id = (int) $req->choice_id;

  if ($choice_id < 1) {
    throw new Exception(l('error.choice'));
  }

  $choice = Choice::getInstance($choice_id);
  $poll = $choice->getPoll();

  if (!$poll->isOnline()) {
    throw new Exception(l('error.poll.closed'));
  }

  $poll->track(Interaction::TYPE_VOTE, $choice_id);

  $ctx->choice_id = $choice->getId();
  $ctx->poll_id = $poll->getId();

}
catch (Exception $e) { $err->add($e); }


