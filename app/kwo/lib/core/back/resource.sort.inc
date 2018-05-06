<?php

try {

  $set = explode(',', $req->resources);

  $i = 1;
  foreach ($set as $id) {
    $resource = Resource::getInstance($id);
    $resource->commit(array('position' => $i));
    $i++;
  }

}
catch (Exception $e) { $err->add($e); }
