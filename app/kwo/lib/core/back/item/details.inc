<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);

  $foreign_key = Model::name($item).'_id';

  $infos = array();
  if (Model::hasFields($item, array('model_id', 'record_id')) &&
      $item->model_id >= 1 && $item->record_id >= 1) {
    $h = array();
    try {
      $i = $item->getItem();
      $h['ID'] = $i->getId();
      $h[l('type')] = $i->getModel();
      $h[l('intitulé')] = $i->getName();
      $h[l('date mise à jour')] = $i->getMeta(Meta::FIELD_UPDATED_AT);
      $h[l('date création')] = $i->getMeta(Meta::FIELD_CREATED_AT);
    }
    catch (Exception $e) {
      $h[l('intitulé')] = '('.Model::name($item->model_id).' ) ?';
    }
    $infos += array(l('objet associé') => $h);
  }

  $metas = $item->getMetas();

  $visit = null;
  try {
    $visit = $item->getVisit();
    $infos += array(l('visite') => Visit::link($visit));
  }
  catch (Exception $e) {}
  $visit = null;
  try {
    $user = $item->getUser();
    $infos += array(l('createur') => Item::edit($user));
  }
  catch (Exception $e) {}


  $update = $metas[Meta::FIELD_UPDATED_AT];
  if (Model::parameter($item, 'revision') === true) {
    $n = $dbo->fetchValue('SELECT COUNT(*)'
                          .' FROM '.Item::TABLE_REVISION
                          .' WHERE model_id='.$item->getModelId()
                          .  ' AND record_id='.$item->getId());
    if ($n > 0) {
      $update .= '<br/><a href="javascript:void(0)"'
                 .      ' onclick="Kwo.F(\''.$item->getModel().'\').onExec(\'revisions\', this, {container: $(this).up(\'.deck-pane\')})"'
                 .      ' data-id="'.$item->getId().'">'.l('voir les modifications').'</a>';
    }
  }

  $infos = array('ID' => $item->getId(),
                 l('création') => $metas[Meta::FIELD_CREATED_AT],
                 l('mise à jour') => $update) + $infos;

  $belongs_to = (array) Model::parameter($item->getModel(), 'belongs_to');
  foreach ($belongs_to as $name => $h) {
    $id = $item->getAttribute($name.'_id');
    if ($id < 1) continue ;
    if (!Item::exists($name, $id)) {
      $log->trace($name.'_id('.$id.') not found');
      continue ;
    }
    $o = Item::getInstance($name, $item->getAttribute($name.'_id'));
    $infos[c(l('liaison')).' : '.ucfirst(Model::label($o))] = $o->getName().' ('.$o->getId().')';
  }

  $has_many = (array) Model::parameter($item->getModel(), 'has_many');

  foreach ($has_many as $name => $h) {
    if ($h['through']) continue ;
    if (!Model::hasField($name, $foreign_key)) continue ;
    $n = $dbo->fetchValue('SELECT COUNT(*)'
                          .' FROM '.T($name)
                          .' WHERE '.$foreign_key.'='.$item->getId());
    $infos[c(l('élément(s) associé(s)').' ['.$name.']')] = $n;
  }

  $ctx->infos = $infos;

  if (method_exists($item, 'getDetails')) {
    $h = $item->getDetails();
    foreach ($h as $key => $value) {
      $ctx->infos[$key] = $value;
    }
  }


  $res->useXml();

}
catch (Exception $e) { $err->add($e); }