<?php

try {

  if ($req->type == 'update') {
    if ($req->status_from == $req->status_to) {
      throw new Exception(c(l('les 2 statuts ne peuvent être identiques')));
    }
    if ($req->status_from < 1 || $req->status_to < 1) {
      throw new Exception(c(l('statuts invalides')));
    }
    $filters = array();
    if ($req->locale > 0) {
      $filters['locale'] = (int) $req->locale;
    }
    if ($req->status_from > 0) {
      $filters['status'] = (int) $req->status_from;
    }
    if ($req->model_id > 0) {
      $filters['model_id'] = (int) $req->model_id;
    }
    $opts = array('filters' => $filters);
    $translations = Translation::collect($opts);
    $status = (int) $req->status_to;
    $n = 0;
    foreach ($translations as $translation) {
      if ($translation->status == $status) continue ;
      $translation->setStatus($status);
      $n++;
    }
    $res->sendMessage(c(l('traductions mises à jour')).' : '.$n);
    return ;
  }

  $models = $req->model_id < 1 ? Translation::getLocalizedModels() : array(Model::name($req->model_id));
  $locales = Translation::locales();
  if ($req->locale > 0) {
    $locales = array(intval($req->locale));
  }
  $n = 0;
  $opts = array('priority' => (int) $req->priority);
  foreach ($models as $model) {
    $model_id = Model::id($model);
    $items = Item::collect($model);
    foreach ($items as $item) {
      $row = array('model_id' => $model_id,
                   'record_id' => $item->getId(),
                   'locale' => 0);
      $set = $dbo->asSet('SELECT locale'
                         .' FROM '.T('translation')
                         .' WHERE model_id='.$model_id
                         .  ' AND record_id='.$item->getId());
      foreach ($locales as $locale) {
        $locale = Locale::toNum($locale);
        $row['locale'] = $locale;
        if (Translation::exists($row)) continue ;
        Translation::add($item, $locale);
        $n++;
      }
    }
  }
  $res->sendMessage(c(l('demandes de traductions enregistrées')).' : '.$n);

}
catch (Exception $e) { $err->add($e); }