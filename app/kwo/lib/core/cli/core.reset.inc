<?php

try {

  if (!$req->confirm === 'yes') return ;

  $skip = array(T('admin'), T('currency'), T('property'), T('resource'),
                T('node'), T('tag'), T('timezone'),
                T('field'), T('paragraph'), T('component'),
                T('chapter'), T('choice'), T('slide'),
                T('section'),
                T('locale'), T('language'), T('region'),
                Page::TABLE_BLOCK,
                T('country'), Country::TABLE_IP,
                Carrier::TABLE_RATES,
                Discount::TABLE_ACTION, Discount::TABLE_CONDITION,
                Item::TABLE_PROPERTY, Item::TABLE_NODE,
                Item::TABLE_SEO, Item::TABLE_TAG,
                Meta::TABLE);

  $tables = $dbo->getTables('core_*');

  foreach ($tables as $table) {
    $name = $table->getName();
    if ($table->hasField('app')) {
      $skip[] = $name;
    }
  }

  foreach ($tables as $table) {
    if (in_array($table->getName(), $skip)) continue ;
    $table->truncate();
  }

  Meta::onAppReset();
  User::onAppReset();

}
catch (Exception $e) { $err->add($e); }
