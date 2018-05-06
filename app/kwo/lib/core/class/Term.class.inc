<?php

class Term extends ActiveRecord {
  
  public function asDocumentHash($locale) {
    $hash = parent::asDocumentHash($locale);
    $hash['name'] = $this->name;
    $hash['content'] = $this->content;
    
    return $hash;
  }
  
  protected function onBeforeCommit(&$hash) {
  if (!empty($hash['name']) && empty($hash['letter'])) {
      $name = strtolower($hash['name']);
      $hash['letter'] = preg_match('/^[a-z]/u', $name) 
                      ? $name[0]
                      : '0';
      $hash['letter'] = SH::stripAccents($hash['letter']);
    }
    if (!empty($hash['letter'])) {
      $hash['letter'] = strtolower($hash['letter']);
    }
  }

}