<?php

try {

  //  $notices = $user->getNotices()->asSet();
  
  foreach ($req->notices as $notice_id => $value) {
    $notice = Notice::getInstance($notice_id);
    if (!$notice->hasFlag(Notice::FLAG_ACCOUNT)) continue ;
    if ($value == 1) {
      $user->setFlag($notice, 'notices');
    }
    else {
      $user->unsetFlag($notice, 'notices');
    }
  }

  $res->setFlash(l('flash.save.success'));

}
catch (Exception $e) { $err->add($e); }
