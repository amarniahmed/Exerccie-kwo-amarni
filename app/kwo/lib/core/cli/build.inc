<?php

try {

  ConfigManager::platform();

}
catch (Exception $e) { $log->error($e); }

