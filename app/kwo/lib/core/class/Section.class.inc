<?php

class Section extends ActiveRecord {
  
  protected function onAfterCommit($attrs) {
    $pos = $this->dbo->fetchValue('SELECT MAX(position)'
                                  .' FROM '.T('section')
                                  .' WHERE faq_id='.$this->faq_id);
    $pos = $pos <= 1 ? 1 : $pos + 1;
    $this->dbo->exec('UPDATE '.T('section')
                     .' SET position='.$pos
                     .' WHERE id='.$this->getId());
  }
  
  protected function onAfterDestroy() {
    S('dbo')->exec('UPDATE '.T('component')
                   . ' SET section=0'
                   .' WHERE section_id='.$this->getId());
  }

}