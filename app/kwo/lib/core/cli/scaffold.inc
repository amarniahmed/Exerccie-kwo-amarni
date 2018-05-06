<?php

//php init.php scaffold extension=shop models=product,order

$mode = 0777;

try {
  
  if (file_exists(Platform::LOCKER)) {
    throw new Exception('locker file exists ['.Platform::LOCKER.'] ; '.
                        'please remove manually or buid');
  }

  if (!$req->hasAttribute('extension', true) ||
      ($req->extension != CORE && strlen($req->extension) < 3)) {
    throw new Exception('invalid extension ['.$req->extension.'], length must be >= 3');
  }

  if (is_numeric($req->extension[0])) {
    throw new Exception('invalid extension ['.$req->extension.'], can not start with a numeric');
  }

  $extension = strtolower($req->extension);
  
  if (in_array($extension, array('front', 'back', 'api', 'account', 'middle', 'devel', 'test',
                                 'cache', 'log', 'scheduler', 'session', 'task'))) {
    throw new Exception('invalid extension name ['.$req->extension.']');
  }

  if (!is_dir(LIB_PATH.'/'.$extension)) {
    if (!Shell::confirm("\n".'Create extension <'.$extension.'> ?', true)) return ;
    if ($req->id > 500) {
      $extension_id = $req->id;
    }
    else {
      $extension_id = Extension::THRESHOLD;
      foreach (Extension::all() as $e) {
        if ($e['id'] >= $extension_id) {
          $extension_id = $e['id'] + 100;
          while ($e['id'] >= $extension_id) {
            $extension_id += 100;
          }
        }
      }
    }
    
    mkdir(LIB_PATH.'/'.$extension.'/back', $mode, true);
    mkdir(LIB_PATH.'/'.$extension.'/class', $mode);
    mkdir(LIB_PATH.'/'.$extension.'/cli', $mode);
//    mkdir(LIB_PATH.'/'.$extension.'/infos', $mode);
//    mkdir(LIB_PATH.'/'.$extension.'/sql', $mode);

    mkdir(Extension::PATH.'/'.$extension.'/models', $mode, true);
    $table_prefix = $req->hasAttribute('table_prefix') ? $req->table_prefix : $extension;
    $table_prefix = trim($table_prefix, '_*') . '_';
    file_put_contents(Extension::PATH.'/'.$extension.'/parameters.conf.inc', 
'<?php

$parameter[\'id\'] = '.$extension_id.';
$parameter[\'name\'] = \''.$extension.'\';
$parameter[\'table_prefix\'] = \''.$table_prefix.'\';
');

//    mkdir(LIB_PATH.'/'.$extension.'/account', $mode);
    mkdir(LIB_PATH.'/'.$extension.'/front', $mode);

    if (!is_dir(WEB_PATH.'/'.$extension)) {
      mkdir(WEB_PATH.'/'.$extension.'/images', $mode, true);
      mkdir(WEB_PATH.'/'.$extension.'/templates/front', $mode, true);
      mkdir(WEB_PATH.'/'.$extension.'/scripts/front', $mode, true);
      mkdir(WEB_PATH.'/'.$extension.'/styles/front', $mode, true);
      mkdir(WEB_PATH.'/'.$extension.'/files', $mode);
/*      file_put_contents(WEB_PATH.'/'.$extension.'/scripts/front.js', 
                        'if (!Kwo) var Kwo = {};'."\n");*/
      touch(WEB_PATH.'/'.$extension.'/styles/front.css');
      if (file_exists(WEB_PATH.'/'.DEVEL.'/images/favicon.png')) {
        copy(WEB_PATH.'/'.DEVEL.'/images/favicon.png',
             WEB_PATH.'/'.$extension.'/images/favicon.png');
      }
      $pix_path = trim(PIX_URL, '/');
      if (file_exists($pix_path.'/favicon.ico')) {
        copy($pix_path.'/favicon.ico',
             WEB_PATH.'/'.$extension.'/images/favicon.ico');
      }
      if (file_exists(WEB_PATH.'/'.DEVEL.'/files/robots.txt')) {
        copy(WEB_PATH.'/'.DEVEL.'/files/robots.txt',
             WEB_PATH.'/'.$extension.'/files/robots.txt');
      }
    }
  }
  else {
    $extension_id = Extension::parameter($extension, 'id');
    $table_prefix = Extension::parameter($extension, 'table_prefix');
    if ($extension_id < 1) {
      throw new Exception('error extension_id ('.$extension_id.')');
    }
  }
  
  if (!is_dir(LIB_PATH.'/'.$extension.'/xml')) {
    mkdir(LIB_PATH.'/'.$extension.'/xml', $mode);
  }

  $models = array();
  if ($req->hasAttribute('model', true)) {
    $models = explode(',', $req->model);
  }
  elseif ($req->hasAttribute('models', true)) {
    $models = explode(',', $req->models);
  }

  if (count($models) >= 1) {
    $model_id = $extension_id; 
    foreach (Model::all() as $id => $model) {
      if (Model::parameter($model, 'extension') === $extension && $id >= $model_id) {
        $model_id = $id + 1;
      }
    }
  }

  sort($models);
  
  $table_prefix = rtrim($table_prefix, '*');
  $table_prefix = empty($table_prefix) ? $extension.'_' : $table_prefix;

  $invalid_models = array('association', 'bitmap', 'branch', 
                          'context', 'developer', 
                          'editor', 'file', 'folder', 'item', 
                          'record', 'workspace');

  $uis = array('edit', 'home', 'finder');
  foreach ($models as $model) {
    $model = strtolower(trim($model));
    if (in_array($model, $invalid_models)) {
      $log->warn('model name forbidden ['.$model.']');
      continue ;
    }
    if (substr($model, -2) == 'ie') { 
      $tmp = substr($model, 0, -2).'y';
      if (Model::exists($tmp) === true) {
        throw new Exception('model ['.$tmp.'] exists');
      }
    }
    if (file_exists(LIB_PATH.'/'.CORE.'/class/'.ucfirst($model).'.class.inc')) {
      $log->warn('model name forbidden ['.$model.'], class exists in '.CORE);
      continue ;
    }
    if (isset($GLOBALS['classes'][ucfirst($model)])) {
      $log->warn('model name forbidden ['.$model.'], '.
                 'class exists in '.$GLOBALS['classes'][ucfirst($model)]);
      continue ;
    }
    if (substr($model, -1) === 's') {
      throw new Exception('invalid name, final s forbidden ('.$model.')');
    }
    if (!Valid::isAlphanum($model, true)) {
      throw new Exception('invalid name, only letters are accepted ('.$model.')');
    }
    if (strlen($model) <= 2) {
      throw new Exception('bad model ('.$model.')');
    }
    if (Model::exists($model) === true) {
      $log->warn('model ['.$model.'] exists');
      continue ;
    }
    $class = LIB_PATH.'/'.$extension.'/class/'.ucfirst($model).'.class.inc';
    if (!file_exists($class)) {
      file_put_contents($class, '<?php'."\n\n".'class '.ucfirst($model).' extends ActiveRecord {}');
    }
    $conf = Extension::PATH.'/'.$extension.'/models/'.$model.'.conf.inc';
    if (!file_exists($conf)) {
      $table = $table_prefix.$model;
      file_put_contents($conf, '<?php

$parameter[\'id\'] = '.$model_id.';
$parameter[\'name\'] = \''.$model.'\';'."\n");
      $model_id++;
    }
    if (!$dbo->hasTable($table)) {
      $dbo->exec('CREATE TABLE '.$dbo->getName().'.`'.$table.'` (
`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY ,
`content` TEXT NOT NULL ,
`flags` INT UNSIGNED NOT NULL default 0 ,
`image` VARCHAR( 255 ) NOT NULL ,
`name` VARCHAR( 255 ) NOT NULL ,
`status` TINYINT UNSIGNED NOT NULL DEFAULT 0
) ENGINE = MYISAM');
    }
    foreach ($uis as $ui) {
      $dest = LIB_PATH.'/'.$extension.'/xml/'.strtolower($model).'.'.$ui.'.xml';
      if (!file_exists($dest)) {
        $src = LIB_PATH.'/'.CORE.'/xml/item/'.$ui.'.xml';
        if (!file_exists($src)) continue ;
        copy($src, $dest);
      }
    }
  }

  Shell::chownDir(WEB_PATH);

  touch(Platform::LOCKER);

}
catch (Exception $e) { $log->error($e); }
