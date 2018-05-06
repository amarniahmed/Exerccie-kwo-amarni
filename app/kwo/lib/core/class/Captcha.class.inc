<?php

class Captcha
{
  const WIDTH  = 75;
  const HEIGHT = 20;

  const LENGTH = 6;

  const TABLE = 'core_captcha';

  private static $client_id = 0;

  public static function check() {
    $value = self::get();
    $uc = S('req')->getAttribute('_captcha');
    if (strlen($value) < self::LENGTH || $value != $uc) {
      if (!empty($uc)) {
        S('log')->trace('db='.$value.' user='.$uc, __METHOD__);
      }
      throw new Exception();
    }
  }

  public static function image() {
    $font = WEB_PATH.'/'.CORE.'/fonts/Captcha.ttf';
    self::renew();
    $value = self::get();
    S('res')->close();
    header('Content-type: image/png');
    $rs = imagecreatetruecolor(self::WIDTH, self::HEIGHT);
    imagesavealpha($rs, true);
    $trans = imagecolorallocatealpha($rs, 0, 0, 0, 127);
    $dark = imagecolorallocate($rs, 30, 30, 30);
    imagefill($rs, 0, 0, $trans);
    imagettftext($rs, self::HEIGHT - (2 * 4), 0, 0, self::HEIGHT - 4, $dark, $font, $value);
    imagepng($rs);
    imagedestroy($rs);
  }

  public static function get() {
    $token = (int) S('req')->getAttribute('_captcha_token');
    $client_id = self::client();
    $value = S('dbo')->fetchValue('SELECT value'
                                  .' FROM '.self::TABLE
                                  .' WHERE client='.$client_id
                                  .  ' AND token='.$token);

    if ($value === false || empty($value)) {
      $value = self::code(self::LENGTH);
      S('dbo')->insert(self::TABLE,
                       array('client' => $client_id,
                             'token' => $token,
                             'value' => $value));
    }
    return $value;
  }

  private static function code($length=8) {
    //    $seed = 'acdefhikmnrstuwxz'.'24679';
    $seed = '123456789';
    $ret  = '';
    for ($i = 1; $i <= $length; $i++) {
      $ret .= $seed[mt_rand(0, strlen($seed) - 1)];
    }
    return $ret;
  }

  public static function renew() {
    $token = (int) S('req')->getAttribute('_captcha_token');
    S('dbo')->exec('DELETE FROM '.self::TABLE
                   .    ' WHERE client='.self::client()
                   .      ' AND token='.$token);
  }

  public static function client() {
    if (self::$client_id < 1) {
      self::$client_id = S('req')->getClient('ip');
    }
    return self::$client_id;
  }

  public static function clean() {
    S('dbo')->exec('DELETE FROM '.self::TABLE
                   .' WHERE (TO_DAYS(:now:) - TO_DAYS(created_at)) > 1',
                   array(':now:' => DH::now()));
  }

}