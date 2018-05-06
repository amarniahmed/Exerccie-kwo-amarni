<?php

try {

  $form = Form::getInstance((int) $req->getBranchValue('form_id'));

  $fields = array();
  foreach ($form->getFields() as $field) {
    $fields[$field->getId()] = $field->code;
  }

  $locales = $dbo->asSet('SELECT DISTINCT locale'
                         .' FROM '.T('submission')
                         .' WHERE form_id='.$form->getId());

  $ctx->fields = $fields;
  $ctx->locales = $locales;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
