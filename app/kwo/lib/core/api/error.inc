<?php

try {

  throw new Exception('resource not found',
                      ApiManager::ERROR_NOT_FOUND);

}
catch (Exception $e) { $err->add($e); }
