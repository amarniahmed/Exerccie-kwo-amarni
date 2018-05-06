<?php

class Admin extends ActiveRecord
{
  const FLAG_RICHTEXT = 1;
  const FLAG_AUTOLIST = 2;

  public static $flags = array(self::FLAG_RICHTEXT => 'édition HTML',
                               self::FLAG_AUTOLIST => 'listing auto');

  protected function onBeforeDestroy() {
    if ($this->getId() == 1) {
      S('log')->error('SuperAdmin can not be destroyed', __METHOD__);
      throw new Exception('SuperAdmin can not be deleted');
    }
  }

  public function getLogin() { return $this->login; }

  //  public function getRootPath() { return DOC_PATH; }

  public function isSuperAdmin() { return $this->policy_id == 1; }

  //  private $policy = null;

  protected static $actions = null;

  public function hasRight($action) {
    if ($this->getId() == 1) return true;
    //    $extension = Extension::id($extension);
    //    $this->policy = $this->policy ?: $this->getPolicy();
    if (self::$actions === null) {
      $policy = $this->getPolicy();
      self::$actions = SH::asSet($policy->actions);
      //      S('log')->debug(self::$actions);
    }
    if (empty(self::$actions)) return false;
    if (strpos($action, '?') > 1) {
      list ($action, ) = explode('?', $action);
    }
    return in_array($action, self::$actions);
  }

  private static $url = null;

  public static function url() {
    if (self::$url === null) {
      self::$url = P('back.url') ?: 'back';
    }
    self::$url = trim(self::$url, '/');
    return self::$url;
  }

}