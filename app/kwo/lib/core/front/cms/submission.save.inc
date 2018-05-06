<?php

try {

  $req->cleanAttributes();

  $form = Form::getInstanceByKey($req->item);

  if (!$form->isA('form')) {
    throw new MaliciousException();
  }

  if ($form->hasFlag(Form::FLAG_AUTH) && !$req->isAuthenticated()) {
    $user = $req->getUser();
  }

  $record = (array) $req->record;
  foreach ($record as $id => $value) {
    if (is_array($value)) {
      $record[$id] = join(',', $value);
    }
  }

  $failures = array();
  $fields = array();
  $name = array();
  foreach ($form->getFields() as $field) {
    if (!$field->isOnline()) continue ;
    if ($field->type == Field::TYPE_TITLE) continue ;
    $id = $field->getId();
    $fields[$id] = $field->code;
    $value = $record[$id];
    if ($field->hasFlag(Field::FLAG_IN_NAME)) {
      $name[] = $value;
    }
    if ($field->isRequired() && empty($value)) {
      $failures[] = $id;
      continue ;
    }
    if ($field->isRequired() && $field->control_type == Field::CONTROL_EMAIL) {
      if (!Email::isValid($value)) {
        $failures[] = $id;
        continue ;
      }
    }
    if ($field->isRequired() && $field->control_type == Field::CONTROL_DATE) {
      if (!DH::isValid($value)) {
        $failures[] = $id;
        continue ;
      }
    }
  }

  $tmp = array_flip($fields);

  if ($form->hasFlag(Form::FLAG_CAPTCHA) && !$req->hasValidCaptcha()) {
    $failures[] = 'captcha';
  }

  if (count($failures) > 0) {
    $ctx->failures = $failures;
    if (!$req->isXhr()) {
      $key = 'form'.$form->getId();
      $parameters[$key]['fields'] = $req->record;
      $parameters[$key]['failures'] = $failures;
      list ($url, $query) = explode('?', $url);
      if (!empty($query)) {
        $hash = array();
        parse_str($query, $hash);
        unset($hash[$key]);
        $parameters = $parameters + $hash;
      }
      $url = H::url($req->getClient('referer'), $parameters);
      $res->sendRedirect($url);
    }
    return ;
  }

  $attrs = array('form_id' => $form->getId(),
                 'locale' => $res->getLocale(),
                 'name' => join(',', $name));

  $submission = Submission::getInstance();
  $submission->commit($attrs);
  $submission->setVisit($req);

  if ($form->hasFlag(Form::FLAG_CONVERSION)) {
    $submission->convert();
  }

  $form->track(Interaction::TYPE_SUBMIT,
               array('value' => $submission->getId()));

  if ($form->hasFlag(Form::FLAG_ALERT) && strpos($form->recipients, '@') > 1) {
    $lines = array();
    foreach ($record as $field_id => $value) {
      if (strpos($value, UploadManager::path()) === 0 && file_exists($value)) {
        $url = H::url($value);
        $value = '<a href="'.$url.'" target="_blank">'.$value.'</a>';
        if (File::isImage($value)) {
          $value .= "\n".'<img src="'.$url.'" style="max-width:300px; max-height:150px; margin-top:6px;" />';
        }
      }
      $lines[$fields[$field_id]] = nl2br($value);
    }
    try {
      $mail = new Mail();
      $mail->addRecipient(SH::asSet($form->recipients));
      if (array_key_exists('email', $tmp)) {
        $mail->setFrom($record[$tmp['email']]);
      }
      $mail->setSubject('Réponse au formulaire : '.$form->code);
      $bindings = array('fields' => $lines
                        + array('* SUBMISSION ID' => $submission->getId(),
                                '* FORM ID' => $form->getId(),
                                '* IP' => $req->getClient('address'),
                                '* HOST' => $req->getClient('host'),
                                '* DATE' => DH::now()));
      if ($req->isAuthenticated()) {
        $user = $req->getUser();
        $bindings['fields']['* USER'] = $user->email;
        $bindings['fields']['* USER ID'] = $user->getId();
      }
      $body = Template::render('submission.mail', $bindings);
      $mail->setBodyHtml($body);
      $mail->send();
    }
    catch (Exception $e) {}
  }

  $records = array();
  foreach ($record as $field_id => $value) {
    $records[] = array('field_id' => $field_id,
                       'submission_id' => $submission->getId(),
                       'value' => $value);
  }
  $dbo->insert(Submission::TABLE_FIELDS, $records);

  S('app')->hook('onSubmissionAdd', array('submission' => $submission));

  if ($form->hasFlag(Form::FLAG_ACK_SEND) &&
      Email::isValid($record[$tmp['email']])) {
    $mail = new Mail();
    $mail->addRecipient($record[$tmp['email']]);
    $mail->setSubject($form->getProperty('ack_subject'));
    $mail->setBody($form->getProperty('ack_body'));
    $mail->send();
  }

  $ctx->submission_id = $submission->getId();
  $ctx->ack = $form->getProperty('ack_content');

  if (!$req->isXhr()) {
    $res->useTemplate();
  }

}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) { $err->add($e); }