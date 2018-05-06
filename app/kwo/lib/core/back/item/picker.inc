<?php

try {

  $item = null;
  $models = SH::asSet($req->models);
  $model = $req->getAttribute('model') ?: $models[0];
  $extension = Model::parameter($model, 'extension');
  $name_field = 'name';
  if ($model == 'email') {
    $name_field = 'email';
  }
  $opts = array('order' => array($name_field),
                'limit' => 250,
                'page' => (int) $req->page);

  $filters = array();
  if ($req->hasAttribute('query', true)) {
    if (is_int($req->query)) {
      $filters['id'] = (int) $req->query;
    }
    else {
      if ($model === 'user') {
        $filters = array(array('|%%|'.$name_field => $req->query),
                         array('OR' => array('|%%|email' => $req->query)));
      }
      else {
        if (Model::hasField($model, 'code')) {
          $filters = array(array('|%%|'.$name_field => $req->query),
                           array('OR' => array('|%%|code' => $req->query)));
        }
        else {
          $filters['|%%|'.$name_field] = $req->query;
        }
      }
    }
  }

  if ($req->hasAttribute('item', true)) {
    $item = Item::getInstanceByKey($req->item);
    $relation = (array) Model::relation($item, $model);
    //d($relation);
    if (strpos($relation['through'], '_') > 1 && $relation['unique'] === true) {
      $through = $relation['through'];
      $table = $dbo->getTable($through);
      $field = $item->getModel().'_id';
      if ($table->hasField($model.'_id')) {
        $set = $dbo->asSet('SELECT '.$model.'_id'
                           .' FROM '.$table->getName()
                           .' WHERE '.$field.'='.$item->getId());
      }
      else {
        $set = $dbo->asSet('SELECT record_id'
                           .' FROM '.$table->getName()
                           .' WHERE '.$field.'='.$item->getId()
                           .  ' AND model_id='.Model::id($model));
      }
      $filters['|<>|id'] = $set;
    }
  }
  //  d($filters);

  $opts['filters'] = $filters;
  $items = Item::collect($model, $opts);

  $args = array('through' => $through,
                'items' => $items);
  if ($item) {
    $item->hook('onPick', $args);
  }
  else {
    Item::getInstance($req->caller_model)->hook('onPick', $args);
  }

  $ctx->items = $items;
  $ctx->model = $model;
  $ctx->models = $models;
  $ctx->query = $req->query;
  $ctx->selection = $req->selection;
  $ctx->title = l('sélection d’un élément');

  $path = WEB_PATH.'/'.$extension.'/templates/back/'.$model.'.picker.psp';
  if (file_exists($path)) {
    $template = $extension.':'.$model.'.picker';
  }
  else {
    $template = 'core:item.picker';
  }

  $res->useTemplate($template);

}
catch (Exception $e) { $err->add($e); }