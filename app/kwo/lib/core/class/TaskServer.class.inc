<?php

// http://blog.motane.lu/2009/01/02/multithreading-in-php/
// https://sites.google.com/a/van-steenbeek.net/archive/php_pcntl_fork?q=php_pcntl_fork

class TaskServer
{
  const SOCKET = 'tcp://127.0.0.1:64000';
  const MAX_TASKS = 64;
  const PID_FILE = 'var/run/task.server.pid';

  private static $pid = 0;

  private $socket = null;
  private $trace = true;

  public function __construct() {
    $this->trace('server');
    $this->socket = stream_socket_server(self::SOCKET, 
                                         $errno, $errstr, 
                                         STREAM_SERVER_BIND|STREAM_SERVER_LISTEN);
    if (!$this->socket) {
      S('log')->error($errstr.' ('.$errno.')');
      throw new Exception('socket server error');
    }
    $this->trace('installing signal handler');

/*    $ret = pcntl_signal(SIGTERM, array($this, 'handleSignal'));
    $ret = pcntl_signal(SIGUSR1, array($this, 'handleSignal')); */
//    pcntl_signal(SIGCHLD, array($this, 'handleSignal'));
//    pcntl_wait($status, WNOHANG);
    pcntl_signal(SIGCHLD, SIG_IGN);
    pcntl_signal(SIGTERM, SIG_DFL);
    pcntl_signal(SIGINT, SIG_DFL);

    $this->listen();
  }
  
  public function listen() {
    $this->trace('listening');
//    stream_set_blocking($this->socket, 1);
/*    $r = array($this->socket);
    $w = $e = null; */
//    while (true) {
/*      $num = @stream_select($r, $w, $e, null);
      if ($num === false) {
        // occurs when a signal is sent
        S('log')->error('stream_select failure');
        continue ;
      } */

    while ($stream = stream_socket_accept($this->socket, -1)) {
      $this->trace('new connection');
      $content = stream_get_contents($stream);
      fclose($stream);
      S('log')->trace($content);
      $cmd = substr($content, 0, 4);
      if ($cmd === 'ping') {
        sleep(3);
        S('log')->trace('pong', __METHOD__);
      }
      elseif ($cmd === 'quit') {
        $this->quit();
        return ;
      }
      elseif ($cmd === 'task') {
        list (, $task_id) = explode(' ', $content);
//        $task_id = (int) $content;
        try {
          //          $taks = 
          $pid = pcntl_fork();
          if ($pid) {
            // parent
            $task = Task::getInstance($task_id);
            $task->setAttribute('pid', $pid);
            S('dbo')->disconnect();
          }
          else {
//              $cmd = Shell::PHP_BIN . ' ' . Platform::BOOTSTRAP . ' ' . 'test/task.sleep';
//              fclose(STDIN);
//              fclose(STDOUT);
//              fclose(STDERR);
//              $sid = posix_setsid();
//              system($cmd, $ret);

//            sleep(5);
//            S('log')->trace('ici '.$cmd);
            $task = Task::getInstance($task_id);
            $task->process();
            exit (0);
          }
        }
        catch (Exception $e) { S('log')->error($e); }
      }
      //      }
//      sleep(3);
    }
//    fclose($stream); 
  }

  private function handleSignal($signo) {
    $this->trace('catch signal ('.$signo.')');
    if ($signo == SIGTERM) $this->quit();
  } 

  private function quit() {
    $this->trace('quit');
    $this->shutdown();
  }

  private function shutdown() {
    if (is_file(self::PID_FILE)) {
      unlink(self::PID_FILE);
    }
    if (is_resource($this->socket)) {
      fclose($this->socket);
    }
  }

  private function trace($msg) {
    if ($this->trace == false) return ;
    S('log')->trace($msg);
  }

  public function __destruct() {
    $this->shutdown();
  }

//  private static $singleton = null;

  public static function getInstance() {
    if (self::isAlive()) return 0;
    S('dbo')->disconnect();
    $pid = pcntl_fork();
    if ($pid == -1) {
      S('log')->error('fork failure', __METHOD__);
      throw new Exception('cannot fork');
    }
    if ($pid > 0) {
      // father
      $ret = @file_put_contents(self::PID_FILE, $pid);
      if ($ret === false) {
        S('log')->error('error occurred while writing file '.
                        '[/proc/'.$pid.'/cmdline]',
                        __METHOD__);
        throw new Exception('pid failure');
      }
      return $pid;
    }
    fclose(STDIN);
    fclose(STDOUT);
    fclose(STDERR);
    new TaskServer();
  }


  private static function isAlive() {
    
    $log = S('log');
    if (!is_file(self::PID_FILE)) {
      return false;
    }
    
    $pid = (int) @file_get_contents(self::PID_FILE);
    
    if ($pid === false) {
      $log->error('error occurred while reading pid file '.
                  '['.self::PID_FILE.']', __METHOD__);
      return false;
    }

    if (!is_dir('/proc/'.$pid)) {
      unlink(self::PID_FILE);
      $log->error('directory not found [/proc/'.$pid.']', __METHOD__);
      return false;
    }

    $cmd_line = @file_get_contents('/proc/'.$pid.'/cmdline');
    if ($cmd_line === false) {
      unlink(self::PID_FILE);
      $log->error('error occurred while reading file '.
                  '[/proc/'.$pid.'/cmdline]', __METHOD__);
      return false;
    }
    
    if (!(strpos($cmd_line, 'task.server.start') >= 0)) {
      unlink(self::PID_FILE);
      $log->error('bad process', __METHOD__);
      return false;
    }

    return true;

  }

  public static function call($msg) {
    if ($msg instanceof Task) {
      $msg = 'task '.$msg->id;
    }
    $stream = stream_socket_client(self::SOCKET, $errno, $errstr, 30);
    fwrite($stream, $msg);
    fclose($stream);
  }
  
  public static function clean() {
    if (file_exists(self::PID_FILE)) {
      unlink(self::PID_FILE);
    }
  }


}

//    pcntl_signal(SIGUSR1, array($this, 'processQueue'));
//    pcntl_signal(SIGUSR2, array($this, 'processLast'));
  //posix_kill
