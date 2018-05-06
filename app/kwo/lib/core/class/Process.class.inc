<?php

class Process extends ActiveRecord {

  const STATUS_FAILED = -10;
  const STATUS_PENDING = 0;
  const STATUS_PENDING_INPUT = 1;
  const STATUS_PROCESSING = 5;
  const STATUS_COMPLETED = 10;

  public static $statuses = array(
    self::STATUS_FAILED => 'échec',
    self::STATUS_PENDING => 'en attente',
    self::STATUS_PENDING_INPUT => 'en attente input user',
    self::STATUS_PROCESSING => 'en cours',
    self::STATUS_COMPLETED => 'terminé',
  );

  public function run() {
    try {
      if ($this->status != self::STATUS_PENDING && $this->status != self::STATUS_PENDING_INPUT) {
        throw new Exception('invalid status [' . $this->status . ']');
      }
      $task = $this->getTask();

      //$opts = (array) $opts;

      $attrs = array('started_at' => DH::now());
//            if (isset($opts['pid'])) {
//              $attrs['pid'] = $opts['pid'];
//            }
      if (empty($this->name) &&
        $this->model_id > 0 &&
        Model::exists($this->model_id)
      ) {
        $attrs['name'] = Model::name($this->model_id);
      }


      $this->setAttributes($attrs);
      $this->setStatus(self::STATUS_PROCESSING);
      if (strpos($task->method, '::') === false) {
        throw new Exception('Method not found [' . $task->method . ']', __METHOD__);
      };
      list ($class, $method) = explode('::', $task->method);
      $output = call_user_func(array($class, $method),
                               $this->input,
                               array('process' => $this));
      $this->setAttributes(array('completed_at' => DH::now(),
                                 'output' => $output));
      if ($this->status == self::STATUS_PROCESSING) {
        $this->setStatus(self::STATUS_COMPLETED);
      }
    }
    catch (Exception $e) {
      $this->setStatus(self::STATUS_FAILED);
      $this->setAttribute('error', $e->getMessage());
      S('log')->error($e, __METHOD__);
      return false;
    }
    return true;
  }

  public function hasUserInput() {
    $task = $this->getTask();
    $inputs_user = parse_ini_string($task->inputs_user);
    $pending_inputs = array();
    foreach ($inputs_user as $key => $value) {
      if (!isset($this->input[$key])) {
        $pending_inputs[$key] = $value;
      }
    }
    return $pending_inputs;
  }

  public function commitInputs($args) {
    $inputs = $this->input;
    foreach ((array) $args as $key => $value) {
      $inputs[$key] = $value;
    }
    $this->setAttribute('input', $inputs);
  }

}