<?php

/*****************************************

KerniX SMTP CLIENT

- Interface : $mail
  + body()
  + bounce()
  + headers()
  + id()
  + recipients()
  + subject()
  + to()

TODO :
- parse $url (cf. parse_url)
  + tcp://login:password@localhost:25
  + ssl://login:password@localhost:25
  + tls://login:password@localhost:25

******************************************/

class SmtpClient {

  const LINE_LENGTH = 1000;

  private $cx_timeout = 5;
  private $debug_level = 0;
  private $domain = '';
  private $eol = PHP_EOL;
  private $encryption = null;
  private $last_read = null;
  private $last_write = null;
  private $lwsp = "\t";
  private $message_count = 0;
  private $port = 25;
  private $rw_timeout = 7200;
  private $server = null;
  private $socket = null;
  private $subject = null;
  private $to = null;
  private $url = null;
  
  public function __construct($url, $opts=null) { 
    $opts = (array) $opts;
    $this->url = $url;
    //    $this->url = empty($url) ? $this->url : $url;
    $this->cx_timeout = $opts['cx_timeout'] ?: $this->cx_timeout;
    $server = $this->url;
    $tls = false;
    if (strpos($server, 'tls://') === 0) {
      $tls = true;
    }
    if (strpos($server, '://') > 0) {
      $server = substr($server, 6);
    }
    $login = null;
    if (strpos($server, '@') !== false) {
      $matches = array();
      $ret = preg_match('/(\/|^)(?P<login>\w+):(?P<password>\w+)@/',
                        $server, $matches);
      $login = $matches['login'];
      $password = $matches['password'];
      list (, $server) = explode('@', $server);
    }
    if (strpos($server, '://') === false) {
      $server = 'tcp://'.$server;
    }
    if (!preg_match('/:\d+$/', $server)) {
      $server = $server.':'.$this->port;
    }
    $this->socket = @stream_socket_client($server, $errno, $errstr, 
                                          $this->cx_timeout);
    if (!is_resource($this->socket)) {
      S('log')->error('connection failure ['.$this->url.'] ('.$errstr.' - '.$errno.')', 
                      __METHOD__);
      throw new Exception('smtp connection failure');
    }
    $this->domain = $opts['identity'] ?: 'localhost';
    $this->Ehlo($this->domain);
    if ($tls === true) {
      $this->StartTLS();
    }
    if ($login != null) {
      $this->Authenticate($login, $password);
    }
  }

  public function send(Mail $mail) {
    if (stream_set_timeout($this->socket, $this->rw_timeout) === false) {
      S('log')->error('stream_set_timeout', __METHOD__);
      throw new Exception('smtp connection failure (timeout)');
    }
    $this->to = null;
    $this->subject = $mail->subject();
    $this->MailFrom($mail->bounce());
    $recipient_count = 0;
    foreach ($mail->recipients() as $email) {
      try {
        $this->Recipient($email);
        $recipient_count++;
      }
      catch (Exception $e) {
        S('log')->error('recipient failure ['.$email.']', __METHOD__);
      }
    }
    if ($recipient_count < 1) {
      S('log')->error('no recipient', __METHOD__);
      $this->Reset();
      return ;
    }
    $data = '';
    if (count($mail->to()) >= 1) {
      $this->to = join(', ', $mail->to());
      $data .= 'To: '.join(','.$this->eol.$this->lwsp, $mail->to()).$this->eol;
    }
    $data .= 'Subject: '.$this->subject.$this->eol;
    $data .= 'Message-ID: <'.$mail->id().'@'.$this->domain.'>'.$this->eol;
    foreach ($mail->headers() as $key => $value) {
      $data .= $key.': '.$value.$this->eol;
    }
    $data .= $this->eol;
    $body = $mail->body();
    if (strlen($body) <= 1) {
      S('log')->error('empty body', __METHOD__);
      $this->Reset();
      return ;
    }
    $data .= $body;
    $this->Data($data);
    $this->Reset();
    $this->message_count++;
  }

  public function Ehlo($host) {
    $this->call(null, 220);
    $this->call('EHLO '.$host, 250);
  }

  public function MailFrom($from) {
    $this->call('MAIL FROM:<'.$from.'>', 250);
  }

  public function Recipient($to) {
    $this->call('RCPT TO:<'.$to.'>', array(250, 251));
  }

  public function Data($data) {
    $data = wordwrap($data, self::LINE_LENGTH - 2, $this->eol, true);
    $this->call('DATA', 354);
    $this->call($data.'.', 250);
  }

  public function Reset() {
   $this->call('RSET', 250);
  }

  public function Quit() {
    $this->call('QUIT', 221);
  }

  public function Noop() {
    $this->call('NOOP', 250); 
  }

  public function StartTLS() {
    $this->call('STARTTLS', 220);
    if (!stream_socket_enable_crypto($this->socket, true, 
                                     STREAM_CRYPTO_METHOD_TLS_CLIENT)) {
      S('log')->error('stream_socket_enable_crypto', __METHOD__);
      throw new Exception('stream_socket_enable_crypto');
    }
  }

  public function Authenticate($login, $password) { 
    $this->call('AUTH LOGIN', 334);
    $this->call(base64_encode($login), 334);
    $this->call(base64_encode($password), 235);
  }

  public function call($cmd, $codes) {
    $codes = (array) $codes;
    if (!empty($cmd)) {
      $this->write($cmd);
    }
    $rply = $this->read();
    $code = (int) substr($rply, 0, 3);
    if (!in_array($code, $codes)) {
      $this->error();
    }
  }

  private function write($cmd) {
    $this->ping();
    $this->last_read = null;
    $this->last_write = $cmd;
    if (@fwrite($this->socket, $cmd . $this->eol) === false) {
      $this->error();
    }
    if ($this->debug_level > 0) {
      S('log')->debug($this->last_write, __METHOD__);
    }
  }

  private function read() {
    $this->last_read = @fread($this->socket, 1024);
    if ($this->last_read === false) {
      $this->error();
    }
    if ($this->debug_level > 0) {
      S('log')->debug(trim($this->last_read), __METHOD__);
    }
    return $this->last_read;
  }

  public function close() {
    if (is_resource($this->socket)) {
      $this->Quit();
      if (fclose($this->socket) === false) {
        S('log')->error('fclose failure', __METHOD__);
      }
    }
    $this->socket = null;
    self::$registry[$this->url] = null;
  }

  public function ping() {
    if (!is_resource($this->socket)) {
      S('log')->trace('invalid socket', __METHOD__);
      $this->error();
      return ;
    }
    $metas = stream_get_meta_data($this->socket);
    if ($metas['timed_out'] === true) {
      S('log')->error('socket timeout', __METHOD__);
      $this->error();
    }
  }

  private function debug($msg, $level) {
    if ($this->debug_level < $level) return ;
    S('log')->debug($msg, __METHOD__);
  }

  private function error() {
    //    $mail->recipients();
    S('log')->error('T : '.$this->to, __METHOD__);
    S('log')->error('S : '.$this->subject, __METHOD__);
    S('log')->error('> : '.$this->last_write, __METHOD__);
    S('log')->error('< : '.$this->last_read, __METHOD__);
    if (is_resource($this->socket)) {
      if (fclose($this->socket) === false) {
        S('log')->error('fclose failure', __METHOD__);
      }
    }
    $this->socket = null;
    self::$registry[$this->url] = null;
    throw new Exception('smtp error ['.$this->subject.']');
  }

  public function __destruct() {
    $this->close();
  }

  private static $registry = array();
  private static $ts = null;

  public static function getInstance($url, $opts=null) {
    $url = empty($url) ? 'localhost' : $url;
    self::$ts = self::$ts ?: time();
    if ((time() - self::$ts) > 300) {
      self::$ts = time();
      if (self::$registry[$url]) {
        self::$registry[$url]->close();
      }
      self::$registry[$url] = null;
    }
    if (self::$registry[$url] === null) {
      self::$registry[$url] = new SmtpClient($url, $opts);
    }
    return self::$registry[$url];
  }

  //----------------------------------------------------------------------------
  
  public static function validate($email, $host=null) {

    try {
      if ($host === null) {
        list(, $domain) = explode('@', $email);
        $domain = strtolower($domain);
        $hosts = $weights = array();
        if (getmxrr($domain, $hosts, $weights) === false) return true;
        $hosts = array_combine($hosts, $weights);
        asort($hosts);
        $host = key($hosts);
      }
      
      $from = P('mail.from.email');
      list (, $ehlo) = explode('@', $from);

      $eol = "\r\n";
      $socket = @stream_socket_client('tcp://'.$host.':25',
                                      $errno, $errstr, 5);
      if (!is_resource($socket)) return true;
      if (stream_set_timeout($socket, 5) === false) return true;

      $res = @fread($socket, 1024);
      if (intval(substr($res, 0, 3)) != 220) throw new Exception($res);

      if (@fwrite($socket, 'EHLO '.$ehlo.$eol) === false) throw new Exception($res);
      $res = @fread($socket, 1024); 
      if (intval(substr($res, 0, 3)) != 250) throw new Exception($res);
      
      if (@fwrite($socket, 'MAIL FROM:<'.$from.'>'.$eol) === false) throw new Exception($res);
      $res = @fread($socket, 1024); 
      if (intval(substr($res, 0, 3)) != 250) throw new Exception($res);
      
      if (@fwrite($socket, 'RCPT TO:<'.$email.'>'.$eol) === false) throw new Exception($res);
      $res = @fread($socket, 1024); 
      if (substr($res, 0, 2) != '25') {
        S('log')->trace($res, $email);
        if ($res[0] == '4' && preg_match('/(rejected)/i', $res)) throw new Exception($res, 1);
        if ($res[0] == '5' && !preg_match('/(fai|spam|rtr|dynamyc)/i', $res)) throw new Exception($res, 1);
      }

      if (is_resource($socket)) {
        fwrite($socket, 'QUIT'.$eol);
        fclose($socket);
      }
    }
    catch (Exception $e) { 
      if (is_resource($socket)) {
        $info = stream_get_meta_data($socket);
        if ($info['timed_out']) {
          S('log')->trace('timeout', $email);
          return false;
        }
        fclose($socket);
      }
      if ($e->getCode() == 1) return false;
      S('log')->error($e, $email);
    }
    return true;
  }

}


