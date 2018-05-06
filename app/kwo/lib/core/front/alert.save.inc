<?php

try {

  $user = $req->getUser();

  if (!$req->hasAttribute('keyword', true)) {
    throw new Exception('invalid keyword');
  }

  $keyword = Keyword::clean($req->keyword);
  if (strlen($keyword) <= 1) {
    throw new Exception(l('error.input.invalid'));
  }
  $item = Keyword::getInstance(Keyword::asId($keyword));

  if (!($item instanceof ActiveRecord)) {
    throw new Exception(l('error.input.invalid'));
  }

  $n = $dbo->fetchValue('SELECT COUNT(*)'
                        .' FROM '.T('alert')
                        .' WHERE model_id='.$item->getModelId()
                        .  ' AND record_id='.$item->getId()
                        .  ' AND user_id='.$user->getId());
  if ($n < 1) {
    $alert = Alert::getInstance();
    $alert->commit(array('model_id' => $item->getModelId(),
                         'record_id' => $item->getId(),
                         'user_id' => $user->getId()));
    $alert->setFlag(Alert::FLAG_DELIVER_EMAIL);
    $alert->setVisit($req);
    $req->track($keyword, 'alerts');
    $alert->convert();    
  }

  $opts = array('locale' => $res->getLocale());
  $snippet = Snippet::getInstance('alert.confirm', $opts);

  $ctx->title = $snippet->title;
  $ctx->content = $snippet->content;
  $ctx->alert = $alert;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }