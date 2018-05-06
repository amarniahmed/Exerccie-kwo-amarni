<?php

try {
  
  $debug = false;

  if (App::count() == 1) {
    $apps = array(S('app')->getName());
  }
  elseif (Extension::exists(DEVEL)) {
    $apps = Extension::names();
  } 
  $apps[] = '';

  $opts = array('filters' => array('|<>|app' => $apps));
  foreach (Model::all() as $model) {
    if (!Model::hasField($model, 'app')) continue ;
    $items = Item::collect($model, $opts);
    if ($items->count() < 1) continue ;
    if ($debug == true) print("\n".strtoupper($model).' ('.Model::id($model).')'."\n");
    foreach ($items as $item) {
      try {
        if ($debug == true) {
          print(' '.str_pad($item->id, 5).' ['.str_pad($item->app, 10).'] '.$item->name."\n");
        }
        else {
          $item->destroy();
        }
      }
      catch (Exception $e) { 
        $log->error($e);
      }
    }
    $dbo->resetAutoIncrement(T($model));
  }

  if (Extension::exists(DEVEL)) {
    $extension = Extension::getInstance(CORE);
    foreach ($extension->getModels() as $model) {
      $table = $dbo->getTable($model['table']);
      $table->optimize()->resetAutoIncrement();
    }
  }
  
}
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}
