<?php

class Faq extends ActiveRecord {

  const FLAG_CONTACT = 1;

  public static $flags = array(self::FLAG_CONTACT => 'dans contact');

  protected function onAfterCommit($hash) {
    if (!isset($hash['position'])) return ;
    $n = $this->dbo->fetchValue('SELECT COUNT(id)'
                                .' FROM '.T('faq')
                                .' WHERE position='.$this->position
                                .' GROUP BY position');
    if ($n <= 1) return ;
    $this->dbo->exec('UPDATE '.T('faq')
                     .' SET position=position+1'
                     .' WHERE id<>'.$this->getId()
                     .  ' AND position>='.$this->position);
  }
  
  public function asShortCodeContent($args=null) {
    $args = array('faq' => $this,
                  'components' => $this->getComponents());   
    $content = Template::render('faq', $args);                   
    if (P('app.shortcodes') === true) {
      return ShortCode::replace($content);
    }
    return $content;
  }

  public function asRelationRow($item, $row) { 
    $row = $item->getName();
    if ($item->isA('component')) {
      if ($item->section_id > 0) $row .= ' *';
    }
    return $row;
  }

}