<?php

class Letter extends ActiveRecord
{
  const STATUS_DRAFT = 0;
  const STATUS_PENDING = 10;
  const STATUS_ONLINE = 20;

  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_PENDING => 'en ligne, à revoir',
                                  self::STATUS_DRAFT => 'brouillon');

  const FLAG_TRACK = 1;

  public static $flags = array(self::FLAG_TRACK => 'à tracer');

  public static $colors = array(self::STATUS_ONLINE => 'green',
                                self::STATUS_PENDING => 'blue',
                                self::STATUS_DRAFT => 'red');

  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['subject']) && strlen($attrs['subject']) < 2) {
      throw new ActiveRecordException('subject required');
    }
    if (isset($attrs['body']) && strlen($attrs['body']) >= 3) {
      $attrs['body'] = SH::tidy($attrs['body']);
    }
    if (isset($attrs['code'])) {
      $attrs['name'] = $attrs['code'];
    }
  }

  public function addMetrics($metrics) {
    $dbo = S('dbo');
    $block = $metrics->getBlock('letter-metrics');
    $block->setLabel('statistiques générales');
    $values['envoi(s)'] = (int) $dbo->fetchValue('SELECT COUNT(*)'
                                                 .' FROM '.T('delivery')
                                                 .' WHERE letter_id='.$this->id);
    $h = $dbo->fetchHash('SELECT COUNT(id) AS v, SUM(conversions) AS c'
                         .' FROM '.T('visit')
                         .' WHERE source='.Item::asSource($this));
    $values['visite(s)'] = (int) $h['v'];
    $values['conversion(s)'] = (int) $h['c'];
    $block->setValues($values);

    $block = $metrics->getBlock('letter-emails');
    $block->setLabel('derniers ouvreurs');
    $rs = $dbo->query('SELECT e.email, d.user_id'
                      .' FROM '.T('delivery').' AS d,'
                      .     ' '.T('email').' AS e'
                      .' WHERE d.letter_id='.$this->getId()
                      .  ' AND d.email_id=e.id'
                      .  ' AND d.opened_at>"0000-00-00"');
    $values = array();
    foreach ($rs as $row) {
      $values[$row['email']] = $row['user_id'] > 0 ? Item::link(array(Model::id('user'), $row['user_id'])) : '';
    }
    $block->setValues($values);

    $block = $metrics->getBlock('letter-visits');
    $block->setLabel('dernières visites');
    $rs = $dbo->query('SELECT *'
                      .' FROM '.T('visit')
                      .' WHERE source='.Item::asSource($this)
                      .' ORDER BY id DESC LIMIT 20');
    $values = array();
    foreach ($rs as $row) {
      $date = $row['created_on'].' '.substr($row['created_at'], 0, 5);
      $values[$date] = Visit::link($row['id']) . ($row['conversions'] > 0 ? ' *' : '');
    }
    $block->setValues($values);
  }

  private function deliver($recipient, $opts=array()) {
    $locale = $opts['locale'];
    $attachments = (array) $opts['attachments'];
    $bindings = (array) $opts['bindings'];

    $mail = new Mail();
    $mail->addRecipient($recipient);

    $from_name = $from_email = '';
    if (isset($opts['from'])) {
      if (is_array($opts['from'])) {
        $from_email = $opts['from']['email'];
        $from_name = $opts['from']['name'];
      }
      else {
        $from_email = $opts['from'];
      }
    }
    elseif (strlen($this->from_name) > 1 && strlen($this->from_email) > 6) {
      $from_email = $this->from_email;
      $from_name = $this->from_name;
    }
    $mail->setFrom($from_email, $from_name);

    if (isset($opts['reply-to'])) {
      if (is_array($opts['reply-to'])) {
        $mail->setReplyTo($opts['reply-to']['email'],
                          $opts['reply-to']['name']);
      }
      else {
        $mail->setReplyTo($opts['reply-to'], '');
      }
    }

    if (!empty($opts['rcpt'])) {
      $mail->addHeader('X-Kwo-Rcpt', $opts['rcpt']);
    }

    $body = $this->getProperty('body', $locale);
    $subject = $this->getProperty('subject', $locale);
    if (empty($subject)) {
      $subject = $letter->name;
      S('log')->trace('letter ['.$letter->code.'] does not have a subject',
                      __METHOD__);
    }
    $keys = array();
    foreach ($bindings as $key => &$value) {
      if ($key === 'url' || ($key === 'link' && $value[0] != '<')) {
        if ($value[0] === '/') {
          if (S('req')->isCli()) {
            $value = H::url($value);
          }
          else {
              $value = 'http://'.S('req')->getServer('host').$value;
          }
        }
        $value = '<a href="'.$value.'">'.$value.'</a>';
      }
      $keys[] = $key;
    }
    $body = self::replace($body, $bindings);
    $subject = self::replace($subject, $bindings);

    $h = P('app.domain');
    $body = str_replace(array("='".DOC_URL,
                              '="'.DOC_URL,
                              "='".WEB_URL,
                              '="'.WEB_URL,
                              ' href="/'),
                        array("='http://".$h.DOC_URL,
                              '="http://'.$h.DOC_URL,
                              "='http://".$h.WEB_URL,
                              '="http://'.$h.WEB_URL,
                              ' href="http://'.$h.'/'),
                        $body);
    if (strpos($body, '((footer))') !== false) {
      $body = str_replace('((footer))', $this->footer($locale), $body);
    }

    $mail->setSubject($subject);

    $delivery = Delivery::getInstance();
    $attrs = array('letter_id' => $this->id,
                   'name' => $this->code.' - '.$recipient,
                   'email_id' => Email::asId($recipient));
    if ($opts['user']) {
      $attrs['user_id'] = $opts['user'] instanceof User ? $opts['user']->id : (int) $opts['user'];
    }
    $delivery->commit($attrs);
    $mail->addHeader('X-Kwo-Delivery', $delivery->id);
    $mail->setBody($this->getTrackedBody($body, $delivery));

    foreach ($attachments as $attachment) {
      $mail->addAttachment($attachment);
    }

    S('app')->hook('onLetterDeliver',
                   array('letter' => $this,
                         'mail' => $mail,
                         'recipient' => $recipient,
                         'bindings' => $bindings,
                         'opts' => $opts));

    $mail->send();

    try {
      $recipients = array();
      if (strlen($this->recipients) > 5) {
        $recipients = SH::asSet($this->recipients);
      }
      if ($opts['recipients']) {
        $recipients = array_merge($recipients, $opts['recipients']);
      }
      $subject = '['.$this->code.'-'.$recipient.'] '.$subject;
      foreach ($recipients as $email) {
        $mail = new Mail();
        $mail->setSubject($subject);
        $mail->setFrom($from_email, $from_name);
        foreach ($attachments as $attachment) {
          $mail->addAttachment($attachment);
        }
        $mail->setBody($body);
        $mail->addRecipient($email);
        $mail->send();
      }
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }

    return $delivery;
  }

  private function getTrackedBody($body, $delivery) {
    $suffix = 'knxdid='.$delivery->id;
    $body .= "\n".'<img src="'.H::url('/delivery.open?'.$suffix).'" />';
    $html = '<delimiter>'.$body.'</delimiter>';
    $doc = new DOMDocument();
    $doc->formatOutput = true;
    $doc->preserveWhiteSpace = true;
    $lines = array('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',
                   '<html><head>',
                   '<meta http-equiv="content-type" content="text/html; charset=utf-8" />',
                   '<meta charset="utf-8" />',
                   '</head><body>',
                   $html,
                   '</body></html>');
    $html = join("\n", $lines);
    @$doc->loadHTML($html);
    $doc->encoding = 'UTF-8';
    foreach ($doc->getElementsByTagName('a') as $node) {
      $url = $node->getAttribute('href');
      if (strpos($url, '://') === false) {
        $url = H::url($url);
      }
      $url .= strpos($url, '?') === false ? '?'.$suffix : '&'.$suffix;
      $node->setAttribute('href', $url);
    }
    $node = $doc->getElementsByTagName('delimiter')->item(0);
    $body = $doc->saveXML($node);
    $body = str_replace(array('<delimiter>', '</delimiter>'),
                        array('', ''),
                        $body);
//    S('log')->trace($body, __METHOD__);
    return $body;
  }

  private static function replace($str, $bindings) {
    if (empty($bindings) || strlen($str) <= 3 || !is_array($bindings)) {
      return $str;
    }
    $patterns = array();
    foreach ($bindings as $key => $value) {
      $patterns[] = '/\(\('.$key.'\)\)/u';
    }
    $str = preg_replace($patterns, $bindings, $str);
    return $str;
  }

  private static $footers = array();

  private function footer($locale) {
    if (!isset(self::$footers[$locale])) {
      self::$footers[$locale] = '';
      try {
        $snippet = Snippet::getInstance('letter.footer',
                                        array('ignore' => true));
        self::$footers[$locale] = $snippet->getProperty('content', $locale);
      }
      catch (Exception $e) {  }
    }
    return self::$footers[$locale];
  }

  private static $registry = array();

  public static function send($code, $recipient, $bindings=null, $opts=array()) {
    if ($recipient instanceof User) {
      $opts['user'] = $recipient;
      if ($recipient->status <= self::STATUS_DRAFT) return ;
      $opts['locale'] = $opts['locale'] >= 1 ? $opts['locale'] : $recipient->locale;
      $opts['rcpt'] = $recipient->getId();
      $recipient = $recipient->email;
    }
    elseif ($recipient instanceof Email) {
      if ($recipient->status < 1) return ;
      $recipient = $recipient->email;
    }
    elseif (is_numeric($recipient)) {
      $recipient = Email::getInstance($recipient)->email;
    }
    $opts['bindings'] = (array) $bindings;
    $opts['bindings']['((email))'] = $recipient;
    $opts['locale'] = $opts['locale'] >= 1
                    ? $opts['locale']
                    : S('res')->getLocale();
    try {
      $letter = $registry[$code] ?: Letter::getInstance($code, array('ignore' => true));
      if (!$letter->isOnline()) {
        return ;
      }
      $registry[$code] = $letter;
      try {
        $delivery = $letter->deliver($recipient, $opts);
        return $delivery;
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    catch (Exception $e) {
      if ($opts['ignore'] === true) {
        S('log')->warn('letter ['.$code.'] unknown', __METHOD__);
      }
    }
  }

}
