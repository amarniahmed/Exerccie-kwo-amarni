<?php

/****

$feed = LeGuideFeed::getInstance();
$feed->inject(Marchandise::collect());
$res->write($feed);

****/

class ProductFeed
{
  const TYPE_LEGUIDE = 1;
  const TYPE_GOOGLE_SHOPPING = 2;
  const TYPE_LINKSHARE = 3;
  const TYPE_LENGOW = 4;

  public static $types = array(self::TYPE_LEGUIDE => 'leguide',
                               self::TYPE_GOOGLE_SHOPPING => 'googleshopping',
                               self::TYPE_LINKSHARE => 'linkshare',
                               self::TYPE_LENGOW => 'lengow');
  
  protected $doc = null;

  protected $type = 0;

  protected $collections = array();

  public function getType() {
    return $this->type;
  }

  public function addItems($items) {
    $this->collections[] = $items;
  }
  
  public function asText() {
    foreach ($this->collections as $collection) {
      foreach ($collection as $item) {
        $this->addItem($item);
      }
    }
    return $this->doc->saveXML();
  }

}