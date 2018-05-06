<?php

try {

  $req->cleanAttributes();

  S('app')->hook('onUserValidate');

  if (!$req->hasValidCaptcha()) {
    throw new Exception(l('error.captcha'));
  }

  $record = (array) $req->user;
  $record['email'] = $req->login;

  $password = $req->password;
  unset($record['password']);

  $password_confirm = $record['password_confirm'];
  unset($record['password_confirm']);

  if ($password != $password_confirm) {
    throw new Exception(l('error.password.different'));
  }

  if (isset($record['email_confirm'])) {
    $email_confirm = $record['email_confirm'];
    unset($record['email_confirm']);
    if ($record['email'] != $email_confirm) {
      throw new Exception(l('error.email.different'));
    }
  }

  $gender = $record['gender'];
  unset($record['gender']);

  $birth_date = $record['birth_date'];
  unset($record['birth_date']);

  $profiles = array();
  if (is_array($req->profiles)) {
    $keys = array_keys($req->profiles);
    foreach ($keys as $profile) {
      if (!Model::exists($profile)) {
        throw new Exception(l('error.profile'));
      }
      $profiles[$profile] = $req->profiles[$profile];
    }
  }

  if (isset($birth_date) && DH::isNull($birth_date)) {
    throw new Exception(l('error.fields.required'));
  }
  if (!User::isValidUser($record)) {
    throw new Exception(l('error.fields.required'));
  }

  if (!Email::isValid($record['email'], true)) {
    throw new Exception(l('error.email.invalid'));
  }

  if (empty($record['name'])) {
    list ($record['name'], ) = explode('@', $record['email']);
  }

  $ret = $dbo->fetchValue('SELECT COUNT(*)'
                          .' FROM '.T('user')
                          .' WHERE email=:email:',
                          array(':email:' => $record['email']));
  if ($ret >= 1) {
    throw new Exception(l('error.email.used'));
  }

  if (strlen($password) < 6 || strlen($password) >= 15) {
    throw new Exception(l('error.password.size'));
  }

  $record['status'] = User::STATUS_PENDING;
  $record['locale'] = $res->getLocale();
//  $record['app_id'] = $req->getAppId();

  $record = User::cleanAttributes($record);

  $user = User::getInstance();
  $user->commit($record);
  $user->setPassword($password);
  $user->setVisit($req);

  $informations = array('recipient_id' => Email::asId($record['email']),
                        'auth_at' => DH::now(),
                        'birth_date' => $birth_date);
  if ($req->origin_id >= 1 && Item::exists('origin', (int) $req->origin_id)) {
    $informations['origin_id'] = $req->origin_id;
  }
  $user->setInformations($informations);

  if (count($profiles) >= 1) {
    foreach ($profiles as $profile => $attrs) {
      $user->setProfile($attrs, $profile);
    }
  }

  if ($gender > 0) {
    $user->setFlag(User::FLAG_WOMAN);
  }

  try {
    $tc = $req->getCookie(Tracker::COOKIE_NAME);
    $sponsorship_id = (int) $tc->getAttribute('sponsorship_id');
    if ($sponsorship_id >= 1) {
      $sponsorship = Sponsorship::getInstance($sponsorship_id);
      $sponsorship->markAsAcceptedBy($user);
      $tc->removeAttribute('sponsorship_id');
    }
  }
  catch (Exception $e) {}

  if (is_array($req->godsons)) {
    $godsons = array_unique($req->godsons);
    foreach ($godsons as $email) {
      Sponsorship::deliver($user, $email);
    }
  }

  try {
    if (is_array($req->notices)) {
      foreach ($req->notices as $notice_id => $value) {
        $notice = Notice::getInstance($notice_id);
        if (!$notice->hasFlag(Notice::FLAG_SIGNUP)) continue ;
        $user->setFlag($notice, 'notices');
      }
    }
  }
  catch (Exception $e) {}

  if ($user->status <= User::STATUS_PENDING &&
      !$user->hasFlag(User::FLAG_WELCOMED)) {
    $code = P('app.name') . '.signup.confirm';
    $code = Letter::exists(array('code' => $code)) ? $code : 'signup.confirm';
    $bindings = array('email' => $user->email,
                      'name' => $user->name,
                      'password' => $password,
                      'link' => $user->confirmUrl());
    Letter::send($code, $user, $bindings);
    $user->setFlag(User::FLAG_WELCOMED);
  }

  AuthManager::onUserAuth($user);

  $user->onSignup();

  $res->disableCache();

}
catch (Exception $e) {
  $req->track('signup', Tracker::CATEGORY_ERROR);
  $ctx->_auth_errors = $e->getMessage();
  $err->add($e);
}

if (!$req->isXhr()) {
  $args = null;
  if ($ctx->hasAttribute('form_errors')) {
    $args = array('errors' => $ctx->form_errors,
                  'fields' => $ctx->form_fields);
    $res->sendRedirect($req->onerror, $args);
  }
  else {
    $args = array('fields' => $ctx->form_fields);
    $res->sendRedirect($req->onsuccess, $args);
  }
}