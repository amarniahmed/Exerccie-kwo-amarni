<?php

try {

  if (empty($req->file_path)) {
    throw new Exception('Error missing file');
  }

  $opts = array('required_fields' => array('from', 'to'));
  $csv = CsvReader::getInstance($req->file_path, $opts);

  foreach ($csv as $hash) {
    try {
      if (strlen($hash['from']) < 1 || strlen($hash['to']) < 1) continue;
      $args = array('flags' => Redirection::FLAG_PERMANENT,
                    'name'  => $hash['from'],
                    'destination'  => $hash['to'],
                    'status'  => 1);
      $id = (int) Redirection::id($args);
      $id = $id > 0 ? $id : true;
      $redirection = Redirection::getInstance($id);
      $redirection->commit($args);
    }
    catch (DuplicateRecordException $e) { }
    catch (Exception $e) { }
  }

  $res->sendMessage(c(l('import réalisé avec succès')));

}
catch (Exception $e) { $err->add($e); }