<?php

class Interaction
{
  const TABLE = 'core_item_interaction';

  const TYPE_DISPLAY = 1;
  const TYPE_REDIRECT = 2;
  const TYPE_DOWNLOAD = 3;
  const TYPE_EVALUATE = 4;
  const TYPE_VOTE = 5;
  const TYPE_LIKE = 6;
  const TYPE_DISLIKE = 7;
  const TYPE_PLAY = 8;
  const TYPE_SHARE = 9;
  const TYPE_FAVORITE = 10;
  const TYPE_FAN = 11;
  const TYPE_ABUSE = 12;
  const TYPE_SUBSCRIBE = 13;
  const TYPE_COMMENT = 14;
  const TYPE_CLIC = 15;
  const TYPE_SUBMIT = 16;
  const TYPE_XXX = 17;
  const TYPE_AUTH = 18;
  const TYPE_INVITE = 19;
  const TYPE_CREATION = 20;
  const TYPE_PRINT = 21;
  const TYPE_PURCHASE = 22;
  const TYPE_PAY = 23;
  const TYPE_CUSTOMIZE = 24;

  const STATUS_OFFLINE = 0;
  const STATUS_ONLINE = 1;

  public static $names = array(self::TYPE_DISPLAY => 'visualisation',
                               self::TYPE_REDIRECT => 'redirection',
                               self::TYPE_DOWNLOAD => 'téléchargement',
                               self::TYPE_EVALUATE => 'évaluation',
                               self::TYPE_VOTE => 'vote',
                               self::TYPE_LIKE => 'j’aime',
                               self::TYPE_DISLIKE => 'je n’aime pas',
                               self::TYPE_PLAY => 'lecture',
                               self::TYPE_SHARE => 'partage',
                               self::TYPE_FAVORITE => 'favoris',
                               self::TYPE_FAN => 'fan',
                               self::TYPE_ABUSE => 'abus',
                               self::TYPE_SUBSCRIBE => 'inscription',
                               self::TYPE_COMMENT => 'commentaire',
                               self::TYPE_CLIC => 'clic',
                               self::TYPE_SUBMIT => 'soumission',
                               self::TYPE_XXX => 'xxx',
                               self::TYPE_AUTH => 'authentification',
                               self::TYPE_INVITE => 'invitation',
                               self::TYPE_CREATION => 'création',
                               self::TYPE_PRINT => 'impression',
                               self::TYPE_PURCHASE => 'achat',
                               self::TYPE_PAY => 'règlement',
                               self::TYPE_CUSTOMIZE => 'personnalisation');

  public static $type2meta = array(self::TYPE_ABUSE => Meta::FIELD_ABUSES,
                                   self::TYPE_COMMENT => Meta::FIELD_COMMENTS,
                                   self::TYPE_DISPLAY => Meta::FIELD_DISPLAYS,
                                   self::TYPE_DOWNLOAD => Meta::FIELD_DOWNLOADS,
                                   self::TYPE_FAVORITE => Meta::FIELD_FAVORITES,
                                   self::TYPE_EVALUATE => Meta::FIELD_VALUATIONS,
                                   self::TYPE_SHARE => Meta::FIELD_SHARES,
                                   self::TYPE_SUBSCRIBE => Meta::FIELD_SUBSCRIPTIONS,
                                   self::TYPE_DISLIKE => Meta::FIELD_DISLIKES,
                                   self::TYPE_LIKE => Meta::FIELD_LIKES);

  public static function track($item, $type=Interaction::TYPE_DISPLAY, $opts=null) {
    $dbo = S('dbo');
    $req = S('req');
    $opts = (array) $opts;
    $visit_id = (int) ($opts['visit_id'] ? $opts['visit_id'] : $req->getVisitId());
    if ($visit_id < 1) return false;
    $arr = $dbo->fetchArray('SELECT medium, source'
                            .' FROM '.Visit::TABLE
                            .' WHERE id='.$visit_id);
    $medium = (int) $arr[0];
    $source = (int) $arr[1];
    if ($item instanceof ActiveRecord) {
      $model_id = $item->getModelId();
      $record_id = $item->getId();
    }
    elseif (is_array($item)) {
      $model_id = (int) $item[0];
      $record_id = (int) $item[1];
    }
    if ($model_id < 1 || $record_id < 1) {
      S('log')->error('invalid item', __METHOD__);
      return false;
    }
    $table = $dbo->getTable(self::TABLE);
    $row = array('model_id' => $model_id,
                 'record_id' => $record_id,
                 'type' => $type,
                 'app_id' => $req->getAppId(),
                 'medium' => $medium,
                 'source' => $source,
                 'visit_id' => $visit_id,
                 'created_on' => DH::today()); 
    if ($table->hasRow($row)) return false;
    $row['status'] = (int) $opts['status'];
    $row['value'] = (int) $opts['value'];
    $row['origin'] = Tracker::str2id($opts['origin'], Tracker::EXPR_ORIGIN);
    $row['user_id'] = (int) $opts['user_id']; 
    if ($req->isAuthenticated() && $row['user_id'] < 1) {
      $row['user_id'] = $req->getUserId();
      try {
        $user = User::getInstance($row['user_id']);
        $user->setFlag(User::FLAG_INTERACTION);
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    $dbo->insert(self::TABLE, $row);
    $affected = $dbo->affectedRows();
    if ($affected >= 1) {
      try {
        $meta = self::$type2meta[$type];
        if (is_array($item)) {
          $item = Item::getInstance($model_id, $record_id);
        }
        if (!empty($meta)) {
          $item->incrementMeta($meta);
        }
        $item->onTrack($type, $opts);
        $item->incrementMeta(Meta::FIELD_INTERACTIONS);
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    return $affected;
  }

  public static function count($item, $type) {
    return (int) S('dbo')->fetchValue('SELECT COUNT(*)'
                                      .' FROM '.self::TABLE
                                      .' WHERE model_id='.$item->getModelId()
                                      .  ' AND record_id='.$item->getId()
                                      .  ' AND type='.$type);
  }

}