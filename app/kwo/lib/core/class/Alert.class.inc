<?php

class Alert extends ActiveRecord {

  const TABLE_QUEUE = 'core_alert_queue';

  public static $tables = array(self::TABLE_QUEUE);

  const FLAG_DELIVER_EMAIL = 1;
  const FLAG_STATE_PENDING = 2;
  const FLAG_STATE_SENT = 4;

  public static $flags = array(self::FLAG_DELIVER_EMAIL => 'email',
                               self::FLAG_STATE_PENDING => 'en attente',
                               self::FLAG_STATE_SENT => 'envoyée');

  const FREQ_NONE = 0;
  const FREQ_DAY = 1;
  const FREQ_WEEK = 2;
  const FREQ_MONTH = 3;

  public static $frequencies = array(self::FREQ_NONE => 'en veille',
                                     self::FREQ_DAY => 'quotidien',
                                     self::FREQ_WEEK => 'hebdomadaire',
                                     self::FREQ_MONTH => 'mensuel');

  const STATUS_OFFLINE = 0;
  const STATUS_ONLINE = 1;

  public function asChannel() {
    $channel = parent::asChannel();
    $channel['title'] = 'alert : '.$this->getItem->getName();
    return $channel;
  }

  public function getKeyword() {
    $this->getSearch()->getName();
  }

  public static function deliver($user_id, $items) { 
    if ($user_id < 1 || count($items) < 1) return ;
    $html = '<ul>'."\n";
    foreach ($items as $item) {
      $html .= '<li><a href="'.H::url($item->asUrl()).'">'.h($item).'</a></li>'."\n";
    }
    $html .= '</ul>';
    $user = User::getInstance($user_id);
    $link = '<a href="/account/alert.list">'.c(l('alert:alert.manage', null, $user->locale)).'</a>';
    Letter::send('alert.document', $user, 
                 array('items' => $html,
                       'account' => $link));
  }

  public function asString() {
    $prefix = l('alert.type.keyword');
    return $prefix.' : <strong>&laquo; '.h($this->getKeyword()).' &raquo;</strong>';
  }

  public function feed() {
    return '/alert.feed?item='.$this->getKey();
  }

  public static function enqueue($rec) {
    $dbo = S('dbo');
    $rec['scheduled_on'] = $rec['scheduled_on'] ?: DH::today();
    $row = $dbo->fetchHash('SELECT *'
                           .' FROM '.self::TABLE_QUEUE
                           .' WHERE model_id='.$rec['model_id']
                           .  ' AND record_id='.$rec['record_id']
                           .  ' AND user_id='.$rec['user_id']
                           .  ' AND scheduled_on="'.$rec['scheduled_on'].'"'
                           .' LIMIT 1');
    if (empty($row)) { S('log')->debug($row);
      if (!empty($rec['type'])) {
        $datas = array();
        $datas[$rec['type']] = array($rec['value']);
      }
      $rec['datas'] = serialize($datas);
      unset($rec['type']);
      unset($rec['value']);
      $dbo->insert(self::TABLE_QUEUE, $rec);
      return ;
    }
    if (!empty($rec['type'])) {
      $datas = strlen($row['datas']) > 2 ? unserialize($row['datas']) : array();
      $datas = (array) $datas;
      $datas[$rec['type']] = (array) $datas[$rec['type']];
      $datas[$rec['type']][] = $rec['value'];
      $rec['datas'] = serialize($datas);
    }
    unset($rec['type']);
    unset($rec['value']);
    $opts = array('filters' => array('model_id' => $rec['model_id'],
                                     'record_id' => $rec['record_id'],
                                     'user_id' => $rec['user_id'],
                                     'scheduled_on' => $rec['scheduled_on']),
                  'limit' => 1);
    $dbo->update(self::TABLE_QUEUE, $rec, $opts);
  }

  public static function onAppReset() {
    $dbo = S('dbo');
    $tables = array(T('alert'), Alert::TABLE_QUEUE, 
                    T('search'), Search::TABLE_RESULTS, Search::TABLE_UPDATES);
    foreach ($tables as $table) {
      if (!$dbo->hasTable($table, false)) {
        S('log')->trace('table unknown : '.$table);
        continue ;
      }
      $dbo->getTable($table)->truncate();
    }
  }

}