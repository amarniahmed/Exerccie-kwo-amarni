<?php

/***
interest_id = 1 : 1 - 1 = 0 -> 2^0 = 1
interest_id = 3 : 2 - 1 = 1 -> 2^1 = 2
interest_id = 3 : 3 - 1 = 2 -> 2^2 = 4

y = 1 << (x-1)
4 = 1 << (3-1)
http://www.guardiani.us/index.php/MySQL_bitwise_operations
SELECT number,descr,resources FROM servdef WHERE resources IN (1 | 2);
SELECT perms FROM acl WHERE perms & @CAN_READ 

***/

class Interest extends ActiveRecord {

  protected function onBeforeCreate(&$hash) {
    $id = $this->dbo->fetchValue('SELECT MAX(id) FROM '.T('interest'));
    if ($id >= 31) {
      throw new ActiveRecordException('id maximum ('.$id.') atteint');
    }
  }

  protected function onAfterDestroy() {
    /* TODO : mettre à jour colonne interest de Meta */
    $this->dbo->getTable(T('interest'))->resetAutoIncrement();
  }

}