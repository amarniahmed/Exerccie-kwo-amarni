<?php

try {

  $item = Item::getInstance(MODEL, $req->id);

  $campaign = Campaign::getInstanceByItem($item);

  $bases = $dbo->asSet('SELECT id FROM '.T('base')
                       .       ' WHERE (flags & '.Base::FLAG_TEST.') > 0'
                       .         ' AND name LIKE "%test%"');
  if (count($bases) < 1) {
    throw Exception('a BAT base must have the Base::FLAG_TEST flag'
                    .           ' **AND** the word "test" in its name');
  }

  $campaign->send($bases);

  $res->sendMessage(l('message envoyé').' (campaign='.$campaign->getId().')');

}
catch (Exception $e) { $err->add($e); }