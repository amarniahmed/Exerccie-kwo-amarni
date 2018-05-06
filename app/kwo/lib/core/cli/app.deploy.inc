<?php

try {

  $res->write("\n".'App Factory : '."\n\n");

  $root_password = System::getMysqlRootPassword();
  if ($root_password === false) {
    $root_password = Shell::prompt(' + Password mysql root ?');
  }

  $dbm = DataBaseObject::getInstance(array('login' => 'root',
                                           'password' => $root_password,
                                           'name' => 'mysql'));
  $dbm->query('SELECT * FROM user LIMIT 1');

  $project = Shell::prompt(' + App name ?');
  if (strlen($project) < 3 || strlen($project) > 11 ||
      !Valid::isAlphanum($project, true)) {
    throw new Exception('invalid name ['.$project.'] (alplanum only, length between 3 and 11)');
  }
  if (file_exists('/var/web/'.$project)) {
    throw new Exception('directory exists [/var/web/'.$project.']');
  }
  if (file_exists('/etc/kernix/http/'.$project.'.conf')) {
    throw new Exception('virtualhost exists [/etc/kernix/http/'.$project.'.conf]');
  }

  $host = 'www.'.$project.'.pulsar';

  $db_name = Shell::prompt(' + DB name ?', $project);
  if (strlen($db_name) < 3 || strlen($db_name) > 11 || !Valid::isAlphanum($db_name, true)) {
    throw new Exception('invalid name ['.$db_name.'] (alphanum only, length between 3 and 11)');
  }
  if (file_exists('/var/lib/mysql/'.$db_name)) {
    throw new Exception('db exists [/var/lib/mysql/'.$db_name.']');
  }

  $rs = $dbm->query('SELECT Host FROM user WHERE User=:name:', array(':name:' => $db_name));
  if ($rs->foundRows() >= 1) throw new Exception('User ['.$db_name.'] exists in mysql:user');

  $rs = $dbm->query('SELECT Host FROM db WHERE Db=:name:', array(':name:' => $db_name));
  if ($rs->foundRows() >= 1) throw new Exception('Db ['.$db_name.'] exists in mysql:db');

  $db_password = Shell::prompt(' + DB password ?', SH::password());

  $seed = SH::password(10);

  do {
    $conf = Shell::prompt(' + App conf ?', $project.'.conf.inc');
    $conf = strpos($conf, '.') === false ? $conf.'.conf.inc' : $conf;
  }
  while (!is_file(App::PATH.'/'.$conf));

  list ($app, ) = explode('.', $conf);

  $app = App::asHash($app);

  $res->write("\n");

  $extensions = array();
  foreach (Extension::all() as $name => $extension) {
    if ($name == $app['app.extension'] || $name == CORE) {
      $extensions[] = $name;
      continue ;
    }
    if ($extension['id'] >= Extension::THRESHOLD) continue ;
    if ($name == DEVEL) continue ;
    if (Shell::confirm(' + Include extension ['.$name.']', true)) {
      $extensions[] = $name;
    }
  }

  $path = '/var/web/'.$project;

  $res->write("\n".'Details'."\n\n");
  $res->write(' - app name   : '.$project."\n");
  $res->write(' - app path   : '.$path."\n");
  $res->write(' - app conf   : '.$conf."\n");
  $res->write(' - ext name   : '.$app['app.extension']."\n");
  $res->write(' - hostname   : '.$host."\n");
  $res->write(' - db_name    : '.$db_name."\n");
  $res->write(' - db_pass    : '.$db_password."\n");
  $res->write(' - extensions : '.join(', ', $extensions)."\n");

  $db_user = $db_name;

  if (!Shell::confirm("\n".'Proceed', true)) return ;

  $dbo->ping();
  $dbm->ping();

  $res->write("\n");

  try {
    $dbm->flush();
  }
  catch (Exception $e) { $log->error($e); }

  system('cp -a /var/web/kwo '.$path);
  if (!is_dir($path)) throw new Exception('copy failed');
  notify('copy tree');

  $ret = $dbm->exec('INSERT INTO db'
                    .   " VALUES ('localhost', '".$db_name."', '".$db_user."',"
                    .           " 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y',"
                    .           " 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y')");
  if ($ret === false) throw new Exception('can not insert into mysql:db');

  $ret = $dbm->exec('INSERT INTO user (Host, User, Password)'
                    .   " VALUES ('localhost', '".$db_name."', password('".$db_password."'))");
  if ($ret === false) throw new Exception('can not insert into mysql:user');

  $dbm->exec('flush privileges');

  system('ln -s '.$path.'/mysql /var/lib/mysql/'.$db_name);
  system('replace -s kwo '.$project
                  .' dbpassword '.$db_password
                  .' XXXXXXXX '.$seed.
         ' -- '.$path.'/'.Platform::CONFIG);

  $dbm->exec('ALTER DATABASE '.$db_name.' CHARACTER SET utf8 COLLATE utf8_general_ci');

  notify('db setup');

  $dbr = DataBaseObject::getInstance(array('login' => 'root',
                                           'password' => $root_password,
                                           'name' => $db_name));

  system('replace -s "ServerAlias       *.kwo" "" www.devel.kwo '.$host.' kwo '.$project.' "/devel/" "/'.$project.'/"'
         .' -- '.$path.'/'.ETC_PATH.'/virtualhost.conf');
  system('ln -s '.$path.'/'.ETC_PATH.'/virtualhost.conf'
         .     ' /etc/kernix/http/'.$project.'.conf');
  notify('virtualhost setup');

  foreach (glob($path.'/'.App::PATH.'/*.conf.inc') as $file) {
    if (basename($file) != $conf) unlink($file);
  }
  notify('app conf cleaning');

  system('replace -s .kwo .pulsar'
         .' -- '.$path.'/'.App::PATH.'/'.$conf);
  notify('app conf setup');

  if (is_dir($path.'/'.DOC_PATH.'/.trash')) {
    system('rm -r '.$path.'/'.DOC_PATH.'/.trash');
  }
  if (is_dir($path.'/.git')) {
    system('rm -r '.$path.'/.git');
  }
  if (is_file($path.'/.gitignore')) {
    system('rm '.$path.'/.gitignore');
  }
  foreach (glob($path.'/'.DOC_PATH.'/*') as $file) {
    if (basename($file) === $project) continue ;
    system('rm -r '.$file);
  }
  notify('doc cleaning');

  remote_exec($path, 'var.clean');
  notify('var clean');

  if (is_dir($path.'/usr')) {
    system('rm -r '.$path.'/usr/*');
    notify('usr remove');
  }

  if (is_dir($path.'/tmp')) {
    system('rm -r '.$path.'/tmp');
    notify('tmp remove');
  }

  if (is_dir($path.'/pub')) {
    system('rm -r '.$path.'/pub');
    notify('pub remove');
    system('mkdir -p '.$path.'/pub/tmp');
    system('chown -R apache:apache '.$path.'/pub');
    notify('pub/tmp creation');
  }

  remote_exec($path, 'build');
  notify('build');

  remote_exec($path, 'core.reset',
              array('confirm' => 'yes'));
  notify('core reset');

  remote_exec($path, 'item.clean',
              array('full' => 'yes'));
  notify('item clean');

  foreach (Extension::all() as $name => $extension) {
    if (is_dir($path.'/'.LIB_PATH.'/'.$name.'/infos')) {
      system('rm -r '.$path.'/'.LIB_PATH.'/'.$name.'/infos');
    }
    if (!in_array($name, $extensions)) {
      remote_exec($path, 'extension.uninstall',
                  array('name' => $name,
                        'confirm' => 'no',
                        'archive' => 'no'));
    }
    remote_exec($path, 'build');
  }

  $dbr->resetTables();

  foreach ($dbr->getTables() as $table) {
    try {
      $table->repair()->optimize();
    }
    catch (Exception $e) { $log->error($e); }
  }

  remote_exec($path, 'build');
  notify('build');

  remote_exec($path, 'platform.app.clean',
              array('full' => 'yes'));
  notify('platform app clean');

  remote_exec($path, 'item.clean',
              array('full' => 'yes'));
  notify('item clean');

  $res->write("\n");

  system('service httpd graceful');

  $res->write("\n");

}
catch (Exception $e) {
  $res->write("\n\n".$e->getMessage()."\n\n");
}

function notify($str) {
  S('res')->write(' # '.str_pad($str.' ', 48, '.').' OK'."\n");
}

function remote_exec($path, $str, $args=array()) {
  if (strlen($path) < 8) throw new Exception('bad path');
  $message = $str;
  if (!empty($args)) {
    $message .= ' '.json_encode($args);
  }
  system('cd '.$path.' && php '.Platform::BOOTSTRAP.' log.trace message="'.$message.'"');
  $cmd = 'cd '.$path.' && php '.Platform::BOOTSTRAP.' '.$str;
  foreach ($args as $key => $value) {
    $cmd .= ' '.$key.'='.$value;
  }
  system($cmd);
}