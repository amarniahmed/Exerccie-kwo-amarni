<?php

class Space extends ActiveRecord {

  protected function onBeforeCreate(&$attrs) {
    $id = $this->dbo->fetchValue('SELECT MAX(id) FROM '.T('space'));
    if ($id >= 31) {
      throw new ActiveRecordException('id maximum ('.$id.') atteint');
    }
  }

  protected function onAfterDestroy() {
    Model::removeFlag('advert', 'spaces', $this);
    $this->dbo->getTable(T('space'))->resetAutoIncrement();
  }

}