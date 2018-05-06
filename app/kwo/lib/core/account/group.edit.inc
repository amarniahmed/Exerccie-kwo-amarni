<?php

try {

  $group = Group::getInstance($req->id ? $req->id : true);

  $elts = array(array('elt' => 'hidden',
                      'name' => 'id',
                      'value' => $group->id),
                array('elt' => 'text',
                      'name' => 'name',
                      'value' => $group->name,
                      'required' => true,
                      'maxlength' => 110,
                      'label' => l('group.label')),
                array('elt' => 'text',
                      'name' => 'description',
                      'value' => $group->description,
                      'rows' => 5,
                      'label' => l('description')));

  $events = array('cancel' => array('label' => l('cancel'),
                                    'action' => 'Kwo.Group.onCancel(this)'),
                  'submit' => array('label' => l('create'),
                                    'action' => 'Kwo.Group.onSave(this)'));

  if ($req->id >= 1) {
    $events['delete'] = array('label' => l('delete'),
                              'action' => 'Kwo.Group.onDelete(this)',
                              'confirm' => l('confirmation'));
    $events['submit']['label'] = l('create');
  }

  $opts = array('layout' => 'hbox',
                'wide' => true,
                'elts' => $elts,
                'events' => $events);

  $ctx->form = Elt::form($opts);
  $ctx->group = $group;

  $res->setMenu('groups');
  $res->setTitle(l('account.groups'));

}
catch (Exception $e) { $err->add($e); }
