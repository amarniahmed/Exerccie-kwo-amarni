<?php

class Meta {

  const TABLE = 'core_item_meta';

  const BIT_FAVORITE = 1;
  const BIT_DESTROYED = 2;
  const BIT_HIDDEN = 4;
  const BIT_LOCKED = 8;
  const BIT_PINNED = 16;
  const BIT_PROTECTED = 32;
  const BIT_ETERNAL = 64;
  const BIT_NEW = 128;
  const BIT_ALERT = 256;
  const BIT_CLOSED = 512;
  const BIT_RESOURCES = 1024;
  const BIT_TWEETED = 2048;
  const BIT_PROMOTED = 4096;
  const BIT_PINGED = 8192;
  const BIT_FACEBOOKED = 16384;
  const BIT_SPAM = 32768;

  const FIELD_ABUSES = 'abuses';
  const FIELD_ADMIN = 'admin';
  const FIELD_BITS = 'bits';
  const FIELD_CHILD = 'child';
  const FIELD_CLUSTER = 'cluster';
  const FIELD_COMMENTS = 'comments';
  const FIELD_CONVERSIONS = 'conversions';
  const FIELD_CREATED_AT = 'created_at';
  const FIELD_DISLIKES = 'dislikes';
  const FIELD_DISPLAYS = 'displays';
  const FIELD_DOWNLOADS = 'downloads';
  const FIELD_FAVORITES = 'favorites';
  const FIELD_FLAGS = 'flags';
  const FIELD_INTERACTIONS = 'interactions';
  const FIELD_INTERESTS = 'interests';
  const FIELD_LATITUDE = 'latitude';
  const FIELD_LIKES = 'likes';
  const FIELD_LOCALES = 'locales';
  const FIELD_LONGITUDE = 'longitude';
  const FIELD_MODEL = 'model';
  const FIELD_NODE = 'node';
  const FIELD_PARENT = 'parent';
  const FIELD_RATING = 'rating';
  const FIELD_RECORD = 'record';
  const FIELD_REVISION = 'revision';
  const FIELD_SHARES = 'shares';
  const FIELD_STATE = 'state';
  const FIELD_STATUS = 'status';
  const FIELD_SUBSCRIPTIONS = 'subscriptions';
  const FIELD_UPDATED_AT = 'updated_at';
  const FIELD_USER = 'user';
  const FIELD_VALUATIONS = 'valuations';
  const FIELD_VISIT = 'visit';

  public static function decrement($item, $key, $value=1) {
    return self::increment($item, $key, $value, '-');
  }

  public static function increment($item, $key, $value=1, $sign='+') {
    if ($item instanceof ActiveRecord) {
      $model_id = (int) $item->getModelId();
      $record_id = (int) $item->getId();
    }
    elseif (isset($item['model'])) {
      $model_id = (int) $item['model_id'];
      $record_id = (int) $item['record_id'];
    }
    else {
      $model_id = (int) $item[0];
      $record_id = (int) $item[1];
    }
    if (!Model::hasMetas($model_id)) return false;
    $dbo = S('dbo');
    $filters = array(self::FIELD_MODEL => $model_id,
                     self::FIELD_RECORD => $record_id);
    $ret = $dbo->update(self::TABLE,
                        array('`'.$key.'`=`'.$key.'`'.$sign.'1'),
                        array('filters' => $filters));
    if ($ret < 1) {
      $value = (int) ($sign === '+');
      $dbo->insert(self::TABLE,
                   array(self::FIELD_MODEL => $model_id,
                         self::FIELD_RECORD => $record_id,
                         $key => $value));
    }
  }

  public static function get($item, $key=null) {
    if ($item instanceof ActiveRecord) {
      $model_id = (int) $item->getModelId();
      $record_id = (int) $item->getId();
    }
    elseif (isset($item['model_id'])) {
      $model_id = (int) $item['model_id'];
      $record_id = (int) $item['record_id'];
    }
    else {
      $model_id = (int) $item[0];
      $record_id = (int) $item[1];
    }
    if (!Model::hasMetas($model_id)) return false;
    $args = func_get_args();
    if (!is_string($key)) {
      if (empty($key)) $cols = '*';
      else {
        if (count($args) > 2) array_shift($args);
        else $args = $key;
        $cols = '`'.join('`, `', $args).'`';
      }
      $h = S('dbo')->fetchHash('SELECT '.$cols
                               .' FROM '.self::TABLE
                               .' WHERE '.self::FIELD_MODEL.'='.$model_id
                               .  ' AND '.self::FIELD_RECORD.'='.$record_id);
      return $h ?: array();
    }
    return S('dbo')->fetchValue('SELECT '.$key
                                .' FROM '.self::TABLE
                                .' WHERE '.self::FIELD_MODEL.'='.$model_id
                                .  ' AND '.self::FIELD_RECORD.'='.$record_id);
  }

  public static function set($item, $key, $value) {
    if ($item instanceof ActiveRecord) {
      $model_id = (int) $item->getModelId();
      $record_id = (int) $item->getId();
    }
    elseif (isset($item['model_id'])) {
      $model_id = (int) $item['model_id'];
      $record_id = (int) $item['record_id'];
    }
    else {
      $model_id = (int) $item[0];
      $record_id = (int) $item[1];
    }
    if (!Model::hasMetas($model_id)) return false;
    $dbo = S('dbo');
    $filters = array(self::FIELD_MODEL => $model_id,
                     self::FIELD_RECORD => $record_id);
    $ret = $dbo->update(self::TABLE,
                        array($key => $value),
                        array('filters' => $filters,
                              'modifiers' => array('ignore'))); 
    if ($dbo->matchedRows() < 1) {
      $dbo->insert(self::TABLE,
                   array(self::FIELD_MODEL => $model_id,
                         self::FIELD_RECORD => $record_id,
                         $key => $value)); 
      $ret = $dbo->affectedRows(); 
    }
    return $ret >= 1;
  }

  public static function onAppReset() {
    $metas = array(self::FIELD_ABUSES, self::FIELD_COMMENTS, 
                   self::FIELD_DISPLAYS,
                   self::FIELD_CLUSTER, self::FIELD_VISIT , self::FIELD_USER,
                   self::FIELD_SUBSCRIPTIONS,
                   self::FIELD_DISLIKES, self::FIELD_LIKES,
                   self::FIELD_RATING, self::FIELD_VALUATIONS);
    foreach ($metas as &$meta) {
      $meta = '`'.$meta.'`=0';
    }
    S('dbo')->exec('UPDATE '.self::TABLE.' SET '.join(', ', $metas));
  }

}

