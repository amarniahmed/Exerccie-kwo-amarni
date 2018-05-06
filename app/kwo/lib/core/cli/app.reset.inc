<?php

return false;

try {
  
  $models = array('comment', 'abuse', 
                  'valuation',
                  'purchase', 'order', 'addressee', 'shipment', 'payment',
                  'transaction', 'debit', 'credit', 'movement',
                  'favorite', 'user', 'person', 'announcement',
                  'discussion', 'invitation', 'message',
                  'topic', 
                  'submission',
                  'conversion', 'affiliate',
                  'sponsorship', 'notification', 'suggestion', 'subscription', 
                  'task', 'email', 'url');
  foreach ($models as $model) {
    if (!Model::exists($model)) {
      $log->trace('Model ['.$model.'] does not exist');
      continue ;
    }
    echo '- '.$model."\n";
    $items = Item::collect($model);
    foreach ($items as $item) {
      try {
        $item->destroy();
      }
      catch (Exception $e) {
        $log->error($e);
      }
    }
    $dbo->getTable(T($model))->truncate();
  }
  
  $tables = array('core_popup_hit',
                  'core_acl', 'core_auth_attempt', 'core_user_information', 'core_privilege_user',
                  'core_smtp', 'core_base_email', Act::TABLE, 
                  Alert::TABLE_QUEUE, 
                  'core_discount_purchase', 'core_discount_order', 'core_discount_item',
                  'core_blacklist', 'core_stream', 'core_group_user', 'core_message_user', 'core_friend',
                  Item::TABLE_DRAFT, Item::TABLE_PING,
                  Captcha::TABLE, Item::TABLE_REVISION, Item::TABLE_LOG, Interaction::TABLE);
  foreach ($tables as $table) {
    if (!$dbo->hasTable($table, false)) {
      S('log')->trace('table unknown : '.$table);
      continue ;
    }
    $dbo->getTable($table)->truncate();
  }
  
  $metas = array(Meta::FIELD_ABUSES, Meta::FIELD_COMMENTS, 
                 Meta::FIELD_DISPLAYS,
                 Meta::FIELD_CLUSTER, Meta::FIELD_VISIT , Meta::FIELD_USER,
                 Meta::FIELD_SUBSCRIPTIONS,
                 Meta::FIELD_DISLIKES, Meta::FIELD_LIKES,
                 Meta::FIELD_RATING, Meta::FIELD_VALUATIONS);
  foreach ($metas as &$meta) {
    $meta = '`'.$meta.'`=0'; 
  }
  $dbo->exec('UPDATE '.Meta::TABLE.' SET '.join(', ', $metas));
  
  Alert::onAppReset();
  Document::onAppReset();
  
  foreach ($dbo->getTables() as $table) {
    $table->repair();
    $table->optimize();
  }
  
  try {
    S('app')->hook('onAppReset');
  }
  catch (Exception $e) {
    $log->error($e);
  }
  
  User::onAppReset();

  Cache::clean();
  Session::clean();
  Logger::clean();
  UploadManager::clean();
  
  echo '- Update : Cookie Names (maybe)' . "\n";
  echo '- Execute : item.clean' . "\n";
  
}
catch (Exception $e) { $err->add($e); }
