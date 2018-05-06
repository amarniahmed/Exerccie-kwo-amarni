<?php

try {
  
  $req->track(true, 'shop');
  
  $ctx->coupon = $req->getCookie()->getAttribute('coupon');

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
