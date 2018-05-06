<?php

class Favorite extends ActiveRecord {

  const FLAG_ALERT = 1;
  const FLAG_SENT = 2;

  public static $flags = array(self::FLAG_ALERT => 'alerte',
                               self::FLAG_SENT => 'envoyé');

  public function sendTo($recipients) {
  }

  public function getName($locale=null) {
    return empty($this->name) ? $this->getItem()->getName(null) : $this->name;
  }

  public static function save($item, $user, $opts) { 
    if (!$item->isAvailable()) {
      S('log')->error('item not available', __METHOD__);
      throw new Exception(l('error.item.unknown'));
    }
    $attributes = array('model_id' => $item->getModelId(),
                        'record_id' => $item->getId(),
                        'user_id' => $user->getId()); 
    $id = Item::exists('favorite', $attributes); 
    if (isset($opts['comment'])) {
      $comment = SH::clean($opts['comment'], 
                                   array('strict' => true));
      $attributes['comment'] = $opts['comment'];
    }
    try { 
      $favorite = Favorite::getInstance($id > 0 ? $id : true);
      $favorite->commit($attributes);
      if ($id < 1) {
        $favorite->setFlag(self::FLAG_ALERT);
        $favorite->setUser($user);
        $favorite->track(Interaction::TYPE_FAVORITE);
        if (method_exists($item, 'onFavorite')) {
          $item->onFavorite($favorite);
        }
      }
    }
    catch (Exception $e) { S('log')->error($e, __METHOD__); }
    return $favorite;
  }

}
