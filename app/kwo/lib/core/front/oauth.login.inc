<?php

try {

  OAuth::login($req->provider);

}
catch (Exception $e) { $err->add($e); }

