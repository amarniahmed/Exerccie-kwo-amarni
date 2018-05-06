<?php

class Glossary extends ActiveRecord {

  public function getLetters() {
    return $this->dbo->query('SELECT letter, COUNT(letter)'
                             .' FROM '.T('term')
                             .' WHERE glossary_id='.$this->getId()
                             .' GROUP BY letter'
                             .' ORDER BY letter')->asHash();
  }

}