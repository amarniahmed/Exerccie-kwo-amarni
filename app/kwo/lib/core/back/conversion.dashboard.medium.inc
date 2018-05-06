<?php

try {
  
  $medium = (int) $req->medium;

  $db = DashBoard::getInstance();

  $cols = array(array('column' => 'source', 'str' => Tracker::EXPR_SOURCE),
                array('column' => 'campaign', 'str' => Tracker::EXPR_CAMPAIGN),
                array('column' => 'term', 'str' => Tracker::EXPR_TERM));
  
  foreach ($cols as $h) {
    $column = $h['column'];
    $rs = $dbo->query('SELECT c.'.$column.', s.name, COUNT(c.id) AS n, SUM(c.worth) AS w, SUM(c.cost) AS cs'
                      .' FROM '.T('conversion').' AS c'
                      .    ', '.Tracker::TABLE_EXPRESSION.' AS s'
                      .' WHERE '.$db->filter('c.converted_at')
                      .  ' AND c.medium='.$medium
                      .  ' AND c.'.$column.'<>0'
                      .  ' AND c.'.$column.'=s.id'
                      .  ' AND s.type='.$h['str']
                      .' GROUP BY c.'.$column
                      .' ORDER BY n DESC'); 
    if ($rs->count() > 0) {
      $b = $db->getBlock();
      $b->setLabel(Tracker::$mediums[$medium].' : '.$column);
      $b->setColumns(array($column => $column, 
                           'n' => array('width' => '40px',
                                        'label' => 'nbre'),
                           'w' => 'gains',
                           'cs' => 'coûts'));
      foreach ($rs as $row) {
        $name = $row['name'];
        if ($name[0] == '|') {
          $name = Tracker::asSourceLink($name, true);
        }
        $values = array($name, 
                        $row['n'], 
                        $row['w'], 
                        $row['cs']);
        $b->addRow(array('values' => $values));
      }
    }
  }

  $res->write($db);

}
catch (Exception $e) { $err->add($e); }
