<?php

try {

  $finder = Finder::getInstanceByModel(MODEL);
  $items = $finder->getEntireCollection(); 
  
  $dir = $req->direction;
  $sibling_id = 0;
  foreach ($items as $item) {
    if ($sibling_id == $req->id) {
      $sibling_id = $item->id;
      break ;
    }
    if ($item->id == $req->id && $dir == 'prev') break ;
    $sibling_id = $item->id;
  }

  $ctx->sibling_id = $sibling_id == $req->id ? 0 : $sibling_id;
  

}
catch (Exception $e) { $err->add($e); }



/*  $params = $req->getAttributes(); 
  $params['limit'] = -1;
  $finder = Finder::getInstance($params);
  $sql = $finder->getCollection()->asSql();

  if ($req->direction == 'prev') {
    $operator = '>';
    $order = 'ASC';
  }
  else {
    $operator = '<';
    $order = 'DESC';
  } 

  $id = (int) $dbo->fetchValue('SELECT t.id'
                               .' FROM ('.$sql.') AS t'
                               .' WHERE t.id '.$operator.' '.$req->id
                               .' ORDER BY t.id '.$order
                               .' LIMIT 1');

  $ctx->sibling_id = $id; */
