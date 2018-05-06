<?php

class Shell {
  const C_RESET = "\033[0m";
  
  const C_BLACK = "\033[0;30m";
  const C_RED = "\033[0;31m";
  const C_GREEN = "\033[0;32m";
  const C_BROWN = "\033[0;33m";
  const C_BLUE = "\033[0;34m";
  const C_PURPLE = "\033[0;35m";
  const C_CYAN = "\033[0;36m";
  const C_LIGHT_GRAY = "\033[0;37m";
  
  const C_GRAY = "\033[1;30m";
  const C_LIGHT_RED = "\033[1;31m";
  const C_LIGHT_GREEN = "\033[1;32m";
  const C_YELLOW = "\033[1;33m";
  const C_LIGHT_BLUE = "\033[1;34m";
  const C_LIGHT_PURPLE = "\033[1;35m";
  const C_LIGHT_CYAN = "\033[1;36m";
  const C_WHITE = "\033[1;37m";
}

if (!function_exists('readline')) {
  throw new Exception('kwo shell needs readline()');
}


$version = '1.1';
$prompt = '>> ';

$res->close();

$_models = array();
foreach (Model::all() as $model) {
  $_models[] = ucfirst($model);
}

readline_completion_function('shell_complete');

echo 'KWO Shell <'.$version.'>'.PHP_EOL.PHP_EOL;

while ($command = readline($prompt)) {
  $command = trim($command);
  readline_add_history($command);
  if (in_array($command, array('quit', 'exit'))) {
    return ;
  }
  eval('$ret = '.$command.';');
  echo display($ret).PHP_EOL.PHP_EOL;
}

function shell_complete($str, $pos) {
  $in = readline_info('line_buffer');
  $a = array();
  if (strpos($in, "ActiveRecord::getInstance('") === 0 ||
      strpos($in, "ActiveRecord::collect('") === 0) {
    foreach (Model::all() as $model) {
      $a[] = ucfirst($model);
    }
    return $a;
  }
  elseif (!empty($in) && $in != "ActiveRecord::") {
    foreach (Model::all() as $model) {
      $a[] = "ActiveRecord::getInstance('".ucfirst($model)."',";
      $a[] = "ActiveRecord::collect('".ucfirst($model)."',";
    }
    return $a;
  }
  $a[] = "ActiveRecord::getInstance('";
  $a[] = "ActiveRecord::collect('";
  return $a;
}

function display($value) {
  $debug = null;
  if ($value instanceof Collection) {
    $value = $value->asArray();
    //$debug = S('dbo')->sql;
  }
  elseif ($value instanceof ActiveRecord) {
    $value = $value->getAttributes();
    //$debug = S('dbo')->sql;
  }
  elseif ($value instanceof ResultSet) {
    $value = $value->asArray();
  }
  elseif (is_object($value)) {
    $value = ($value instanceof Exception) ? $value->getMessage() : (array) $value; 
  }
  elseif ($value === 0) { 
    $value = 'INT(0)';
  }
  elseif (is_null($value)) { 
    $value = 'NULL';
  }
  elseif (is_bool($value)) {
    $value = 'BOOL('.((int)$value).')';
  }
  if (is_array($value)) { 
    $value = var_export($value, true); 
  }
  $out = $value;
  $out = $debug ? $out.PHP_EOL.PHP_EOL.Shell::C_YELLOW.$debug.Shell::C_RESET : $out;
  return $out;
}

