<?php

try {

  $item = Form::getInstance($req->id);
  $item->build($req->xml);
  $res->write('Kwo.warn("ok")');
  
}
catch (Exception $e) { }

