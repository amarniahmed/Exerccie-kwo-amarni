<?php

try {

  $user = $req->getUser();

  $addressee = Addressee::getInstance($req->id ? $req->id : true);

  if ($addressee->isItem() && !$user->hasAccessTo($addressee)) {
    return new MaliciousException();
  }

  $record = $req->record;

  $required_fields = array('first_name', 'last_name', 'address', 'city', 
                           'country_id', 'postal_code');
  foreach ($required_fields as $field) {
    if (empty($record[$field])) { 
      throw new Exception(l('error.fields.required'));
    }
    $record[$field] = SH::clean($record[$field], 
                                        array('strict' => true));
  }

  $addressee->commit($record);
  $addressee->setUser($user);

  $req->track(true, 'shop');

  $ctx->id = $addressee->getId();

}
catch (Exception $e) { $err->add($e); }
