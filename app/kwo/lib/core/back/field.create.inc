<?php

try { 

  //  $req->debug();

  $form_id = (int) $ctx->getBranchAttribute('form_id');
  $form = Form::getInstance($form_id);
  
  $position = $req->position ?: 1;

  $field = Field::getInstance(); 
  $field->commit(array('name' => 'champ '.Field::$type_names[$req->type],
                       'position' => (int) $position,
                       'type' => (int) $req->type,
                       'form_id' => $form->getId(),
                       'code' => 'code-tmp-'.$form->getId()));
  $field->commit(array('code' => 'code-'.$field->getId()));
  
  $dbo->exec('UPDATE '.T('field')
             .' SET position=position+1'
             .' WHERE form_id='.$field->form_id
             .  ' AND position>='.$field->position
             .  ' AND id<>'.$field->id);
  
  $ctx->field_id = $field->getId();

}
catch (Exception $e) { $err->add($e); }
