<?php

class Workflow extends ActiveRecord {

  public function initialize($inputs, $opts = array()) {
    $commit = array('code' => $this->code,
                    'name' => $this->name,
                    'counter' => 0,
                    'tasks' => $this->tasks,
                    'inputs' => $inputs,
                    'status' => Execution::STATUS_INITIALIZED);
    $execution = $this->addExecution($commit);
    return $execution;
  }

  public function getNextTask($execution) {

    $last_process = $execution->getLastProcess();
    if ($last_process) {
      $current_node_path = $last_process->node_path;
    }
    else {
      $current_node_path = '/';
    }

    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    if ($doc->loadXML($execution->tasks) === false) {
      S('log')->error('bad XML', __METHOD__);
      throw new Exception();
    }
    $xpath = new DomXPath($doc);
    $nodeList = $xpath->query($current_node_path);
    $currentNode = $nodeList->item(0);
    foreach ($currentNode->childNodes as $childNode) {
      if (!($childNode instanceof DOMElement)) continue;
      $condition = $childNode->getAttribute('condition');
      if (self::isTaskMatch($condition, $previous_process)) {
        $task = Task::getInstance(array('code' => $childNode->getAttribute('code')));
        return array('task' => $task,
                     'node_path' => $childNode->getNodePath());
      }
    }

    return false;
  }

  public static function isTaskMatch($condition = null, $previous_process = null) {

    if ($previous_process === null) {
      // cas de la 1ère task
      return true;
    }
    if (empty($condition)) {
      return true;
    }
    // condition == success
    if ($condition == 'success' && $previous_process->status == Process::STATUS_COMPLETED) {
      return true;
    }
    // condition == failure
    elseif ($condition == 'failure' && $previous_process->status == Process::STATUS_FAILED) {
      return true;
    }
    // condition == /pattern/
    elseif (substr($condition, 0, 1) == '/' && substr($condition, -1, 1) == '/') {
      $pattern = $condition;
      if (preg_match($pattern, $previous_process->buffer_output)) { // !== false && > 0
        return true;
      }
    }
    // condition == '$x==2'
    else {
      $matches = array();
      preg_match_all('/^\$(?P<field>\w+)(?P<operator>[><=]+)(?P<value>\w+)$/', $condition, $matches);
      $field = $matches['field'][0];
      $operator = $matches['operator'][0];
      $value = $matches['value'][0];

      //S('log')->debug($condition, __METHOD__.' $condition');
      //S('log')->debug($previous_process->output, __METHOD__.' $previous_process->output');

      if ($operator == '==' && ($previous_process->output[$field] == $value)) {
        return true;
      }
      elseif ($operator == '>=' && ($previous_process->output[$field] >= $value)) {
        return true;
      }
      elseif ($operator == '>' && ($previous_process->output[$field] > $value)) {
        return true;
      }
      elseif ($operator == '<=' && ($previous_process->output[$field] <= $value)) {
        return true;
      }
      elseif ($operator == '<' && ($previous_process->output[$field] < $value)) {
        return true;
      }

    }
    return false;
  }

  protected function onBeforeCommit(&$attrs) {

    if ($attrs['tasks']) {
      $xml = $attrs['tasks'];
      // notion de cohérence entre les tasks
      $doc = new DOMDocument();
      $doc->preserveWhiteSpace = false;

      if ($doc->loadXML($xml) === false) {
        S('log')->error('bad XML', __METHOD__);
        throw new Exception('bad XML');
      }

      $rootElement = $doc->documentElement;
      self::checkInputChildNodes($rootElement);

    }
  }

  public static function checkInputChildNodes($parentElement){
    $parent_task_code = $parentElement->getAttribute('code');
    $task_parent = Task::getInstance($parent_task_code);
    $outputs = Sh::asSet($task_parent->outputs, ',');
    foreach ($parentElement->childNodes as $childNode) {
      if (!($childNode instanceof DOMElement)) continue;
      $child_task_code = $childNode->getAttribute('code');
      $child_task = Task::getInstance($child_task_code);
      $inputs = Sh::asSet($child_task->inputs, ',');

      $intersect = array_intersect($inputs, $outputs);
      if (count($intersect) != count($inputs)) {
        throw new Exception('Parent[' . $parent_task_code . '](' . $task_parent->outputs . ') => Child [' . $child_task_code . '] (' . $child_task->inputs . ') ');
      }

      self::checkInputChildNodes($childNode);
    }
  }

}