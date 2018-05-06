<?php

try {

  $resource = Resource::getInstance($req->id);
  $resource->destroy();

}
catch (Exception $e) { $err->add($e); }
