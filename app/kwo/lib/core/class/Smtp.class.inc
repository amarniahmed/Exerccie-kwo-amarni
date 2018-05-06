<?php

class Smtp
{
  const CODE_SUCCESS = 200;

  const TABLE = 'core_smtp';

  public static function isHardbounce($dsn) {
    return in_array($dsn, self::$hard_errors);
  }

  public static function add($row) {
    S('dbo')->insert(self::TABLE, $row);
  }
  
  private static $soft_errors = array(400, 
                                      411, 412, 417, 418, 
                                      420, 421, 422, 
                                      430, 431, 432, 435, 
                                      440, 441, 442, 443, 445, 
                                      450, 451, 452, 
                                      470, 471, 
                                      500, 503, 512, 522, 531, 535, 551, 552, 553, 560);
  
  private static $hard_errors = array(511, 513, 516, 517, 518, 520, 521, 530, 540, 
                                      550, 554, 570, 571, 580);
  
  public static $errors = array(400 => 'Connection refused',
                                411 => 'Recipient address rejected',
                                412 => 'Recipient address rejected',
                                417 => 'Recipient address rejected',
                                418 => 'Domain of sender address not resolved',
                                420 => 'Recipient address rejected',
                                422 => 'Mailbox full',
                                421 => 'Mailbox temporarily disabled',
                                430 => 'Requested mail action not taken: mailbox unavailable',
                                431 => 'Too many recipients received this hour',
                                432 => 'Please try again later',
                                435 => 'Server configuration problem',
                                440 => 'Disconnected or network error',
                                441 => 'Disconnected or network error',
                                442 => 'Connection timed out',
                                443 => 'Mail queue for this domain is overloaded',
                                445 => 'Too many SMTP sessions for this host',
                                450 => 'Requested mail action not taken: mailbox unavailable',
                                451 => 'Requested action aborted: local error in processing',
                                452 => 'Requested action not taken: insufficient system storage',
                                470 => 'Authentication failed',
                                471 => 'Greylisting in action',
                                500 => 'Service unavailable',
                                503 => 'Bad sequence of commands',
                                511 => 'User unknown',
                                512 => 'User unknown',
                                513 => 'User unknown',
                                516 => 'User unknown',
                                517 => 'Bad sender\'s mailbox address syntax',
                                518 => 'Bad sender\'s system address',
                                520 => 'User unknown',
                                521 => 'User unknown',
                                522 => 'Service unavailable',
                                530 => 'User unknown',
                                531 => 'Mail system full',
                                535 => 'Local configuration error',
                                540 => 'Invalid domain name',
                                550 => 'Requested action not taken: mailbox unavailable',
                                551 => 'Service unavailable',
                                552 => 'Service unavailable',
                                553 => 'Too many recipients',
                                554 => 'Transaction failed',
                                560 => 'Data format error',
                                570 => 'User unknown',
                                571 => 'User unknown',
                                580 => 'User unknown');
  
}