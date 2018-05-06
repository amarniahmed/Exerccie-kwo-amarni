<?php

try {

  $graph = Graph::getInstance();
  $graph->ping();

  $rs = $dbo->query('SELECT * FROM '.Network::TABLE
                    .      ' WHERE status='.Network::STATUS_ONLINE);
  foreach ($rs as $row) {
    try {
      $graph->addConnexion(User::getInstance((int) $row['user_id']),
                           User::getInstance((int) $row['friend_id']));
      $dbo->exec('UPDATE '.Network::TABLE
                 . ' SET status='.Network::STATUS_IN_GRAPH
                 .' WHERE user_id='.$row['user_id']
                 .  ' AND friend_id='.$row['friend_id']);
    }
    catch (Exception $e) { $log->error($e); }
  }

  $rs = $dbo->query('SELECT * FROM '.Network::TABLE
                    .      ' WHERE status='.Network::STATUS_REMOVED);
  foreach ($rs as $row) {
    try {
      $graph->removeConnexion(User::getInstance((int) $row['user_id']),
                              User::getInstance((int) $row['friend_id']));
      $dbo->exec('DELETE FROM '.Network::TABLE
                 .    ' WHERE user_id='.$row['user_id']
                 .      ' AND friend_id='.$row['friend_id']);
    }
    catch (Exception $e) { $log->error($e); }
  }

}
catch (Exception $e) { $err->add($e); }
