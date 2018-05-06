<?php

class Valuation extends ActiveRecord {

  const TABLE_LOG = 'core_criterion_valuation';

  const OPINION_NEGATIVE = -1;
  const OPINION_NEUTRAL = 0;
  const OPINION_POSITIVE = 1;

  const RATE_MAX = 5;

  public static $opinions = array(self::OPINION_NEGATIVE => 'valuation.negative',
                                  self::OPINION_NEUTRAL => 'valuation.neutral',
                                  self::OPINION_POSITIVE => 'valuation.positive');


  protected function onAferDestroy() {
    $this->dbo->exec('DELETE FROM '.self::TABLE_LOG
                     .    ' WHERE valuation_id='.$this->getId());
  }

  public static function isEvaluated($item, $user) {
    $user_id = $user instanceof User ? $user->getId() : (int) $user;
    if ($user_id < 1) return false;
    $row = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId(),
                 'user_id' => $user_id);
    return Item::exists('valuation', $row);
  }

  public static function grade($item, $user) {
    $user_id = $user instanceof User ? $user->getId() : (int) $user;
    try {
      $args = array('model_id' => $item->getModelId(),
                    'record_id' => $item->getId(),
                    'user_id' => $user_id);
      return (int) Valuation::getInstance($args)->grade;
    }
    catch (Exception $e) { }
    return 0;
  }

  public function onAfterCreate() {
    S('app')->hook('onAfterValuationCreate', array('valuation' => $this));
  }

}