<?php

class Pack extends Activerecord {

  public function asEditHash() {
    $hash = array();
    $purchasables = array();
    foreach(Model::all() as $model) {
      $instance = Item::getInstance($model);
      if($instance instanceof Purchasable) {
        $purchasables[] = $model;
      }
    }
    $hash['purchasable_models'] = implode(',', $purchasables).',';
    return $hash;
  }

}