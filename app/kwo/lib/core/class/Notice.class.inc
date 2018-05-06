<?php

class Notice extends ActiveRecord {

  const INBOX = 3;

  const FLAG_ACCOUNT = 1;
  const FLAG_DEFAULT = 2;
  const FLAG_SIGNUP = 4;

  public static $flags = array(self::FLAG_ACCOUNT => 'affichage compte',
                               self::FLAG_SIGNUP => 'affichage inscription',
                               self::FLAG_DEFAULT => 'association automatique');

  protected function onBeforeCreate(&$hash) {
    $id = $this->dbo->fetchValue('SELECT MAX(id) FROM '.T('notice'));
    if ($id >= 31) {
      throw new ActiveRecordException('id maximum ('.$id.') atteint');
    }
  }

  protected function onBeforeDestroy() {
    if ($this->getId() <= 4) {
      throw new ActiveRecordException('les 4 premiers avis ne peuvent être supprimés');
    }
  }

  protected function onAfterDestroy() {
    Model::removeFlag('user', 'notices', $this);
    $this->dbo->getTable(T('notice'))->resetAutoIncrement();
  }

}










  /*
  protected function onAdd(ActiveRecord $user) {
    $user->setMask($user->getNotices());
  }

  protected function onRemove(ActiveRecord $user) {
    $user->setMask($user->getNotices());
  }*/

   // TODO : Model::removeFlag($model, $flag, $column); ATTENTION decaler la valeur
    /*$this->dbo->query('UPDATE '.T('user')
                      . ' SET notices=notices-'.NH::asBit($this->getId()));
    $this->dbo->query('DELETE FROM social_notice_user'
                      .    ' WHERE notice_id='.$this->getId());*/