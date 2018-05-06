<?php

/***

http://domain/payment.callback/-/psp/paybox
http://domain/payment.callback/-/psp/cybermut

***/

try {

  $log->enable();
  $req->debug();
  $res->close();
  Method::callback();

}
catch (Exception $e) {
  $req->debug();
  $log->error($e);
}