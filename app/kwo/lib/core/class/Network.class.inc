<?php

class Network {

  const TABLE = 'core_friend';
  const TABLE_BLACKLIST = 'core_blacklist';

  const STATUS_REMOVED = -1;
  const STATUS_ONLINE = 1;
  const STATUS_IN_GRAPH = 2;

  const NEO4J = false;

  public static function friends($user, $opts=null) {
    $opts = (array) $opts;
    $user = $user instanceof User ? $user->getId() : (int) $user;
    $friends = S('dbo')->asSet('SELECT friend_id FROM '.self::TABLE
                               .              ' WHERE user_id='.$user.' AND status>0'
                               .' UNION'
                               .' SELECT user_id FROM '.self::TABLE
                               .              ' WHERE friend_id='.$user.' AND status>0');
    if ($opts['collection'] === true) {
      $friends = User::collect(array('filters' => array('id' => $friends)));
    }
    return $friends;
  }

  public static function isBlacklisted($user, $culprit) {
    $user_id = (int) ($user instanceof ActiveRecord ? $user->getId() : $user);
    $culprit_id = (int) ($culprit instanceof ActiveRecord ? $culprit->getId() : $culprit);
    $table = S('dbo')->getTable(self::TABLE_BLACKLIST);
    return $table->hasRow(array('user_id' => $user_id,
                                'culprit_id' => $culprit_id));
  }

  public static function blacklist($user, $culprit, $cause=null) {
    if (self::isBlacklisted($user, $culprit)) return ;
    $user_id = (int) ($user instanceof ActiveRecord ? $user->getId() : $user);
    $culprit_id = (int) ($culprit instanceof ActiveRecord ? $culprit->getId() : $culprit);
    $row = array('user_id' => $user_id,
                 'culprit_id' => $culprit_id);
    if ($cause instanceof ActiveRecord) {
      $row['model_id'] = $cause->getModelId();
      $row['record_id'] = $cause->getId();
    }
    S('dbo')->insert(self::TABLE_BLACKLIST, $row,
                     array('modifiers' => 'ignore'));
  }

  public static function removeBlacklist($user, $culprit) {
    if (!self::isBlacklisted($user, $culprit)) return ;
    $user_id = (int) ($user instanceof ActiveRecord ? $user->getId() : $user);
    $culprit_id = (int) ($culprit instanceof ActiveRecord ? $culprit->getId() : $culprit);
    S('dbo')->exec('DELETE FROM '.self::TABLE_BLACKLIST
                   .' WHERE user_id='.$user_id
                   .  ' AND culprit_id='.$culprit_id);
  }

  public static function isConnected($user, $friend) {
    $user = $user instanceof ActiveRecord ? $user->getId() : (int) $user;
    $friend = $friend instanceof ActiveRecord ? $friend->getId() : (int) $friend;
    $n = S('dbo')->fetchValue('SELECT COUNT(*)'
                              .' FROM '.self::TABLE
                              .' WHERE (user_id='.$user
                              .   ' AND friend_id='.$friend
                              .   ' AND status>0)'
                              .   ' OR (user_id='.$friend
                              .   ' AND friend_id='.$user
                              .   ' AND status>0)'
                              .' LIMIT 1');
    return $n > 0;
  }

  public static function distance($from, $to) {
    $from_id = (int) ($from instanceof ActiveRecord ? $from->getId() : $from);
    $to_id = (int) ($to instanceof ActiveRecord ? $to->getId() : $to);
    if (self::isConnected($from_id, $to_id)) return 1;
    try {
      $from = $from instanceof ActiveRecord ? $from : User::getInstance($from_id);
      $to = $to instanceof ActiveRecord ? $to : User::getInstance($to_id);
      return self::graph()->getDistance($from, $to);
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
    return 0;
  }

  public static function addConnexion($user, $friend) {
    $user_id = (int) ($user instanceof ActiveRecord ? $user->getId() : $user);
    $friend_id = (int) ($friend instanceof ActiveRecord ? $friend->getId() : $friend);
    if (self::isConnected($user, $friend)) return ;

    $row = array('user_id' => $user_id,
                 'friend_id' => $friend_id);
    try {
      $user = $user instanceof ActiveRecord ? $user : User::getInstance($user_id);
      $friend = $friend instanceof ActiveRecord ? $friend : User::getInstance($friend_id);
      self::graph()->addConnexion($user, $friend);
      $row['status'] = self::STATUS_IN_GRAPH;
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      $row['status'] = self::STATUS_ONLINE;
    }
    S('dbo')->insert(self::TABLE, $row);
  }

  public static function removeConnexion($user, $friend) {
    $user_id = (int) ($user instanceof ActiveRecord ? $user->getId() : $user);
    $friend_id = (int) ($friend instanceof ActiveRecord ? $friend->getId() : $friend);
    try {
      $user = $user instanceof ActiveRecord ? $user : User::getInstance($user_id);
      $friend = $friend instanceof ActiveRecord ? $friend : User::getInstance($friend_id);
      self::graph()->removeConnexion(array($user, $friend));
    }
    catch (Exception $e) {
      S('dbo')->exec('UPDATE '.self::TABLE
                     . ' SET status='.self::STATUS_REMOVED
                     .' WHERE (user_id='.$user_id.' AND friend_id='.$friend_id.')'
                     .   ' OR (user_id='.$friend_id.' AND friend_id='.$user_id.')');
      S('log')->error($e, __METHOD__);
      return ;
    }
    S('dbo')->exec('DELETE FROM '.self::TABLE
                   .' WHERE (user_id='.$user_id.' AND friend_id='.$friend_id.')'
                   .       ' OR (user_id='.$friend_id.' AND friend_id='.$user_id.')');
    S('dbo')->exec('DELETE FROM '.T('invitation')
                   .' WHERE model_id='.Model::id('user')
                   .  ' AND ((user_id='.$user_id.' AND record_id='.$friend_id.' AND recipient_record_id='.$friend_id.')'
                   .       ' OR (user_id='.$friend_id.' AND record_id='.$user_id.' AND recipient_record_id='.$user_id.'))');
  }

  public static function getPath(User $from, User $to) {
    $paths = self::graph()->getPaths($from, $to);
    if (empty($paths)) return array();
    return $paths[0];
  }

  public static function getPaths(User $from, User $to) {
    if (self::isConnected($from, $to)) {
      $paths = array();
      $paths[0][] = $from;
      $paths[0][] = $to;
      return $paths;
    }
    return self::graph()->getPaths($from, $to);
  }

  private static $graph = null;
  public static function graph() {
    if (self::$graph === null) {
      if (self::NEO4J === false) {
        self::$graph = new GraphNull();
      }
      else {
        self::$graph = Graph::getInstance();
      }
    }
    return self::$graph;
  }

}

class GraphNull {

  public function getDistance($node1, $node2, $opts=null) { return 0; }

  public function getPath($node1, $node2, $opts=null) {  return array(); }

  public function getPaths($node1, $node2, $opts=null) {  return array(); }

  public function addConnexion($from, $to, $opts=null) { return array(); }

  public function removeConnexion($id, $type=null) { return ; }

}