<?php

class Body extends ActiveRecord 
{
  protected function onBeforeCommit(&$hash) {
    if (isset($hash['content'])) {
      $hash['content'] = SH::tidy($hash['content'], 
                                      array('wrap' => 900));
    }
  }

  public function asUrl($locale=null) {
    return '/body?id=' . $this->id;
  }

}