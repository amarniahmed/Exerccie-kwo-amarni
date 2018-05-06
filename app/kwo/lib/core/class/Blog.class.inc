<?php

class Blog extends ActiveRecord
{
  const FEED_URL = '/blog.feed';

  public function getArchives() {
    $hash = $this->dbo->asHash('SELECT DATE_FORMAT(published_at, "%Y-%m") AS `monthkey`,'
                               .     ' COUNT(*) AS `count`,'
                               .     ' DATE_FORMAT(published_at, "y/%Y/m/%c") AS `link`'
                               .' FROM '.T('post')
                               .' WHERE blog_id='.$this->getId()
                               .  ' AND status=1'
                               .' GROUP BY `monthkey`'
                               .' ORDER BY `monthkey` DESC',
                               null, 'monthkey');
    return $hash;
  }

  public function getPostHeadings() {
    $hash = $this->dbo->asHash('SELECT h.id, h.name,'
                               .     ' COUNT(p.id) as `count`'
                               .' FROM `'.T('heading').'` AS h,'
                               .      '`'.Heading::TABLE_POST.'` AS hp,'
                               .      '`'.T('post').'` AS p'
                               .' WHERE h.blog_id='.$this->getId()
                               .'   AND h.id = hp.heading_id'
                               .'   AND hp.post_id = p.id'
                               .'   AND p.status > 0'
                               .' GROUP BY h.id'
                               .' HAVING `count` > 0'
                               .' ORDER BY h.name ASC',
                               null, 'id');
    return $hash;
  }

  public function ping() {
    $url = H::url(self::FEED_URL, array('id' => $this->getId()));
    if (!$this->isOnline()) return ;
    $bindings = array('name' => $this->getName(),
                      'url' => $this->asUrl(),
                      'feed' => $url);
    foreach ($bindings as &$value) {
      $value = urlencode($value);
    }
    $providers = array('google' => 'blogsearch.google.com/ping?name=${name}&url=${url}&changesURL=${feed}');
    foreach ($providers as $name => $url) {
      $url = SH::bind('http://'.$url, $bindings);
      $fd = fopen($url,'r');
      $out = fread($fd, 1024);
      fclose($fd);
    }
  }

  public function __toString() {
    S('log')->trace('DEPRECATED ('.$this->getModel().') (avertir FX)', __METHOD__);
    S('log')->backtrace();
    return null;
  }

}
