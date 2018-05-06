<?php

class Component extends ActiveRecord {

  protected function onAfterCommit($attrs) {
    if ($this->position > 0) {
      $n = $this->dbo->fetchValue('SELECT COUNT(position)'
                                  .' FROM '.T('component')
                                  .' WHERE faq_id='.$this->faq_id
                                  .  ' AND id<>'.$this->getId());
      if ($n > 0) {
        $this->dbo->exec('UPDATE '.T('component')
                         .' SET position=position+1'
                         .' WHERE id<>'.$this->getId()
                         .  ' AND faq_id='.$this->faq_id
                         .  ' AND position>='.$this->position);
      }
    }
    else {
      $pos = $this->dbo->fetchValue('SELECT MAX(position)'
                                    .' FROM '.T('component')
                                    .' WHERE faq_id='.$this->faq_id);
      $pos = $pos <= 1 ? 1 : $pos + 1;
      $this->dbo->exec('UPDATE '.T('component')
                       .' SET position='.$pos
                       .' WHERE id='.$this->getId());
    }
  }

  public function asEditHash() {
    $faq_id = $this->faq_id ?: S('req')->getBranchValue('faq_id');
    $h = array();
    $h['sections'] = S('dbo')->asHash('SELECT id, name'
                                      .' FROM '.T('section')
                                      .' WHERE faq_id='.$faq_id
                                      .' ORDER BY name');
    return $h;
  }



  public function onPick($args) {
    $items = $args['items'];
    $filters = $items->getFilters(); 
    if ($items->getModel() === 'section') {
      if ($this->faq_id > 0) {
        $filters['faq_id'] = $this->faq_id;
      }
      else {
        $filters['faq_id'] = S('req')->getBranchValue('faq_id');
      }
    }
    $items->setFilters($filters);
  }

}