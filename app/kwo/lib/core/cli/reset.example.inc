<?php

return false;

try {

  $models = array('purchase', 'order', 'cart', 'addressee',
                  'transaction', 'debit', 'credit',
                  'user', 'person', 'url',
                  'favorite',
                  'sponsorship', 'notification',
                  'comment', 'abuse');
  foreach ($models as $model) {
    if (!Model::exists($model)) continue ;
    echo '- '.$model."\n";
    $items = Item::collect($model);
    foreach ($items as $item) {
      $item->destroy();
    }
    $dbo->getTable(T($model))->truncate();
  }

  $tables = array(Captcha::TABLE, Item::TABLE_REVISION,
                  Item::TABLE_LOG, Interaction::TABLE,
                  Item::TABLE_DRAFT, Smtp::TABLE,
                  T('translation'));
  foreach ($tables as $table) {
    if (!$dbo->hasTable($table, false)) {
      S('log')->trace('table unknown : '.$table);
      continue ;
    }
    $dbo->getTable()->truncate();
  }

  Alert::onAppReset();
  Document::onAppReset();
  Tracker::onAppReset();
  Meta::onAppReset();
  
  $set = (array) P('search.models');
  foreach ($set as $model_id) {
    if ($model_id < 1) continue ;
    $model_name = Model::name($model_id);
    $items = Item::collect($model_name);
    foreach ($items as $item) {
      $item->index();
    }
  }

  foreach ($dbo->getTables() as $table) {
    $table->repair();
    $table->optimize();
  }

}
catch (Exception $e) { $err->add($e); }
