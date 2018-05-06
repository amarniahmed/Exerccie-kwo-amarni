<?php

class Chapter extends ActiveRecord {

  protected function onAlter() {
    parent::onAlter();
    $this->getStory()->setMeta(Meta::FIELD_UPDATED_AT, 
                               DH::now());
  }

}