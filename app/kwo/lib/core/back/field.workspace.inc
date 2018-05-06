<?php

try { 

  $field = null;
  if ($req->field_id > 0) {
    $field = Field::getInstance((int) $req->field_id);
    $form = $field->getForm();
  }
  else {
    $form_id = $req->form_id ?: $req->getBranchValue('form_id');
    $form = Form::getInstance($form_id);
  }
  
  $args = array('form_id' => $form->getId());
  if ($field) {
    $args['field_id'] = $field->getId();
  }
  $fields = H::get('/back/core/field.home', $args);

  $ctx->form = $form;

  $ctx->fields = $fields;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
