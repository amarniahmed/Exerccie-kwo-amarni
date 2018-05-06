<?php

class Mbox 
{

  const BACKUP_PATH = '/var/web/mboxes';
  const SPOOL_PATH = '/var/spool/knx_scan';


  private $obj = null;
  public function __construct($mail_id) {
    $mail_id = (int) $mail_id;
    $db = $GLOBALS['db'];
    $db->query("select *, UNIX_TIMESTAMP(created_at) AS t from server_mail where mail_id=$mail_id");
    $this->obj = $db->object();
  }

  public function download () {       
    header('Content-type: message/rfc822'); 
    header('Content-Disposition: inline; filename="'.$this->obj->mail_key.'.eml"');           
    header('Content-Transfer-Encoding: binary');
    header('Pragma: public');          
    $file_path = self::BACKUP_PATH.'/'.date('Y',$this->obj->t).
         '/'.date('m',$this->obj->t).'/'.date('d',$this->obj->t).'/'.$this->obj->mail_key.'.eml';
    readfile($file_path);
  }

  public static function index () {
    $mails_arr = self::getMessages();
    print_r($mails_arr);
    foreach ($mails_arr as $key => $mail_arr) {
      $db = $GLOBALS['db'];
      $insert_arr = array();
      $insert_arr['mail_key'] = "'".mysql_real_escape_string($key)."'";
      $insert_arr['subject'] = "'".mysql_real_escape_string($mail_arr['subject'])."'";
      $insert_arr['mail_from'] = "'".mysql_real_escape_string($mail_arr['from'])."'";
      $insert_arr['mail_to'] = "'".mysql_real_escape_string($mail_arr['to'])."'";
      $insert_arr['size'] = "'".mysql_real_escape_string($mail_arr['size'])."'";
      $insert_arr['created_at'] = "'".mysql_real_escape_string($mail_arr['mtime'])."'";
      print_r($insert_arr);
      return;
      $db->query("insert into server_mail ".
                 " (".join(',',array_keys($insert_arr)).") VALUES ".
                 " (".join(',',array_values($insert_arr)).")");
      $dir = self::BACKUP_PATH.'/'.$mail_arr['dir'];
      if (strlen($mail_arr['file_path'])>6) {
        exec("mkdir -p $dir");
        rename($mail_arr['file_path'],$dir.'/'.$key.'.eml');  
      }
    }
  }

  public static function getMessages () {
    $glob_arr = glob (self::SPOOL_PATH.'/*.eml');
    $files_arr = array();
    foreach ($glob_arr as $file_path) {
      $key = basename($file_path,'.eml');
      $files_arr[$key]['file_path'] = $file_path;
      $stat_arr = stat($file_path);
      $files_arr[$key]['mtime'] = date('Y-m-d H:i:s',$stat_arr['mtime']);
      $files_arr[$key]['dir'] = date('Y/m/d',$stat_arr['mtime']);
      $files_arr[$key]['size'] = $stat_arr['size'];
      $f = fopen($file_path,'r');
      $header_raw = fread($f,1024);
      fclose($f);
      $header_arr = self::getHeader($header_raw);
      
      $files_arr[$key]['subject'] = $header_arr['subject'];
      $files_arr[$key]['from']    = $header_arr['from'];
      $files_arr[$key]['to']       = $header_arr['to'];
    }
    return $files_arr;
  }

  public static function utf8dec ( $s_String ) {
    $s_String = html_entity_decode(htmlentities($s_String." ", ENT_COMPAT, 'UTF-8'));
    return substr($s_String, 0, strlen($s_String)-1);
  }

  public static function getHeader ($s) {
     $tmp_arr = preg_split("/(\r)?\n/",$s);
     $lines_arr = array();
     $n = 0;
     foreach ($tmp_arr as $line) {
       if (strlen($line)<2) break;
       if (preg_match('/^\s/',$line)) {
         $lines_arr[$n-1] .= ' '.trim($line);
         continue;
       }
       $line = trim($line);
       $lines_arr[$n] = $line;
       $n++;
       if ($n>=50) break;
     }
     if (count($lines_arr)<2) return array();
     $header_arr = array();
     foreach ($lines_arr as $line) {
       list ($k, $v) = split (':',$line);
       /*       if (preg_match('/utf/i',$v)) {
         echo "$v\n";
         $v = self::utf8dec ($v);
       }*/
       $k = trim(strtolower($k));
       $v = trim(strtolower($v));
       if (!in_array($k,array('subject','from','to'))) continue;
       $header_arr[$k] = $v;
     }
     return $header_arr;
  }


  
 
}
