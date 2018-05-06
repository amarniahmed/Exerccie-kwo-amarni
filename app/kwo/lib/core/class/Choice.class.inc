<?php

class Choice extends ActiveRecord {

  protected function onBeforeCreate(&$attrs) {
    if (empty($attrs['position'])) {
      $position = $this->dbo->fetchValue('SELECT MAX(position)'
                                         .' FROM '.T('choice')
                                         .' WHERE poll_id='.$attrs['poll_id']);
      $attrs['position'] = intval($position) + 1;
    }
  }

  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM '.Interaction::TABLE
                     .    ' WHERE value='.$this->getId()
                     .      ' AND type='.Interaction::TYPE_VOTE);
  }

}