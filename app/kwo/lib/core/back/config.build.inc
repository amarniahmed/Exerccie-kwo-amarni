<?php

try {

  ConfigManager::build();

}
catch (Exception $e) { $err->add($e); }
