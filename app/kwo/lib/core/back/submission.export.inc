<?php

try { 

  $form_id = (int) $req->getBranchValue('form_id');
  $form = Form::getInstance($form_id);

  $limit = '';
  if ($req->limit_value >= 1) {
    $limit = ' LIMIT '.intval($req->limit_value);
  }

  $bindings = array();

  $columns = 's.*, s.submitted_at';
  $from = T('submission').' AS s';
  $where = 's.form_id='.$form->getId();
  $havings = array();
  
  if ($req->status == 1) {
    $where .= ' AND s.status > 0';
  }

  if (strlen($req->submitted_from) > 5) {
    $where .= ' AND s.submitted_at >= "'.DH::toDb($req->submitted_from).'"';
  }

  if (strlen($req->submitted_to) > 5) {
    $where .= ' AND s.submitted_at <= "'.DH::toDb($req->submitted_to).'"';
  }

  if (!empty($req->flags)) {
    $flag = array_sum($req->flags);
    $where .= ' AND (s.flags & '.$flag.') >= '.$flag;
  }

  if (!empty($req->locales)) {
    $locale = array_sum($req->locales);
    $where .= ' AND (s.locale & '.$locale.') > 0';
  }

  $filters = (array) $req->filters;
  foreach ($filters as $id => $value) {
    if (empty($value)) continue ;
    $t = 'f'.$id;
    $from .= ' JOIN '.Submission::TABLE_FIELDS.' AS '.$t
              .' ON (s.id='.$t.'.submission_id AND '.$t.'.field_id='.$id.')';
    $columns .= ', '.$t.'.value AS v'.$id;
    if (strpos($value, '%') === false) {
      $havings[] = 'v'.$id.'=:v'.$id.':';
      $bindings[':v'.$id.':'] = $value;
    }
    else {
      $havings[] = 'v'.$id.' LIKE "#v'.$id.'#"';
      $bindings['#v'.$id.'#'] = $value;
    }
  }
  
  $having = '';
  if (count($havings) > 0) {
    $having = ' HAVING '.join(' AND ', $havings);
  }

  $rs = $dbo->query('SELECT '.$columns
                    .' FROM '.$from
                    .' WHERE '.$where
                    .$having
                    .' ORDER BY s.id DESC'
                    .$limit,
                    $bindings); 
  
  $fields = $form->getFieldCodes(); 
 
  $columns = array_merge(array('submission_id'), 
                         $fields,
                         array('created_at'));

  $opts = array('columns' => $columns,
                'filename' => $form->code.'-'.DH::today());
  $csv = CsvWriter::getInstance($opts);

  if ($rs === false || $rs->numRows() < 1) return ;

  while ($row = $rs->getNext()) {
    $line = array($row['id']);
    $h = $dbo->asHash('SELECT field_id, value'
                      .' FROM '.Submission::TABLE_FIELDS
                      .' WHERE submission_id='.$row['id']);
    foreach ($fields as $field_id => $code) {
      $line[] = (string) $h[$field_id];
    }
    $line[] = $row['submitted_at'];
    $csv->append($line);
  }

}
catch (Exception $e) { $err->add($e); }

