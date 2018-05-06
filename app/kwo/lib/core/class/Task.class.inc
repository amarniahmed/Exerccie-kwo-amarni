<?php

class Task extends ActiveRecord {

  const TYPE_VIDEO_ENCODING = 1;
  const TYPE_PDF_CONVERSION = 2;
  const TYPE_SCRIPT_EXEC = 3;

  const PATH = 'var/run';

  public static function add($h) {
    $dbo = S('dbo');
    foreach ($h as $key => &$value) {
      if (is_array($value)) {
        $value = serialize($value);
      }
    }
    $dbo->insert(T('task'), $h);
  }

  public function initialize($input) {
    $process = Process::getInstance();
    $commit = array('task_id' => $this->getId(),
                    'input' => $input);
    $process->commit($commit);
    return $process;
  }


/*
  public static function show($statuses = null, $last = null) {
    if ($statuses === null) {
      $statuses = array(self::STATUS_PENDING,
                        self::STATUS_PROCESSING);
    }
    $opts = array('filters' => array('status' => $statuses),
                  'order' => array('id' => 'asc'));
    if ($last > 0) {
      $opts['last'] = $last;
    }
    $total = 0;
    foreach (self::collect($opts) as $that) {
      echo "\n" . $that->getId() . "\tajout    : " . $that->queued_at . "\n\t";
      if ($that->status == self::STATUS_PROCESSING) {
        echo 'status   : en cours (depuis le ' . $that->started_at . ")\n";
      }
      elseif ($that->status == self::STATUS_COMPLETED) {
        echo 'status   : terminé (débuté le ' . $that->started_at . " et terminé le " . $that->completed_at . ")\n";
        if (!empty($that->output)) {
          echo "\t" . 'output   : ' . $that->output . "\n";
        }
      }
      elseif ($that->status == self::STATUS_FAILED) {
        echo 'status   : erreur (débuté le ' . $that->started_at . ")\n";
        if (!empty($that->error)) {
          echo "\terreur   : " . $that->error . "\n";
        }
      }
      else {
        echo 'status   : en attente' . "\n";
      }
      if ($that->model_id > 0 &&
        $that->record_id > 0 &&
        Item::exists($that->model_id, $that->record_id)
      ) {
        echo "\titem     : [M:" . $that->model_id . "-R:" . $that->record_id . "] " . (string) $that->getItem() . "\n";
      }
      if ($that->user_id > 0
        && Model::exists('user', $that->user_id)
      ) {
        echo "\tuser     : " . (string) $that->getUser() . "\n";
      }
      if (!empty($that->callback)) {
        echo "\tcallback : " . $that->callback . "()\n";
      }
      if (!empty($that->name)) {
        echo "\tnom      : " . $that->name . "\n";
      }
      if (!empty($that->input)) {
        echo "\tinput    : \n" . var_export($that->input, true) . "\n";
      }
      $total++;
    }
    echo 'Total : ' . $total . "\n";
  }
*/
}