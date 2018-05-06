<?php

try {

  $password = System::getMysqlRootPassword();
  if ($password === false) {
    $password = Shell::prompt('Password mysql root ?');
  }
 
  $dbm = DataBaseObject::getInstance(array('login' => 'root',
                                           'password' => $password, 
                                           'name' => 'mysql'));
  
  $dbm->exec('flush privileges');
  
  $n = $dbm->fetchValue('SELECT COUNT(*) FROM user WHERE User=:user:', 
                        array(':user:' => P('app.dsn.login')));
  
  if ($n < 1 && Shell::confirm('Insert ['.P('app.dsn.login').'] into table <mysql.user>')) {
    $dbm->exec('CREATE USER :user:@"localhost" IDENTIFIED BY :password:',
               array(':user:' => P('app.dsn.login'), 
                     ':password:' => P('app.dsn.password')));
  }
  
  $n = $dbm->fetchValue('SELECT COUNT(*)'
                        .' FROM db'
                        .' WHERE User=:user:'
                        .  ' AND Db=:db:', 
                        array(':user:' => P('app.dsn.login'), 
                              ':db:' => P('app.dsn.name')));
  
  if ($n < 1 && Shell::confirm('Insert ['.P('app.dsn.name').'] into table <mysql.db>')) {
    $table = $dbm->getTable('db');
    $fields = $table->getFields();
    $row = array();
    foreach ($fields as $field) {
      $row[$field] = 'Y';
    }
    $row['Host'] = 'localhost';
    $row['Db'] = P('app.dsn.name');
    $row['User'] = P('app.dsn.login');
    $cols = array('Grant_priv', 'Create_view_priv', 
                  'Create_routine_priv', 'Alter_routine_priv',
                  'Execute_priv', 'Event_priv', 'Trigger_priv');
    foreach ($cols as $col) {
      if (!in_array($col, $fields)) continue ;
      $row[$col] = 'N';
    }
    $dbm->insert('db', $row);
  }
  
  $dbm->exec('flush privileges');
  
  $datadir = $dbm->getVariable('datadir');
  $link = $datadir.'/'.P('app.dsn.name');
  if (!file_exists($link)) {
    $target = realpath('.').'/mysql';
    if (Shell::confirm('Create link <'.$link.'> -> <'.$target.'>')) {
      $ret = @symlink($target, $link);
      if ($ret === false) {
        echo 'symlink failure';
      }
    }
  }
  
}
catch (Exception $e) { $err->add($e); }
