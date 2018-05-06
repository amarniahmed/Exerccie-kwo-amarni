<?php

try {

  $set = $dbo->asSet('SELECT group_id FROM '.Group::TABLE_GROUP_USER
                     .' WHERE user_id='.$user->getId());

  $opts = array('filters' => array('id' => $set),
                'order' => array('name'));
  $groups = Group::collect($opts);
  
  $ctx->group_id = (int) $req->group_id;
  $ctx->groups = $groups;

  $res->setMenu('groups');
  $res->setTitle(l('account.groups'));

}
catch (Exception $e) { $err->add($e); }
