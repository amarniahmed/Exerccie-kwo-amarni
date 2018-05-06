<?php

class Paper extends ActiveRecord {

  protected function onBeforeCommit(&$hash) {
    if (isset($hash['content']) && strlen($hash['content']) >= 3) {
      $hash['content'] = SH::tidy($hash['content']);
    }
  }

  public function asDocumentHash($locale) {
    $hash = parent::asDocumentHash($locale);
    $paragraphes = $this->getParagraphs();
    foreach ($paragraphes as $paragraph) {
      $hash['content'] .= $paragraph->name.' '.$paragraph->content;
    }
    return $hash;
  }

}