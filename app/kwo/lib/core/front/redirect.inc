<?php

try {

  $advert = Advert::getInstanceByKey($req->item);
  if (!$advert->isOnline()) {
    throw new Exception('advert ('.$advert->getId().'), invalid status');
  }

  $advert->track(Interaction::TYPE_REDIRECT);

  Conversion::add($advert,
                  array('unique' => true,
                        'worth' => $advert->profit));

  if ((S('app')->ts % 2) && $advert->threshold > 0) {
    $n = Interaction::count($advert, Interaction::TYPE_REDIRECT);
    if ($n > $advert->threshold) {
      $advert->setStatus(Item::STATUS_OFFLINE);
    }
  }

  $res->sendRedirect($advert->target_url);

}
catch (Exception $e) {
  $log->error($e);
  $req->debug();
  $res->sendRedirect('/');
}