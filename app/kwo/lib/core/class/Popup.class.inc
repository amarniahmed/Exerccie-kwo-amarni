<?php

class Popup extends ActiveRecord {

  const TABLE_HIT = 'core_popup_hit';

  const STATUS_ONLINE = 1;
  const STATUS_SCHEDULED = 0;
  const STATUS_OFFLINE = -1;
  
  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_SCHEDULED => 'planifié',
                                  self::STATUS_OFFLINE => 'hors ligne');

  const FLAG_NEW_VISITOR = 1;

  public static $flags = array(self::FLAG_NEW_VISITOR => 'nouveau visiteur');

  public static $colors = array(self::STATUS_ONLINE => 'green',
                                self::STATUS_SCHEDULED => 'yellow',
                                self::STATUS_OFFLINE => 'red');

  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM '.self::TABLE_HIT
                     .    ' WHERE popup_id='.$this->getId());
  }

  public function addMetrics($metrics) {
    $block = $metrics->getBlock('popups-visits');
    $block->setLabel('statistiques générales');
    $h = S('dbo')->fetchHash('SELECT COUNT(v.id) AS v, SUM(v.conversions) AS c'
                             .' FROM '.self::TABLE_HIT.' AS h,'
                             .     ' '.T('visit').' AS v'
                             .' WHERE h.visit_id=v.id'
                             .  ' AND h.popup_id='.$this->id);
    $values = array('visite(s)' => (int) $h['v'],
                    'conversion(s)' => (int) $h['c']);
    $block->setValues($values);
  }

  public static function findId($req) {
    $ip = $req->getClient('ip');
    $visit_id = $req->getVisitId();
    $user_id = $req->isAuthenticated() ? $req->getUserId() : 0;
    if ($visit_id < 0) return 0;
    $popup_id = 0;
    $dbo = S('dbo');
    $popups = $dbo->asSet('SELECT popup_id'
                          .' FROM '.self::TABLE_HIT
                          .' WHERE visit_id='.$visit_id
                          .   ' OR user_id='.$user_id);
    $popups[] = 0;
    $filters = array('|<>|id' => $popups,
                     'app' => array('', S('app')->getName()),
                     'status' => self::STATUS_ONLINE,
                     'visit_number' => array($req->getVisitNumber(), '0'));
    $opts = array('filters' => $filters,
                  'order' => array('visit_number' => 'desc',
                                   'priority' => 'desc'));
    $popups = Popup::collect($opts);
    if ($popups->count() < 1) return $popup_id;
    if ($req->isNewVisitor()) {
      foreach ($popups as $popup) {
        if ($popup->hasFlag(self::FLAG_NEW_VISITOR)) {
          $popup_id = $popup->getId();
          break ;
        }
      }
    }
    if ($popup_id < 1) {
      $popup_id = $popups->first()->getId();
    }
    if ($popup_id >= 1) {
      $dbo->insert(self::TABLE_HIT,
                   array('popup_id' => $popup_id,
                         'visit_id' => $visit_id,
                         'user_id' => $user_id,
                         'ip' => $ip));
    }
    return $popup_id;
  }

}