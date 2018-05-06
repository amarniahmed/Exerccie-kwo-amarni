<?php

/*
${source}The source of the item
${title}The title of the item
${url}The URL of the item
${short-url}A shortened URL that redirects to the item

http://www.viadeo.com/shareit/share/?url=


url title srcurl srctitle snippet tags


*/

class Notification extends ActiveRecord {

  const TABLE_EMAIL = 'core_email_notification';

  const TYPE_MAIL = 1;
  const TYPE_SITE = 2;

  public static $services = array(1 => array('name' => 'facebook',
                                             'icon' => 'facebook.gif',
                                             'url' => 'http://www.facebook.com/share.php?u=${url}&t=${title}'),
                                  2 => array('name' => 'twitter',
                                             'icon' => 'twitter.gif',
                                             'url' => 'http://twitter.com/home?status=reading+${title}+${url}'),
                                  3 => array('name' => 'Google Buzz',
                                              'icon' => 'googlebuzz.gif',
                                              'url' => 'http://www.google.com/reader/link?url=${url}&title=${title}&srcUrl=${srcurl}&srcTitle=${srctitle}&snippet=${snippet}'),
                                  4 => array('name' => 'Yahoo! Buzz',
                                              'icon' => 'yahoobuzz.gif',
                                              'url' => 'http://buzz.yahoo.com/buzz?publisherurn={$srcurl}&targetUrl=${url}&headline=${title}&summary=${snippet}'),
                                  5 => array('name' => 'Tumblr',
                                              'icon' => 'tumblr.png',
                                              'url' => 'http://www.tumblr.com/share?t=${title}&u=${url}&v=3'),
                                  6 => array('name' => 'Posterous',
                                              'icon' => 'posterous.png',
                                              'url' => 'http://posterous.com/share?linkto=${url}&title=${title}'),
                                  7 => array('name' => 'myspace',
                                             'icon' => 'myspace.gif',
                                             'url' => 'http://www.myspace.com/index.cfm?fuseaction=postto&u=${url}&t=${title}'),
                                  8 => array('name' => 'wikio',
                                             'icon' => 'wikio.gif',
                                             'url' => 'http://www.wikio.fr/vote?url=${url}'),
                                  9 => array('name' => 'digg',
                                             'icon' => 'digg.gif',
                                             'url' => 'http://digg.com/submit?phase=2&url=${url}'),
                                  10 => array('name' => 'delicious',
                                             'icon' => 'delicious.gif',
                                             'url' => 'http://del.icio.us/post?v=5&noui&jump=close&url=${url}&title=${title}&notes=${snippet}'),
                                  11 => array('name' => 'google Bookmark',
                                             'icon' => 'google.gif',
                                             'url' => 'http://www.google.com/bookmarks/mark?op=add&bkmk=${url}&title=${title}'),
                                  12 => array('name' => 'technorati',
                                             'icon' => 'technorati.gif',
                                             'url' => 'http://technorati.com/faves?add=${url}'),
                                  13 => array('name' => 'stumbleupon',
                                             'icon' => 'stumbleupon.gif',
                                              'url' => 'http://www.stumbleupon.com/submit?url=${url}'),
                                  14 => array('name' => 'reddit',
                                             'icon' => 'reddit.gif',
                                             'url' => 'http://reddit.com/submit?url=${url}')
                                  );
  /*
  reddit : http://reddit.com/submit?url=
  */
  protected function onAfterCreate() {
    S('app')->hook('onAfterNotificationCreate',
                   array('notification' => $this));
  }

  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM '.self::TABLE_EMAIL
                     .    ' WHERE notification_id='.$this->getId());
  }

}

