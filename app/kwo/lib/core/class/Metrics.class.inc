<?php

class Metrics
{
  private $blocks = null;
  private $day = null;
  private $format = '';
  private $item = null;
  private $month = null;
  private $period = null;
  private $value = '';
  private $year = null;

  public function __construct($item, $period) {
    $this->item = $item;
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
  
  public function getBlock($code) {
    if ($this->blocks[$code]) {
      return $this->blocks[$code];
    }
    $block = new MetricsBlock($this);
    $this->blocks[$code] = $block;
    return $block;
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
      return 'Globale';
    }
    if ($this->isYear()) {
      return 'Année '.$this->year;
    }
    if ($this->isMonth()) {
      return I18N::$months[Locale::FR]['long'][$this->month].' '.$this->year;
    }
    if ($this->isDay()) {
      return DH::asTextual($this->year.'-'.$this->month.'-'.$this->day);
    }
  }

  public function filter($column) {
    if ($this->isGlobal()) return ' 1';
    return ' DATE_FORMAT('.$column.', "'.$this->format.'")="'.$this->value.'"';
  }

  public function loadDefaultBlocks() {

    $dbo = S('dbo');
    $item = $this->item;

    //-- interactions

    $rs = $dbo->query('SELECT type, COUNT(record_id) AS n'
                      .' FROM '.Interaction::TABLE
                      .' WHERE model_id='.$item->getModelId()
                      .  ' AND record_id= '.$item->getId()
                      .' GROUP BY type');
    $h = array();
    foreach ($rs as $row) {
      $n = (int) $row['n'];
      $attrs = array('data' => array('action' => 'interaction.export',
                                     'item' => $item->getKey(),
                                     'values' => array('interaction_type' => $row['type']),
                                     'container' => 'stdout'),
                     'on' => array('click' => 'kwo.ux.onExec(this)'));
      $link = H::elt('a', $n, $attrs);
      $h[Interaction::$names[$row['type']]] = $link;
    }
    $n = $dbo->fetchValue('SELECT COUNT(*)'
                          .' FROM '.T('subscription')
                          .' WHERE model_id='.$item->getModelId()
                          .  ' AND record_id='.$item->getId()
                          .  ' AND status>0');
    if ($n > 0) {
      $h['abonné(s)'] = $n;
    }
    $n = $dbo->fetchValue('SELECT COUNT(*)'
                          .' FROM '.T('subscription')
                          .' WHERE model_id='.$item->getModelId()
                          .  ' AND record_id='.$item->getId()
                          .  ' AND status<1');
    if ($n > 0) {
      $h['désabonné(s)'] = $n;
    }
    
    $metas = $item->getMetas();
    if ($metas[Meta::FIELD_VALUATIONS]) {
      $h['note'] = $metas[Meta::FIELD_RATING].' - '.$metas[Meta::FIELD_VALUATIONS].' vote(s)';
    }

    if (count($h) > 0) {
      $block = $this->getBlock('interactions');
      $block->setLabel('statistiques générales');
      $block->setValues($h);
    }
 
    //-- conversions

    if (!Model::exists('order')) return ;

    $purchasables = array();
    $has_many = (array) Model::parameter($item->getModel(), 'has_many');
    foreach ($has_many as $name => $h) {
      if ($h['through']) continue ;
      if (!Model::hasField($name, $foreign_key)) continue ;
      $cr = Item::getInstance($name);
      if ($cr instanceof Purchasable) {
        $purchasables[] = $name;
      }
    }

    if ($item instanceof Purchasable) { 
      
      $rs = $dbo->query('SELECT o.id, SUM(p.quantity) AS q, p.price, o.discount_amount, o.shipping_country_id'
                        .' FROM '.T('purchase').' AS p,'
                        .     ' '.T('order').' AS o'
                        .' WHERE p.model_id='.$item->getModelId()
                        .  ' AND p.record_id='.$item->getId()
                        .  ' AND p.order_id=o.id'
                        .  ' AND o.status>='.Order::STATUS_PAID
                        .' GROUP BY p.order_id');
      $n = $q = $fr = $discounts = $ca = 0;
      foreach ($rs as $row) {
        $n++;
        $q += $row['q'];
        $ca += floor($row['q'] * $row['price']);
        if ($row['shipping_country_id'] == Country::FR) $fr++;
        if ($row['discount_amount'] > 0) $discounts++;
      }
      $link = $n;
      if ($n > 0) {
        $attrs = array('data' => array('action' => 'order.export',
                                       'item' => $item->getKey(),
                                       'container' => 'stdout'),
                       'on' => array('click' => 'kwo.ux.onExec(this)'));
        $link = H::elt('a', $n, $attrs);
      }
      
      $h = array('commande(s)' => $link,
                 'commande(s) avec réduction' => $discounts,
                 'CA (Eur HT)' => $ca,
                 'livraison(s) hors FR' => ($n - $fr),
                 'quantité vendue' => $q);
      $rs = $dbo->query('SELECT * FROM '.Item::TABLE_DISCOUNT
                        .' WHERE model_id='.$item->getModelId()
                        .  ' AND record_id='.$item->getId());
      $discounts = array();
      foreach ($rs as $row) {
        $discounts[] = c(Discount::name($row['discount_id'])).' : '.$row['amount'].' EUR';
      }
      if (!empty($discounts)) {
        $h['réduction(s) en cours'] = join('<br/>', $discounts);
      }
      S('log')->debug($h);
      if ($q > 0) {
        $block = $this->getBlock('orders');
        $block->setLabel('commandes');
        $block->setValues($h);
      }

    }      
    elseif (count($purchasables) > 0) {
      $h = array();
      foreach ($purchasables as $pm) { 
        $rs = $dbo->query('SELECT o.id, o.discount_amount, o.shipping_country_id, p.quantity'
                          .' FROM '.T('purchase').' AS p,'
                          .     ' '.T('order').' AS o,'
                          .     ' '.T($pm).' AS i'
                          .' WHERE p.model_id='.Model::id($pm)
                          .  ' AND p.record_id=i.id'
                          .  ' AND i.'.$foreign_key.'='.$item->getId()
                          .  ' AND p.order_id=o.id'
                          .  ' AND o.status>='.Order::STATUS_PAID);
        $q = 0;
        $orders = $fr = $discounts = array();
        foreach ($rs as $row) {
          $orders[$row['id']]++;
          $q += $row['quantity'];
          if ($row['shipping_country_id'] == Country::FR) $fr[$row['id']]++;
          if ($row['discount_amount'] > 0) $discounts[$row['id']]++;
        }
        
        $n = count($orders);
        $link = $n;
        if ($n > 0) {
          $attrs = array('data' => array('action' => 'order.export',
                                         'values' => array('purchase_model' => $pm),
                                         'item' => $item->getKey(),
                                       'container' => 'stdout'),
                         'on' => array('click' => 'kwo.ux.onExec(this)'));
          $link = H::elt('a', $n, $attrs);
        }
        
        $h += array(c($pm).' : commande(s)' => $link,
                    c($pm).' : commande(s) avec réduction' => count($discounts),
                    c($pm).' : livraison(s) hors FR' => $n - count($fr),
                    c($pm).' : quantité vendue' => $q);
      }
      if ($q > 0) {
        $block = $this->getBlock('related-orders');
        $block->setLabel('commandes liées');
        $block->setValues($h);
      }
    }


  }

  public function asHtml() {
    //set positions
    $columns = array();
    foreach ($this->blocks as $code => $block) {
      $columns[$block->getColumn()][] = $block;
    }
    if (count($columns) == 1) $columns[] = array();
    $html = '<fieldset><legend>Métriques</legend><div class="metrics"><table class="columns"><tbody><tr>';
    foreach ($columns as $num => $blocks) {
      $html .= '<td>';
      foreach ($blocks as $code => $block) {
        $html .= $block->asHtml($locale)."\n";
      }
      $html .= '</td>';
    }
    $html .= '</tr></tbody></table></div></fieldset>';
    return $html;
  }

  public static function getInstance($item, $period=null) { 
    if ($period === null) {
      $period = S('req')->getAttribute('period');
    }
    return new Metrics($item, $period);
  }

}

class MetricsBlock 
{
  public $metrics = null;

  private $actions = array();
  private $code = '';
  private $column = 1;
  private $columns = null;
  private $label = null;
  private $position = 1;
  private $values = array();
  private $graph = '';

  public function __construct($metrics) {
    $this->metrics = $metrics;
  }

  public function setLabel($label) {
    $this->label = $label;
  }

  public function setColumn($num) {
    $num = $num ?: 1;
    $this->column = $num;
  }

  public function getColumn() {
    return $this->column;
  }

  public function getPosition() {
    return $this->position;
  }

  public function setPosition($position) {
    $this->position = $position;
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
    $html = '<div class="metrics-block">'."\n";
    $html .= $this->renderGrid();
    $html .= '</div>';
    return $html;
  }
  
  public function addRow($opts) { 
    $this->values[] = new MetricsRow($opts); 
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
    if (!empty($this->columns)) {
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
    if ($first instanceof MetricsRow) { 
      foreach ($this->values as $row) { 
        $html .= $row->asHtml()."\n";
      }
    }
    elseif (is_array($first)) {
      foreach ($this->values as $row) { 
        $html .= '<tr>'."\n";
        foreach ($row as $key => $value) {
          $html .= '<td>';
          $html .= $value;
          $html .= '</td>'."\n";
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

class MetricsRow
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

