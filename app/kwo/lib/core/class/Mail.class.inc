<?php

/***
for qmail : \n instead of \r\n
***/

class Mail
{
  private $attachments = array();
  private $bcc         = array();
  private $bodies      = array();
  private $body        = '';
  private $bounce      = null;
  private $boundary0   = null;
  private $boundary1   = null;
  private $charset     = 'UTF-8';
  private $eol         = PHP_EOL;
  private $from        = null;
  private $headers     = array();
  private $is_mime     = false;
  private $lwsp        = "\t";
  private $recipients  = array();
  private $smtp        = null;
  private $subject     = null;
  private $subject_raw = null;
  private $template    = '/mail';
  private $to          = array();
  private $url         = null;
  public  $md5         = null;
  public  $sign_dkim   = false;

  const BODY_TEXT = 'text/plain';
  const BODY_HTML = 'text/html';

  const DKIM_PRIV = 'lib/core/opt/dkim/dkim.priv';
  const DKIM_PUB = 'lib/core/opt/dkim/dkim.pub';
  const DKIM_SELECTOR = 'kwo';

  public function __construct() {
    if ($this->md5 === null) {
      $this->md5 = hash('md5', uniqid(rand()));
    }
    $this->boundary0 = '00-kwo-'.$this->md5;
    $this->boundary1 = '01-kwo-'.$this->md5;
  }

  public function setSubject($subject) {
    if (preg_match('/’/', $subject)) {
      $subject = preg_replace('/’/u', "'", $subject);
    }
    $this->subject_raw = $subject;
    $this->subject = $this->encodeHeader($subject);
    return $this;
  }

  public function getSubject() {
    return $this->subject;
  }
  
  public function setBodies($bodies) {
    $this->bodies = $bodies;
  }

  public function getBodies() {
    return $this->bodies;
  }

  public function getRecipients() {
    return $this->bodies;
  }

  public function setHeaders($headers) {
    $this->headers = $headers;
  }

  public function getHeaders() {
    return $this->headers;
  }

  public function setTo($to) {
    $this->to = $to;
  }

  public function getTo() {
    return $this->to;
  }

  private function encodeHeader($str) {
    $str = mb_encode_mimeheader($str, 'UTF-8', 'B', "\n");
    return $str;
  }

  public function setFrom($email, $name=null) {
    if (empty($email)) return $this;
    if ($email instanceof User) {
      if (strlen($email->name) > 0) {
        $name = $email->name.' - '.P('mail.from.name');
      }
      $email = $email->email;
    }
    $email = $this->cleanValue($email);
    $name = $this->cleanValue($name);
    $email = $this->looksLikeEmail($email) ? $email : P('mail.from.email');
    $name = empty($name) ? P('mail.from.name') : $name;
    if ($this->charset === 'UTF-8') {
      $name = $this->encodeHeader($name);
    }
    $this->from = '"'.$name.'" <'.$email.'>';
    $this->addHeader('From', $this->from);
    return $this;
  }

  public function setReplyTo($email, $name='') {
    $email = $this->cleanValue($email);
    $name = $this->cleanValue($name);
    if (!$this->looksLikeEmail($email)) {
      S('log')->warn($email, __METHOD__);
      return ;
    }
    $str = $email;
    if (!empty($name)) {
      if ($this->charset === 'UTF-8') {
        $name = $this->encodeHeader($name);
      }
      $str .= ' <'.$name.'>';
    }
    $this->addHeader('Reply-To', $str);
    return $this;
  }

  public function setBounce($email) {
    $this->bounce = $email;
    return $this;
  }

  public function setUrl($url) {
    $this->url = $url;
    return $this;
  }

  public function addRecipient($recipient, $name='') {
    if (is_string($recipient) && strpos($recipient, ',') > 1) {
      $recipient = explode(',', $recipient);
      $recipient = array_map('trim', $recipient);
    }
    if (is_array($recipient)) {
      foreach ($recipient as $email) {
        $this->addRecipient($email);
      }
      return ;
    }
    if ($recipient instanceof User) {
      $email = $recipient->email;
      $this->addHeader('X-Kwo-Rcpt', $recipient->getId());
    }
    else {
      $email = $recipient;
    }
    return $this->dispatchRecipient($email, $name, 'To');
  }

  public function setRecipient($recipient, $name='') {
    $this->recipients = $this->to = array();
    return $this->dispatchRecipient($recipient, $name, 'To');
  }

  public function addCc($email, $name='') {
    if (strpos($email, ',') > 3) {
      $emails = explode(',', $email);
      foreach ($emails as $email) {
        $email = trim($email);
        $this->dispatchRecipient($email, '', 'Cc');
      }
      return ;
    }
    return $this->dispatchRecipient($email, $name, 'Cc');
  }

  public function addBcc($email) {
    if (strpos($email, ',') > 3) {
      $emails = explode(',', $email);
      foreach ($emails as $email) {
        $email = trim($email);
        $this->dispatchRecipient($email);
      }
      return ;
    }
    return $this->dispatchRecipient($email, '', 'Bcc');
  }

  public function looksLikeEmail($email) {
    return strlen($email) >= 6 && substr_count($email, '@') === 1;
  }

  private function dispatchRecipient($email, $name='', $header='') {
    $email = $this->cleanValue($email);
    
    S('log')->trace((P('app.domain')));
    
    $patterns = array('.pulsar', '.kwo', 'devel', 'test', 'preprod');
    foreach ($patterns as $pattern) {
      if (strpos(P('app.domain'), $pattern) === false) continue ;
      if (strpos($email, '@kernix.com') === false) {
        S('log')->trace('invalid recipient ['.$email.']('.$header.') on a devel instance ['.P('app.domain').']',
                        __METHOD__);
        return false;
      }
    }
    
    $name = $this->cleanValue($name);
    if (!$this->looksLikeEmail($email)) {
      S('log')->warn($email, __METHOD__);
      return false;
    }
    if (count($this->recipients) > 100) {
      S('log')->trace('too many recipients', __METHOD__);
      return false;
    }
    $this->recipients[] = $email;
    if (!empty($header)) {
      if (empty($name)) {
        $value = $email;
      }
      else {
        $value = '"'.$name.'" <'.$email.'>';
      }
      if ($header === 'To') {
        $this->to[$email] = $value;
      }
      elseif ($header === 'Cc') {
        if (!empty($this->headers[$header])) {
          $this->headers[$header] .= ','.$this->eol.$this->lwsp;
        }
        $this->headers[$header] .= $value;
      }
      elseif ($header === 'Bcc') {
        $this->bcc[] = $email;
      }
    }
    return true;
  }

  private function cleanValue($value) {
    $value = trim($value);
    $safe = $value;
    if (empty($value)) return $value;
    $value = strtr($value, '":<>', '    ');
    $value = preg_replace('/(\\|\n|\r|\')/m', ' ', $value);
    if ($safe != $value) {
      S('log')->error('from ['.$safe.'] to ['.$value.']', __METHOD__);
    }
    return $value;
  }

  public function addHeader($key, $value) {
    if ($key === 'Bcc') {
      S('log')->warning('Bcc not allowed', __METHOD__);
      return $this;
    }
    if (strlen($key) >= 32) {
      S('log')->error('header error for msg ['.$this->subject.'] '.
                      '['.substr($key, 0, 8).':'.substr($value, 0, 8).']',
                      __METHOD__);
      return $this;
    }
    $this->headers[$key] = trim($value);
    return $this;
  }

  public function setBodyHtml($content) {
    $this->is_mime = true;
    if (strpos($content, '="/') > 1) {
      $prefix  = P('app.url.secure') ? 'https://' : 'http://';
      if (P('app.url.user')) {
        $prefix .= P('app.url.user');
        if (P('app.url.password')) {
          $prefix .= ':'.P('app.url.password');
        }
        $prefix .= '@';
      }
      $prefix .= P('app.domain');
      $prefix .= P('app.url.port') ? ':'.P('app.url.port') : '';
      $content = str_replace(array(' src="/', ' href="/'),
                             array(' src="'.$prefix.'/', ' href="'.$prefix.'/'),
                             $content);
    }
    $this->bodies[] = array('content_type' => self::BODY_HTML,
                            'content' => $content);
    return $this;
  }

  public function setBodyText($content) {
    $this->bodies[] = array('content_type' => self::BODY_TEXT,
                            'content' => $content);
    return $this;
  }

  public function setBody($content, $template=true) {
    $content = is_array($content) ? $this->hash2html($content) : $content;
    if (!empty($template)) {
      if ($template === true) {
        $template = P('mail.template') ?: $this->template;
      }
      $content = Template::render($template,
                                  array('body' => $content,
                                        'hostname' => P('app.domain')));
    }

    if (stripos($content, '<html') !== false) {
      $this->setBodyHtml($content);
    }
    else {
      $this->setBodyText($content);
    }
    return $this;
  }

  protected function hash2html($hash) {
    $html = '<table class="grid"><tbody>';
    foreach ($hash as $key => $value) {
      $html .= '<tr><td class="left">'.$key.'</td>';
      $html .= '<td>'.$value.'</td></tr>';
    }
    $html .= '</tbody></table>';
    return $html;
  }

  public function addAttachment($uri, $data=null) {
    if (empty($uri)) return $this;
    if (strpos($uri, '://') === false) $uri = 'file://'.$uri;
    if (substr($uri, 0, 7) === 'file://') {
      $uri = substr($uri, 7);
      try {
	$file = new File($uri);
	$content = $file->getContent();
      }
      catch (Exception $e) {
        return S('log')->warn('attachment error ['.$uri.']', __METHOD__);
      }
    }
    elseif (substr($uri, 0, 6) === 'mem://') {
      $uri = substr($uri, 6);
      $content = $data;
    }
    if (strlen($content) < 1) {
      S('log')->warn('attachment ['.$uri.'], empty content ', __METHOD__);
      return $this;
    }
    $filename = basename($uri);
    $this->attachments[$filename]['filename'] = $filename;
    $this->attachments[$filename]['content_type'] = Mime::getContentType($filename);
    $this->attachments[$filename]['content'] = chunk_split(base64_encode($content));
    $this->is_mime = true;
    return $this;
  }

  public function send($server=null) {
    $header = '';
    $content = '';
    $sent_count = 0;

    if ($this->subject === null) {
      $this->subject = 'no subject';
    }
    if ($this->from === null) {
      $this->setFrom(P('mail.from.email'), P('mail.from.name'));
    }
    
    if (P('mail.redirect')) {
      $this->recipients = explode(';', P('mail.redirect'));
      $this->bcc = array(); 
      $this->to = array(); 
    } 

    if (count($this->recipients) < 1 || count($this->bodies) < 1) {
      S('log')->error('can not send mail ['.substr($this->subject, 0, 12).'] to ['.$this->recipients[0].']'.
                      ' ('.count($this->recipients).')', __METHOD__);
      return false;
    }

    if ($this->is_mime === true) {
      $this->addHeader('Content-Transfer-Encoding', '8bit');
      $this->addHeader('MIME-Version', '1.0');

      $content_type = count($this->attachments) >= 1
                    ? 'multipart/mixed'
                    : 'multipart/alternative';
      $this->setHeaderContentType($content_type);

      $content .= $this->url !== null
                ? 'Cliquez ici si vous souhaitez visualiser cette lettre au format html : '.$this->eol.$this->url
                : 'This is a multi-part message in MIME format.';
      $content .= $this->eol;

      if (count($this->attachments) >= 1) {
        $content .= $this->openPart('multipart/alternative');
      }
//      $bodies = array_reverse($this->bodies);
      $bodies = $this->bodies;
      foreach ($bodies as $body) {
        $content .= $this->openPart($body['content_type']);
        $content .= $body['content'];
        $content .= $this->eol;
      }
      if (count($this->attachments)>=1) {
        $content .= $this->closePart($this->boundary1);
      }
      
      foreach ($this->attachments as $file) {
        $content .= $this->openPart($file['content_type'],$file);
        $content .= $file['content'].$this->eol.$this->eol;
      }
      $content .= $this->closePart($this->boundary0);
    }
    else {
      // mb_detect_encoding($body['content']) -> UTF-8
      $this->addHeader('Content-Type', 'text/plain; charset=UTF-8; format=flowed');
      $this->addHeader('Content-Transfer-Encoding', '8bit');
      foreach ($this->bodies as $body) {
        $content .= $body['content'].$this->eol;
      }
    }
    
    $this->body = $content;
    $this->canonicalizeBody();
    
    $this->headers['X-Kwo-App'] = P('app.name');
    $this->headers['MIME-Version'] = '1.0';
    foreach ($this->headers as $k => $v) {
      $header .= $k.': '.$v.$this->eol;
    }
    
    try {
      if ($this->sign_dkim) {
        // TODO : ajouter le dkim si webmail
        $this->addDkim();
      }
      $this->sendMta($server);
    }
    catch (Exception $e) { return false; }
    return true;
  }

  public function canonicalizeBody() {
    $body = trim($this->body);
    $body = $body.$this->eol;
    $body = str_replace("\r\n", "\n", $body);
    $body = str_replace("\n", $this->eol, $body);
    if (strpos($body, "\n.\r") !== false) {
      S('log')->trace('single point', __METHOD__);
      $body = preg_replace('/^\.(\n|\r|$)/m', $this->eol, $body);
    }
    $this->body = $body;
  }

  private function scan($body) {
    $l = strlen($body);
    for ($i = 0; $i < $l; $i++) {
      if ($body[$i] == "\r") print("'OD'");
      elseif ($body[$i] == "\n") print("'OA'\n");
      elseif ($body[$i] == "\t") print("'09'");
      elseif ($body[$i] == " ") print("'20'");
      else print($body[$i]) ;
    }
  }

  private static $dkim_priv_key = null;

  private function addDkim() {
    $domain = 'kernix.com';
    $dkim  = 'v=1; a=rsa-sha1; c=relaxed/simple; q=dns/txt; s='.self::DKIM_SELECTOR.'; ';
    $dkim .= 'i=@'.$domain.'; d='.$domain.';'.$this->eol.$this->lwsp;
    $dkim .= 'h=Subject:From:Reply-To:MIME-Version; '.$this->eol.$this->lwsp;
    $dkim .= 't='.time().'; '.$this->eol.$this->lwsp;

    $bh = base64_encode(pack('H*', sha1($this->body)));
    $dkim .= 'bh='.$bh.'; '.$this->eol.$this->lwsp;
    $dkim .= 'b=';

    $headers  = 'subject:'.$this->subject.$this->eol;
    $headers .= 'from:'.$this->headers['From'].$this->eol;
    $headers .= 'reply-to:'.$this->headers['Reply-To'].$this->eol;
    $headers .= 'mime-version:'.$this->headers['MIME-Version'].$this->eol;
    $headers .= 'dkim-signature:'.preg_replace('/\s+/', ' ', preg_replace("/\r\n\s+/", ' ', $dkim)).';';
    if (self::$dkim_priv_key === null) {
      self::$dkim_priv_key = file_get_contents(self::DKIM_PRIV);
    }
    if (openssl_sign($headers, $signature, self::$dkim_priv_key)) {
      $b = base64_encode($signature) ;
    }
    else {
      return false;
    }
    $dkim .= $b.';';
    $this->addHeader('DKIM-Signature', $dkim);
  }

  private function sendMta($server) {
    $server = trim($server);
    $server = empty($server) ? P('mail.smtp.default') : $server;
    if (!empty($server) && $server != 'bsmtp') {
      $opts = array('identity' => P('app.domain'));
      $smtp_client = SmtpClient::getInstance($server, $opts);
      $smtp_client->send($this);
      return ;
    }
    if ($server == '/dev/null') return ;
    $bounce = $this->bounce();
    $to = join(','.$this->eol.$this->lwsp, $this->to());
    $subject = $this->subject();
    $body = $this->body();
    $header = '';
    foreach ($this->headers() as $key => $value) {
      $header .= $key.': '.$value.$this->eol;
    }
    if (count($this->bcc) >= 1) {
      $header .= 'Bcc: '.join(', ', $this->bcc).$this->eol;
    }
    if ($server == 'bsmtp') {
      $raw  = 'MAIL FROM: '.$bounce."\n";
      $raw .= 'RCPT TO: '.$to."\n";
      $raw .= str_replace(array("\r"), array(''), $header);
      $raw .= 'Subject: '.$subject."\n".'-------------'."\n";
      $raw .= str_replace(array("\r"), array(''), $body);
      $raw .= '.';
      $to = str_replace(array('.', '/'),
                        array('_', '_'),
                        $to);
      file_put_contents('/tmp/mail-'.$to.'.eml', $raw);
      return true;
    }
    return mail($to, $subject, $body, $header, '-f'.$bounce);
  }

  public function bounce() {
    if (!empty($this->bounce)) {
      return $this->bounce;
    }
    if (strlen(P('mail.bounce')) >= 5) {
      return P('mail.bounce');
    }
    S('log')->error('no bounce email', __METHOD__);
    throw new Exception('mail error');
  }

  public function headers() { return  $this->headers; }

  public function body() {
    return $this->body;
  }

  public function subject() { return $this->subject; }
  public function recipients() { return $this->recipients; }
  public function to() { return $this->to; }
  public function id() { return $this->md5; }

  private function setHeaderContentType($content_type) {
    $this->headers['Content-Type']  = $content_type.';'.$this->eol;
    $this->headers['Content-Type'] .= $this->lwsp.'boundary="'.$this->boundary0.'"';
  }

  private function openPart($content_type, $file=null) {
    $ret = '';
    //if (substr($content_type, 0, 4) === 'text') { /*text/csv*/
    if (substr($content_type, 0, 4) === 'text' && $file == null) {
      $boundary = count($this->attachments) >= 1 ? $this->boundary1 : $this->boundary0;
      $ret .= $this->eol.'--'.$boundary.$this->eol;
      $ret .= 'Content-Type: '.$content_type.'; charset='.$this->charset.$this->eol;
    }
    elseif ($content_type === 'multipart/alternative') {
      $ret .= $this->eol.'--'.$this->boundary0.$this->eol;
      $ret .= 'Content-Type: '.$content_type.';'.$this->eol;
      $ret .= $this->lwsp.'boundary="'.$this->boundary1.'"'.$this->eol;
    }
    elseif ($file != null) {
      $ret .= $this->eol.'--'.$this->boundary0.$this->eol;
      $ret .= 'Content-Type: '.$content_type.';'.$this->eol.'  name="'.$file['filename'].'"'.$this->eol;
      $ret .= 'Content-Transfer-Encoding: base64'.$this->eol;
      $ret .= 'Content-Disposition: inline;'.$this->eol.'  filename="'.$file['filename'].'"'.$this->eol;
    }
    $ret .= $this->eol;
    return $ret;
  }

  private function closePart($boundary) {
    return $this->eol.'--'.$boundary.'--'.$this->eol.$this->eol;
  }

  public function useSmtp($server) {
    $this->smtp = $server;
  }

  public function reset() {
    $this->attachments = array();
  }

  public static function getInstance() {
    $class = __CLASS__;
    return new $class;
  }

  public static function alarm($content, $recipients=null) {
    $recipients = $recipients ?: array(P('contact.admin'), P('contact.devel'));

    $head  = P('app.name') . ' - ' . P('app.domain') . "\n" . $content;
    $head .= "\n" . '--------------------------------' . "\n";
    $content = $head . $content;

    $mail = Mail::getInstance();
    $mail->addRecipient($recipients);
    $mail->setSubject(P('app.name').' : alarm');
    $mail->setBody($content, false);
    $mail->send();
  }

}

