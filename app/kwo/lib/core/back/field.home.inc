<?php

try {

  if ($req->field_id > 0) {
    $field = Field::getInstance((int) $req->field_id);
    $form = $field->getForm();
  }
  else {
    $form_id = $req->form_id ?: $req->getBranchValue('form_id');
    $form = Form::getInstance((int) $form_id);
  }

  $ctx->form = $form;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
