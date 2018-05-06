<?php

try {

  $ids = array_keys(Model::all());

  S('log')->debug('INVALID MODELS :');
  foreach ($dbo->getTables() as $table) {
    if (!$table->hasField('model_id')) continue ;
    if (in_array($table->getName(), array(T('node'), T('tree')))) continue ;
    $rs = $dbo->query('SELECT model_id'
                      .' FROM '.$table->getName()
                      .' WHERE model_id NOT IN ([ids])'
                      .  ' AND model_id>0'
                      .' GROUP BY model_id',
                      array('[ids]' => $ids));
    $set = $rs->asSet();
    if (empty($set)) continue ;
    $n = $dbo->exec('DELETE FROM '.$table->getName()
                    .    ' WHERE model_id IN ([set])',
                    array('[set]' => $set));
    S('log')->debug(' - ('.$n.') deletions - model', $table->getName());
    $table->optimize();
  }

  $ids = array();
  foreach (Extension::all() as $extension) {
    $ids[] = $extension['id'];
  }

  foreach ($dbo->getTables() as $table) {
    if (!$table->hasField('extension_id')) continue ;
    $rs = $dbo->query('SELECT extension_id'
                      .' FROM '.$table->getName()
                      .' WHERE extension_id NOT IN ([ids])'
                      .  ' AND extension_id>0'
                      .' GROUP BY extension_id',
                      array('[ids]' => $ids));
    $set = $rs->asSet();
    if (empty($set)) continue ;
    $n = $dbo->exec('DELETE FROM '.$table->getName()
                    .    ' WHERE extension_id IN ([set])',
                    array('[set]' => $set));
    S('log')->debug('- ('.$n.') deletions - extension', $table->getName());
    $table->optimize();
  }
  if ($req->full != 'yes') return ;
  S('log')->debug('INVALID RECORDS :');
  $tables = array_merge(P('item_tables'));

  foreach ($tables as $table) {
    $table = new Table($table); // ML
    $field_model = 'model_id';
    $field_record = 'record_id';
    if ($table->getName() == Meta::TABLE) {
      $field_model = Meta::FIELD_MODEL;
      $field_record = Meta::FIELD_RECORD;
    }
    $models = $dbo->asSet('SELECT DISTINCT '.$field_model
                          .' FROM '.$table->getName()
                          .' WHERE '.$field_model.'>0');
    if (count($models) < 1) continue ;
    foreach ($models as $model_id) {
      $model_id = (int) $model_id;
      if (!Model::exists($model_id)) {
        $n = $dbo->exec('DELETE FROM '.$table->getName()
                            .' WHERE '.$field_model.'='.$model_id);
        S('log')->debug('- ('.$n.') deletions', $table->getName().' : ('.$model_id.')');
        continue ;
      }
      $rs = $dbo->query('SELECT DISTINCT t.'.$field_record
                        .' FROM '.$table->getName().' AS t'
                        .' LEFT JOIN '.T($model_id).' AS j'
                        .       ' ON t.'.$field_record.'=j.id'
                              .' AND t.'.$field_model.'='.$model_id
                        .' WHERE j.id IS NULL'
                        .  ' AND t.'.$field_model.'='.$model_id);
      $records = $rs->asSet();
      if (count($records) < 1) continue ;
      $n = $dbo->exec('DELETE FROM '.$table->getName()
                          .' WHERE '.$field_model.'='.$model_id
                          .  ' AND '.$field_record.' IN ('.join(',', $records).')');
      S('log')->debug('- ('.$n.') deletions',
                      $table->getName().' : '.Model::name($model_id).' ('.$model_id.')');
    }
  }

  foreach (P('item_tables') as $table) {
    if (!$dbo->hasTable($table, false)) continue ;
    $table = $dbo->getTable($table);
    $table->repair();
    $table->optimize();
  }

}
catch (Exception $e) { $err->add($e); }

