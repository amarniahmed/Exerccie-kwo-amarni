<?php

try { 

  $user = User::getInstance((int) $req->user_id); 

  if ($user->getId() == $req->getUser()->getId()) {
    throw new Exception('you can not ban yourself');
  }

  if (!$req->isModerator()) {
    throw new PrivilegeException(l('error.privilege'));
  }

  $user->commit(array('status' => User::STATUS_CLOSED));

  $req->track('ban', 'moderation');

  $res->sendMessage('User ['.$user.'] is now banned.');

}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (PrivilegeException $e) { $err->add($e); }
catch (Exception $e) { $err->add($e); }
