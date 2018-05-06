<?php

class Execution extends ActiveRecord {

  const STATUS_INITIALIZED = 0;
  const STATUS_PENDING_INPUT = 1;
  // const STATUS_RUNNING = 10;
  const STATUS_COMPLETED = 100;

  public function getProcesses($opts = null) {
    $opts['filters']['execution_id'] = $this->getId();
    return Process::collect($opts);
  }

  private function run() {
    $workflow = $this->getWorkflow();

    // $outputs = provient de execution (si 1ère tache) ou tache précedente
    $last_process = $this->getLastProcess();
    if ($last_process) {
      $outputs = $last_process->output;
    }
    else {
      $outputs = $this->inputs; // pour init la 1ere task
    }

    // process run loop
    while ($hash = $workflow->getNextTask($this)) {
      $task = $hash['task'];
      $node_path = $hash['node_path'];
      print "Run Task [$task->code:$node_path]\n";

      $process = $task->initialize($outputs);
      $process->setAttributes(array('execution_id' => $this->getId(),
                                    'node_path' => $node_path,
                                    'name' => '[#' . $this->counter . ']' . $task->code));

      if (!$this->processRun($process)) {
        break;
      }
      $outputs = $process->output;
    }

    if (!$task) {
      //$this->commit(array('outputs' => $outputs));
      $this->setStatus(self::STATUS_COMPLETED);
    }
  }

  public function start() {
    if ($this->status != self::STATUS_INITIALIZED
      || $this->status == self::STATUS_COMPLETED
    ) {
      return false;
    }
    $this->run();
  }

  public function resume() {
    if ($this->status != self::STATUS_PENDING_INPUT) {
      throw new Exception('bad status [' . $this->status . ']');
    }
    $process = $this->getLastProcess();
    if (!$process || $process->status != Process::STATUS_PENDING_INPUT) {
      throw new Exception('no process or process don`\'t wait input');
    }
    if(!$this->processRun($process)) {
      return false;
    }

    $this->run();
  }


  private function processRun($process) {
    if ($process->hasUserInput()) {
      $this->setStatus(self::STATUS_PENDING_INPUT);
      $process->setStatus(Process::STATUS_PENDING_INPUT);
      return false;
    }

    $process->run();
    $this->increment('counter');

    if ($process->output === false) { // erreur à l'execution du process
      S('log')->error('Process [' . $process->getId() . ']', __METHOD__);
      return false;
    }
    return true;
  }

  public function getLastProcess() {
    $process = null;
    $processes = $this->getProcesses(array('order' => array('id' => 'DESC'),
                                           'limit' => 1));
    if ($processes->count() > 0) {
      $process = $processes->first();
    }
    return $process;
  }

}