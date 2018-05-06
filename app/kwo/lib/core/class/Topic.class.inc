<?php

class Topic extends ActiveRecord 
{
  public function markAsEvolved() {
    parent::markAsEvolved();
    $this->getForum()->markAsEvolved();
  }
  
  public function getLocale() {
    return $this->getForum()->getLocale();
  }

  protected function onBeforeCommit(&$attrs) { 
    if (isset($attrs['name']) && strlen($attrs['name']) < 1) {
      throw new ActiveRecordException(l('error.fields.required.all'));
    }
    if (isset($attrs['content'])) { 
      if (strlen($attrs['content']) < 1) {
        throw new ActiveRecordException(l('error.fields.required.all'));
      }
      $attrs['content'] = SH::decorate($attrs['content']);
    }
  }

  public function asDocumentHash($locale) {
    $h = parent::asDocumentHash($locale);
    try {
      $h['name'] = $this->name;
      foreach ($this->getComments() as $comment) {
        $h['content'] .= $comment->content.' ';
        if (strlen($h['content']) > 10000) break ;
      }
      return $h;
    }
    catch (Exception $e) { return null; }
  }
  
  public function asSearchHash($locale) {
    $h = parent::asSearchHash($locale);
    $forum = $this->getForum();
    $h['name'] = $this->getForum()->name.' : '.$this->name;
    $h['description'] .= $this->content;
    try {
      foreach ($this->getComments() as $comment) {
        $h['content'] .= $comment->content.' ';
        if (strlen($h['content']) > 200) break ;
      }
      return $h;
    }
    catch (Exception $e) { return null; }
    return $h;
  }
  
  public function onRelationCreate($item) {
    if ($item->isA('comment') && $item->isOnline()) { 
      $this->getForum()->incrementMeta(Meta::FIELD_COMMENTS);
    }
  }
  
  public function onRelationDestroy($item) {
    if ($item->isA('comment') && $item->isOnline()) { 
      $this->getForum()->decrementMeta(Meta::FIELD_COMMENTS);
    }
  }
  
  public function getLocales() {
    return array($this->getForum()->locale);
  }
  
}
