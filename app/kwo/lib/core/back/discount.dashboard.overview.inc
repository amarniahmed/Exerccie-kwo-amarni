<?php

try {

  $db = DashBoard::getInstance($req->period);

  $rs = $dbo->query('SELECT d.name, d.id, COUNT(o.id) AS n'
                    .' FROM '.T('order').' AS o,'
                    .     ' '.T('discount').' AS d,'
                    .     ' '.Order::TABLE_DISCOUNT.' AS j'
                    .' WHERE '.$db->filter('o.paid_at')
                    .  ' AND o.status='.Order::STATUS_PROCESSED
                    .  ' AND o.id=j.order_id'
                    .  ' AND j.discount_id=d.id'
                    .' GROUP BY d.id'
                    .' ORDER BY n DESC');
  $b = $db->getBlock();
  $b->setLabel(l('réductions de panier les plus efficaces'));
  $b->setColumns(array('name' => l('réduction'),
                        'id' => array('width' => '30px'),
                        'n' => l('nb commande(s)')));
  $b->setValues($rs->asArray());

  $rs = $dbo->query('SELECT d.name, d.id, COUNT(o.id) AS n'
                    .' FROM '.T('order').' AS o,'
                    .     ' '.T('discount').' AS d,'
                    .     ' '.T('purchase').' AS p,'
                    .     ' '.Purchase::TABLE_DISCOUNT.' AS j'
                    .' WHERE '.$db->filter('o.paid_at')
                    .  ' AND o.status='.Order::STATUS_PROCESSED
                    .  ' AND o.id=p.order_id'
                    .  ' AND p.id=j.purchase_id'
                    .  ' AND j.discount_id=d.id'
                    .' GROUP BY d.id'
                    .' ORDER BY n DESC');
  $b = $db->getBlock();
  $b->setLabel(l('réductions de catalogue les plus efficaces'));
  $b->setColumns(array('name' => l('réduction'),
                        'id' => array('width' => '30px'),
                        'n' => l('nb commande(s)')));
  $b->setValues($rs->asArray());

  $res->write($db);

}
catch (Exception $e) { $err->add($e); }