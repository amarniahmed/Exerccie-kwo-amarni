<?php

try {

  if (file_exists(Platform::LOCKER)) {
    throw new Exception('locker file exists ['.Platform::LOCKER.'] ; '.
                        'please remove manually or buid');
  }

  $app_name = $req->name;
  if (empty($app_name)) {
    $app_name = Shell::prompt('App name ?');
  }
  else {
    echo 'App name : '.$app_name."\n";
  }
  $app_conf = App::PATH.'/'.$app_name.'.conf.inc';
  if (!file_exists($app_conf)) {
    throw new Exception('invalid app ['.$app_name.']');
  }

  $app = App::asAttributes($app_name);

  $extension_name = $app['app.extension'];

  if (!Extension::exists($extension_name)) {
    throw new Exception('invalid extension ['.$extension_name.']');
  }

  $extension = Extension::asAttributes($extension_name);

  if ($extension['id'] < Extension::THRESHOLD) {
    throw new Exception('invalid extension id ['.$extension['id'].']');
  }

  if (!Shell::confirm('Remove extension : '.$extension_name, true)) return ;

  $log->trace('>> app');
  foreach (Model::all() as $model) {
    if (!Model::hasField($model, App::COLUMN)) continue ;
    $opts = array('filters' => array(App::COLUMN => $app_name));
    $items = Item::collect($model, $opts);
    foreach ($items as $item) {
      $item->destroy();
    }
    $dbo->resetAutoIncrement(T($model));
  }

  $log->trace('>> ext remove');
  try {
    $ext = Extension::getInstance($extension_name);
    $ext->archive();
    $ext->uninstall();
  }
  catch (Exception $e) {
    $log->error($e);
  }

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}