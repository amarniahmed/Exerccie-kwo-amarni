<?php

class DashBoard
{
  private $blocks = null;
  private $period = null;
  private $year = null;
  private $month = null;
  private $day = null;
  private $format = '';
  private $value = '';

  public function __construct($period) {
    $this->blocks = array();
    $this->period = $period;
    list ($this->year, $this->month, $this->day) = explode('-', $this->period);
    $this->year = (int) $this->year;
    $this->month = (int) $this->month;
    $this->day = (int) $this->day;
    if ($this->year > 0) {
      $this->format .= '%Y';
      $this->value .= $this->year;
    }
    if ($this->month > 0) {
      $this->format .= '%c';
      $this->value .= $this->month;
    }
    if ($this->day > 0) {
      $this->format .= '%e';
      $this->value .= $this->day;
    }
  }

  public function getBlock() {
    $dbb = new DashBoardBlock($this);
    $this->blocks[] = $dbb;
    return $dbb;
  }

  public function asHtml() {
    $html = '';
    if (S('req')->match('dashboard.overview$')) {
      $html .= '<div class="hbox-label">'.$this->getPeriodLabel().'</div>';
    }
    foreach ($this->blocks as $block) {
      $html .= $block->asHtml($locale)."\n";
    }
    return $html;
  }

  public function isGlobal() { return $this->year < 1; }
  public function isYear() { return $this->year >= 1 && $this->month < 1; }
  public function isMonth() { return $this->month >= 1 && $this->day < 1; }
  public function isDay() { return $this->day >= 1; }

  public function getPeriod() {
    return $this->period;
  }

  private function getPeriodLabel() {
    if ($this->isGlobal()) {
      return c(l('globale'));
    }
    if ($this->isYear()) {
      return c(l('année')).' '.$this->year;
    }
    if ($this->isMonth()) {
      return I18N::$months[BackManager::getLocale()]['long'][$this->month].' '.$this->year;
    }
    if ($this->isDay()) {
      return DH::asTextual($this->year.'-'.$this->month.'-'.$this->day, BackManager::getLocale());
    }
  }

  public function filter($column) {
    if ($this->isGlobal()) return ' 1';
    return ' DATE_FORMAT('.$column.', "'.$this->format.'")="'.$this->value.'"';
  }

  public static function getInstance($period=null) {
    if ($period === null) {
      $period = S('req')->getAttribute('period');
    }
    return new DashBoard($period);
  }

}

class DashBoardBlock
{
  public $dashboard = null;

  private $actions = array();
  private $column = 0;
  private $columns = null;
  private $label = null;
  private $values = array();
  private $graph = '';

  public function __construct($dashboard) {
    $this->dashboard = $dashboard;
  }

  public function setLabel($label) {
    $this->label = $label;
  }

  public function setColumn($num) {
    $this->column = $num;
  }

  public function setColumns($columns) {
    $this->columns = array();
    foreach ($columns as $key => $value) {
      $column = array('label' => null,
                      'code' => null,
                      'width' => null);
      if (is_array($value)) {
        $code = $key;
        $label = empty($value['label']) ? $code : $value['label'];
        if ($value['width']) {
          $column['width'] = $value['width'];
        }
      }
      elseif (is_int($key)) {
        $code = $value;
        $label = $value;
      }
      else {
        $code = $key;
        $label = $value;
      }
      $column['label'] = $label;
      $column['code'] = $code;
      $this->columns[] = $column;
    }
  }

  public function setValues($values) {
    $this->values = $values;
  }

  public function setGraph($graph) {
    $this->graph = $graph;
  }

  public function addAction($label, $action, $params=null) {
    $this->actions[] = array('label' => $label,
                             'action' => $action,
                             'params' => $params);
  }

  public function asHtml() {
    $html = '<div class="dashboard-block">'."\n";
    $html .= $this->renderGrid();
    $html .= '</div>';
    return $html;
  }

  public function addRow($opts) {
    $this->values[] = new DashBoardRow($opts);
  }

  private function renderGrid() {
    $html = '';
    if (!empty($this->label)) {
      $html .= '<div class="label">'."\n";
      if (!empty($this->actions)) {
        $html .= '<div class="actions">'."\n";
        foreach ($this->actions as $action) {
          $datas = (array) $action['params'];
          $datas['action'] = $action['action'];
          $html .= H::elt('span', $action['label'],  array('data' => $datas))."\n";
        }
        $html .= '</div>'."\n";
      }
      $html .= c($this->label)."\n";
      $html .= '</div>'."\n";
    }
    if ($this->graph) {
      $html = '<div class="graph"><img src="'.$this->graph.'" class="graph" /></div>'."\n";
    }
    $html .= '<div class="values"><table class="grid">'."\n";
    $num_cols = 0;
    if (!empty($this->columns)) {
      $num_cols = count($this->columns);
      $html .= '<thead><tr>'."\n";
      foreach ($this->columns as $column) {
        $style = '';
        if ($column['width']) {
          $style = ' style="width:'.$column['width'].'"';
        }
        $html .= '<th'.$style.'>';
        $html .= $column['label'];
        $html .= '</th>'."\n";
      }
      $html .= '</tr></thead>'."\n";
    }
    $html .= '<tbody>'."\n";
    $first = reset($this->values);
    if ($first instanceof DashBoardRow) {
      foreach ($this->values as $row) {
        $html .= $row->asHtml()."\n";
      }
    }
    elseif (is_array($first)) {
      foreach ($this->values as $row) {
        $html .= '<tr>'."\n";
        $i = 1;
        foreach ($row as $key => $value) {
          $html .= '<td>';
          $html .= $value;
          $html .= '</td>'."\n";
          $i++;
          if ($num_cols > 0 && $i > $num_cols) break ;
        }
        $html .= '</tr>'."\n";
      }
    }
    else {
      foreach ($this->values as $key => $value) {
        $html .= '<tr>'."\n";
        $html .= '<td>'.$key.'</td>';
        $html .= '<td>'.$value.'</td>'."\n";
        $html .= '</tr>'."\n";
      }
    }
    $html .= '</tbody>'."\n";
    $html .= '</table></div>';
    return $html;
  }

}

class DashBoardRow
{
  private $values = null;
  private $action = null;
  private $params = null;

  public function __construct($opts) {
    $this->values = $opts['values'];
    $this->action = $opts['action'];
    $this->params = $opts['params'];
  }

  public function asHtml() {
    if ($this->action) {
      $attrs['data'] = array('values' => (array) $this->params);
      $attrs['data']['action'] = $this->action;
      $html = H::tag('tr', $attrs);
    }
    else {
      $html = '<tr>';
    }
    $html .= "\n";
    foreach ($this->values as $value) {
      $html .= '<td>'.$value.'</td>'."\n";
    }
    $html .= '</tr>';
    return $html;
  }

}