<?php

try {

  $res->setHead('TPE : '.P('app.name'));
  $res->useTemplate();

}
catch (Exception $e) {  }