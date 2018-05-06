<?php

// http://en.wikipedia.org/wiki/Recommender_system

class RecommendationEngine
{
  private $dbo = null;
  private $exclude = array();
  private $input = null;
  private $items = array();
  private $opts = array();
  private $order = null;
  private $user = null;

  /***
  -> param : opts
  input: $item (order, purchase, ..)
  limit: nombre de produits à retourner
  model: model des items à retourner
  user: 

  -> out : array('mode1-record1', 'model2-record2')
  ***/
  public function __construct($opts) {
    $this->dbo = S('dbo');
    if ($opts['input']) {
      $this->input = $opts['input'];
    }
    if ($opts['user']) {
      $this->user = is_numeric($opts['user']) ? User::getInstance((int) $opts['user']) : $opts['user'];
    }
    if ($this->input instanceof Purchasable) { 
      $this->order = $this->input->getOrder();
      if (!$this->user && $this->order->user_id > 0) {
        $this->user = User::getInstance((int) $this->order->user_id);
      }
    }
    if (!$this->user) {
      $req = S('req');
      if ($req->isAuthenticated()) {
        $this->user = $req->getUser();
      }
    }
  }

  // on exploite les purchases, les crosssell, les bookmarks, les likes, les views, etc.
  public function getItems() {
    try {
      if ($this->order) {
        $this->getItemsByOrder();
      }
      elseif ($this->user) {
        $this->getItemsByUser();
      }
      else {
        $this->getItemsByRandom();
      }
      $this->clean();
      return $this->items;
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
  }

  private function clean() {
    if ($this->order) {
      foreach ($this->order->getPurchases() as $purchase) {
        $this->exclude[] = $purchase->model_id . '-' . $purchase->record_id;
      }
    }
    if ($this->user) {
      $rs = $this->dbo->query('SELECT CONCAT(p.model_id, "-", p.record_id) AS item'
                              .' FROM '.T('purchase').' AS p,'
                              .     ' '.T('order').' AS o'
                              .' WHERE o.user_id='.$this->user->getId()
                              .  ' AND o.status>='.Order::STATUS_PAID
                              .  ' AND p.order_id=o.id'
                              .' GROUP BY item');
      $out = array();
      foreach ($rs as $row) {
        $this->exclude[] = $row['item'];
      }
    }
    $out = array();
    foreach ($this->items as $item) {
      if (in_array($item, $this->items)) continue ;
      if (in_array($item, $out)) continue ;
      $out[] = $item;
    }
    $this->items = $out;
  }
  
  private function getItemsByOrder() {
    $order = $this->order;
    if ($this->input instanceof Purchase) {
      $purchase = $this->input;
    }
    else {
      $purchase = $order->getPurchases()->first();
    }
    $where = '';
    if ($this->user) {
      $where .= ' AND o.user_id<>'.$this->user->getId();
    }
    $orders = $this->dbo->asSet('SELECT DISTINCT p.order_id'
                                .' FROM '.T('purchase').' AS p, '
                                .     ' '.T('order').' AS o'
                                .' WHERE o.id=p.order_id'
                                .  ' AND p.model_id='.$purchase->model_id
                                .  ' AND p.record_id='.$purchase->record_id
                                .  ' AND o.status>='.Order::STATUS_PAID
                                .$where
                                .' LIMIT 100');
    $orders[] = 0;
    $rs = $this->dbo->query('SELECT CONCAT(p.model_id, "-", p.record_id) AS item'
                            .' FROM '.T('purchase').' AS p,'
                            .     ' '.T('order').' AS o'
                            .' WHERE o.id IN ([orders])'
                            .  ' AND p.order_id=o.id'
                            .' GROUP BY item',
                            array('[orders]' => $orders));
    $this->dbo->debug();
    $out = array();
    foreach ($rs as $row) {
      $this->items[] = $row['item']; 
    }
  }

  private function getItemsByUser() {
  }

  private function getItemsByRandom() {
    // -> utilisation de l'affinité entre contenus
  }

  public static function getInstance($opts=array()) {
    return new RecommendationEngine($opts);
  }

}