<?php

try {

  $form = Form::getInstance((int) $req->id);

  $rs = $dbo->query('SELECT s.*, md.created_at'
                    .' FROM '.T('submission').' AS s,'
                    .     ' '.Meta::TABLE.' AS md'
                    .' WHERE md.'.Meta::FIELD_MODEL.'='.Model::id('submission')
                    .  ' AND md.'.Meta::FIELD_RECORD.'=id'
                    .  ' AND s.form_id='.$form->getId());

  $cols = array_keys($form->fields);

  $columns = array_merge(array('submission_id'), $cols, array('created_at'));

  $csv = CsvWriter::getInstance(array('columns' => $columns));

  if ($rs === false || $rs->numRows() < 1) return ;
  
  while ($row = $rs->getNext()) {
    $line = array($row['id']);
    $fields = AH::decode($row['fields']);
    foreach ($cols as $col) {
      $line[] = (string) $fields[$col];
    }
    $line[] = $row['created_at'];
    $csv->append($line);
  }

}
catch (Exception $e) { $err->add($e); }

