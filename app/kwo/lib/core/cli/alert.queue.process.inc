<?php

try {

  return false;

  function send($user_id, $rows) {
    if (count($rows) < 1) return ; 
    $user = User::getInstance($user_id);
    if (!$user->isOnline()) return ; 
    $html = '';
    foreach ($rows as $row) {
      $item = Item::getInstance((int) $row['model_id'],
                                (int) $row['record_id']);
      if (!$item->isOnline()) continue ;
      $hash = $item->asAlertHash($row['locale']);
      $url = H::url($hash['url'], 
                    array('knxm' => Tracker::MEDIUM_MAIL, 
                          'knxs' => 'alert',
                          'knxt' => $row['model_id'].'-'.$row['record_id']));
      $html .= '<a href="'.$url.'" style="text-decoration:none; color:#333;">';
      if (!empty($hash['image'])) {
        $url = H::url($hash['image']);
        $html .= '<img src="'.$url.'" style="float:left; margin-right:10px;">';
      }
      $html .= '<strong>'.$hash['title'].'</strong><br/>';
      if (strlen($row['datas']) > 2) {
        $html .= '<span style="color:#aaa;">';
        $datas = (array) unserialize($row['datas']);
        foreach ($datas as $key => $values) {
          $html .= $key.' : '.join('&middot;', $values);
        }
        $html .= '</span>';
      }
      $html .= '<span>'.$hash['description'].'</span>';
      $html .= '</a>';
      $html .= '<div style="clear:both;"></div><br/>';
    }
    if (empty($html)) {
      S('log')->error('empty content '.$user_id);
      return ;
    }
    $mail = Mail::getInstance();
    $mail->addRecipient($user->email);
    $mail->setSubject('Alerte '.P('app.name'));
    $mail->setBody($html, true);
    $mail->send();
    //    print($user->email.' - '.$row['name'].' - '.$item);

    S('dbo')->exec('UPDATE '.Alert::TABLE_QUEUE
                   .' SET flags=flags|1'
                   .' WHERE user_id='.$user_id
                   .  ' AND scheduled_on="'.DH::today().'"'); 
  }

  if (!($req->manual == 'yes' || intval(date('H')) == 9)) return ;

  $rs = $dbo->query('SELECT *'
                    .' FROM '.Alert::TABLE_QUEUE
                    .' WHERE scheduled_on="'.DH::today().'"'
                    .  ' AND flags=0'
                    .' ORDER BY user_id'); 

  $current = array('user_id' => 0,
                   'rows' => array());

  foreach ($rs as $row) { 
    if ($current['user_id'] != $row['user_id']) {
      send($current['user_id'], $current['rows']);
      $current['user_id'] = $row['user_id'];
      $current['rows'] = array(); 
    }
    $current['rows'][] = $row;
  }

  send($current['user_id'], $current['rows']);
  
}
catch (Exception $e) { $err->add($e); }
