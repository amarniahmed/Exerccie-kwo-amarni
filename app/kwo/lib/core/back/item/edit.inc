<?php

try {

  $item = null;

  if ($req->item) {
    $item = Item::getInstanceByKey($req->item);
    define(MODEL, $item->getModel());
  }

  if ($item === null) {
    $arg = true;
    if ($req->id >= 1) {
      $arg = (int) $req->id;
    }
    elseif (Model::isProfile(MODEL) && $req->getBranchValue('user_id') >= 1) {
      $id = $dbo->fetchValue('SELECT id'
                             .' FROM '.T(MODEL)
                             .' WHERE user_id='.$req->getBranchValue('user_id'));
      $arg = $id >= 1 ? (int) $id : true;
    }
    $item = Item::getInstance(MODEL, $arg);
  }

  $h = array();
  $branch = $req->getBranch();
  foreach ($branch as $key => $value) {
    $h[$key] = $value;
  }

  $label = Model::label($item);
  if ($item->isItem()) {
    $h = (array) $item->_nucleus;
    $h['is_loaded'] = true;
    $title = $item->getName();
    $title = strlen($title) > 60 ? substr($title, 0, 70) . '&hellip;' : $title;

    $h['fieldset_label'] .= $label;
    if (!is_numeric($title)) {
      $h['fieldset_label'] .= ' <small>( <b>'.$item->getId().'</b> )</small>';
    }
    $h['fieldset_label'] .= ' <b>'.$title.'</b>';
    $h['fieldset_label'] .= $item->hasData(Item::DATA_NOTE) ? ' *' : '';
    $h['_datas'] = $item->getDatas();
  }
  else {
    $h['is_loaded'] = false;
    $h['fieldset_label'] = l('nouvel élément : <b>((label))</b>', array('label' => ucfirst($label)));
  }

  $h['is_not_loaded'] = !$h['is_loaded'];

  if (method_exists($item, 'asEditHash')) {
    $tmp = $item->asEditHash();
    $tmp = is_array($tmp) ? $tmp : array();
    foreach ($tmp as $key => $value) {
      $h[$key] = $value;
    }
  }

//  $dialogs = $req->getDialogs();
/*  if (!(Model::isProfile(MODEL) && $req->getBranchValue('user_id') >= 1) &&
      !TreeManager::isNode($item) && $h['disable_home'] != true) {
    $h['fieldset_label'] .= ' <img src="'.PIX_URL.'/action/home-search.png"'
                              .' onclick="Kwo.F(\''.$item->getModel().'\').onHome(this);"'
                              .' onload="if (!$(this).inDialog()) { $(this).show(); }"'
                              .' style="display:none;"'
                              .' title="retour à liste" />';
  } */


  if (TreeManager::isTree($item) && $item->isItem()) {
    $columns = TreeManager::columns($item);
    $node_model = TreeManager::nodeModel($item);
    $h['columnview'] = Template::render('core:columns',
                                        array('columns' => $columns,
                                              'tree_id' => $item->getId(),
                                              'tree_model' => $item->getModel(),
                                              'node_model' => $node_model));
  }

  if (Model::hasField(MODEL, 'app') && empty($h['app']) &&
      !in_array(MODEL, array('parameter', 'pagelet', 'style', 'carrier',
                             'string', 'snippet', 'letter', 'notice'))) {
  }

  $ctx->setAttributes($h);
  $ctx->item = $item;

  $res->useXml(MODEL.'.edit');

}
catch (Exception $e) { $err->add($e); }