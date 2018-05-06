<?php

/***
 Fonctionne avec :
 - http://es.noticias.yahoo.com/rss/internet.xml

copyright, description, image, items, language,
lastBuildDate, link, title, ttl

 ***/

class FeedReader implements Iterator {

  public $items = array();
  private $timeout = 10;

  public function __construct($source=null, $opts=null) {
    if (strlen($source) < 5) {
      throw new Exception('invalid source  ['.$source.']');
    }

    // Ajout loick pour scheme uri non standard
    if (strpos($source, 'feed://') === 0) {
      $source = str_replace('feed://', 'http://', $source);
    }
    else if (strpos($source, 'feed:') === 0) {
      $source = str_replace('feed:', '', $source);
    }
    // fin ajout

    $opts = (array) $opts;
    foreach ($opts as $key => $value) {
      $this->{$key} = $value;
    }
    $xml = $this->fetchContent($source);
    if ($xml === false) {
      throw new Exception('invalid source ['.$source.']');
    }
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    if ($doc->loadXML($xml) === false) {
      throw new Exception('invalid xml in ['.$source.']');
    }
    $this->load($doc);
  }

  private function fetchContent($source) {
    $opts = array('timeout' => $this->timeout);
    return HttpClient::get($source, null, $opts);
  }

  private function load($doc) {
    $channel = $doc->documentElement->tagName === 'feed'
             ? $doc->documentElement
             : $doc->documentElement->firstChild;

    foreach ($channel->childNodes as $node) {
      if ($node->tagName === 'item' || $node->tagName === 'entry') {
        $this->items[] = $this->node2hash($node);
      }
      else {
        $children = $node->childNodes;
        if ($children->length >= 2) {
          $this->{$node->tagName} = $this->node2hash($node);
        }
        else {
          if (!$node->tagName) continue;
          $this->{$node->tagName} = $node->nodeValue;
        }
      }
    }
  }

  private function node2hash($node) {
    $h = array();
    foreach ($node->childNodes as $child) {
      //      $h[$child->tagName] = utf8_decode($child->nodeValue);
      $h[$child->tagName] = $child->nodeValue;
    }
    if (strlen($h['pubDate']) >= 8) {
      $time = date_parse($h['pubDate']);
      $time = mktime($time['hour'], $time['minute'], $time['second'],
                     $time['month'], $time['day'], $time['year']);
      if ($time > time()) {
        $time = time();
      }
      $h['pubDate'] = date(DH::DATE_MYSQL, $time);
    }
    return $h;
  }

  public function rewind()  { reset($this->items); }
  public function current() { return current($this->items); }
  public function key()     { return key($this->items); }
  public function next()    { return next($this->items); }
  public function valid()   { return $this->current() !== false && $this->current() !== null; }

  public function debug() {
    S('log')->debug($this);
  }

  public static function getInstance($source) {
    return new FeedReader($source);
  }

}
