<?php

class Trackvar extends ActiveRecord {

  public function onBeforeCommit(&$attrs) {
    $mediums = array(Tracker::MEDIUM_AFFILIATE, Tracker::MEDIUM_CAMPAIGN, Tracker::MEDIUM_LETTER);
    if (in_array($attrs['medium'], $mediums)) {
      switch ($attrs['medium']) {
        case Tracker::MEDIUM_AFFILIATE:
          $model = 'affiliate';
          break;
        case Tracker::MEDIUM_CAMPAIGN:
          $model = 'campaign';
          break;
        case Tracker::MEDIUM_USER:
          $model = 'user';
          break;
        case Tracker::MEDIUM_LETTER:
          $model = 'letter';
          break;
      }
      if (!is_numeric($attrs['source']) || $attrs['source'] < 1) {
        throw new Exception('la source doit correspondre à un id ['.$model.']');
      }
      if (!Item::exists($model, $attrs['source'])) {
        throw new Exception('cet id n’existe pas');
      }
    }
  }

  public function asFinderHash($h) {
    $h['medium'] = Tracker::$mediums[$h['medium']];
    return $h;
  }

  public function addMetrics($metrics) {
    $block = $metrics->getBlock('trackvars-interactions');
    $block->setLabel('statistiques générales');
    $exprs = $this->getExpressions();
    $h = S('dbo')->fetchHash('SELECT COUNT(id) AS v, SUM(conversions) AS c'
                             .' FROM '.T('visit')
                             .' WHERE medium='.$exprs['medium']
                             .  ' AND source='.$exprs['source']
                             .  ' AND term='.$exprs['term']
                             .  ' AND campaign='.$exprs['campaign']);
    $values = array('visite(s)' => (int) $h['v'],
                    'conversion(s)' => (int) $h['c']);
    $block->setValues($values);
  }

  public function getExpressions() {
    $h['medium']   = $this->medium;
    $h['source']   = Tracker::str2id($this->source, Tracker::EXPR_SOURCE);
    $h['term']     = Tracker::str2id($this->term, Tracker::EXPR_TERM);
    $h['campaign'] = Tracker::str2id($this->campaign, Tracker::EXPR_CAMPAIGN);
    return $h;
  }

  public static function mediums() {
    $mediums = Tracker::$mediums;
    unset($mediums[Tracker::MEDIUM_LETTER]);
    return $mediums;
  }

}