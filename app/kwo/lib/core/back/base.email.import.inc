<?php

try {

  $base = Base::getInstance($req->id);

  if (!empty($req->file_path)) {
    $opts = array('required_fields' => array('email'));
    $csv = CsvReader::getInstance($req->file_path,
                                  $opts);
    foreach ($csv as $hash) {
      try {
        $base->addEmail($hash['email']);
      }
      catch (DuplicateRecordException $e) { }
      catch (Exception $e) { }
    }
  }

  if (!empty($req->emails)) {
    $emails = SH::asSet($req->emails);
    foreach ($emails as $email) {
      try {
        $base->addEmail($email);
      }
      catch (DuplicateRecordException $e) { }
      catch (Exception $e) { }
    }
  }

  $res->sendMessage(c(l('import réalisé avec succès')));

}
catch (Exception $e) { $err->add($e); }