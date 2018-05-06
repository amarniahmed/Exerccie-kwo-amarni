<?php

class Privilege extends ActiveRecord {

  const ADMINISTRATOR = 1;
  const EDITOR = 2;
  const AUTHOR = 3;
  const CONTRIBUTOR = 4;
  const MODERATOR = 5;
  const SUBSCRIBER = 6;
  const PREMIUM = 7;

  const FLAG_MIDDLE = 1;

  public static $flags = array(self::FLAG_MIDDLE => 'accès middle');

  protected function onBeforeCreate(&$attrs) {
    $id = $this->dbo->fetchValue('SELECT MAX(id) FROM '.T('privilege'));
    if ($id >= 31) {
      throw new ActiveRecordException('id maximum ('.$id.') atteint');
    }
  }

  protected function onBeforeDestroy() {
    if ($this->getId() <= 7) {
      throw new ActiveRecordException('les privilèges (1) à (7) ne peuvent être supprimés');
    }
  }

  protected function onAfterDestroy() {
    Model::removeFlag('user', 'privileges', $this);
    $this->dbo->query('DELETE FROM '.User::TABLE_PRIVILEGE
                      .    ' WHERE privilege_id = '.$this->getId());
    $this->dbo->getTable(T('privilege'))->resetAutoIncrement();
    ConfigManager::DBCacheBuild();
  }

  protected function onAfterCommit($attrs) {
    ConfigManager::DBCacheBuild();
  }

  public function grantTo($user, $opts=null) {
    $user = $user instanceof User ? $user : User::getInstance($user);
    $privilege_id = $this->getId();
    $h = $this->dbo->fetchHash('SELECT * FROM '.User::TABLE_PRIVILEGE
                               .      ' WHERE privilege_id = '.$privilege_id
                               .        ' AND user_id = '.$user->getId());
    if (is_array($h) && (empty($opts) || DH::isNull($h['available_from']))) {
      return ;
    }
    $row = array('privilege_id' => $privilege_id, 
                 'user_id' => $user->getId());
    if (is_array($opts)) {
      if (isset($opts['duration'])) {
        if (is_array($h) && !DH::isNull($h['available_to'])) {
          $opts['available_to'] = strtotime($h['available_to']) + ($opts['duration'] * 24 * 3600);
          $opts['available_to'] = date(DH::DATE_MYSQL, $opts['available_to']);
        }
        elseif (!isset($opts['available_from'])) {
          $opts['available_from'] = DH::now();
          $opts['available_to'] = time() + ($opts['duration'] * 24 * 3600);
          $opts['available_to'] = date(DH::DATE_MYSQL, $opts['available_to']);
        }
        unset($opts['duration']);
      }
      $row += $opts;
    }
    $filters = null;
    if (is_array($h)) {
      $filters = array('privilege_id' => $privilege_id, 
                       'user_id' => $user->getId());
    }
    $this->dbo->set(User::TABLE_PRIVILEGE, $row, $filters);
  }

  public static function asCache() {
    $set = S('dbo')->asSet('SELECT id FROM '.T('privilege')
                           .       ' WHERE (flags & '.self::FLAG_MIDDLE.') > 0');
    return $set;
  }

  public static function middle() {
    return $GLOBALS['middle'];
  }

}