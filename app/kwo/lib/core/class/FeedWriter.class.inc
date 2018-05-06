<?php

/* 

http://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references 
http://en.wikipedia.org/wiki/RSSl(file_format)

<media:content url="http://graphics8.nytimes.com/images/2009/08/06/us/06golf.75.jpg" medium="image" height="75" width="75"/>
<media:description>When a few drops here and there are enough</media:description>
<media:credit>Kendrick Brinson for The New York Times</media:credit>

http://vimeo.com/channels/hd/videos/rss 

*/

class FeedWriter
{
  public $channel = null;
  public $entries = null;

  public function __construct($channel, $items) { 
    $channel = $channel instanceof ActiveRecord
             ? $channel->asChannel()
             : $channel;
    if (empty($channel['title'])) {
      S('log')->error('required info needed', __METHOD__);
      throw new Exception('required info needed');
    }
    $channel['title'] = $this->protect($channel['title']);
    $channel['description'] = $this->protect($channel['description']);
    $channel['copyright'] = $this->protect($channel['copyright']);
    if (empty($channel['ttl'])) {
      $channel['ttl'] = 240;
    }
    if (empty($channel['language'])) {
      $channel['language'] = 'fr';
    }
    if (empty($channel['link'])) {
      $channel['link'] = H::url('/');
    }
    elseif (strpos($channel['link'], '://') === false) {
      $channel['link'] = H::url($channel['link']);
    }
    if (empty($channel['generator'])) {
      $channel['generator'] = 'KWO';
    }
    if (!empty($channel['image'])) {
      $channel['image'] = $channel['image'][0] === '/'
                        ? H::url($channel['image'])
                        : $channel['image'];
    }
    $channel['updated_at'] = strlen($channel['updated_at']) > 6 
                           ? strtotime($channel['updated_at'])
                           : time();
    $this->channel = $channel;
    $this->loadEntries($items); 
  }

  private function loadEntries($items) {
    $this->entries = (array) $this->entries;
    foreach ($items as $item) {
      $entry = $item instanceof ActiveRecord ? $item->asFeedHash() : $item; 
      $entry['title'] = $this->protect($entry['title']);
      $entry['description'] = $this->protect($entry['description']);
      $entry['category'] = $this->protect($entry['category']);
      if (!array_key_exists('link', $entry)) {
        $entry['link'] = $this->channel['link'];
      }
      else if (strpos($entry['link'], '://') === false) {
        $link  = '/'.trim($entry['link'], '/');
        $link .= '&knxs='.urlencode($this->channel['title']);
        $entry['link'] = H::url($link);
      }
      $entry['link'] = htmlspecialchars($entry['link'], ENT_COMPAT, 'UTF-8');
      if (!array_key_exists('comments', $entry)) {
        $entry['comments'] = $entry['link'].'#comments';
      }
      $entry['updated_at'] = strlen($entry['updated_at']) > 6 
                           ? strtotime($entry['updated_at'])
                           : time();
      $this->entries[] = $entry;
    }
  }

  private function asString($type=Feed::TYPE_RSS) {
    if ($type === Feed::TYPE_RSS) {
      return $this->asRss();
    }
  }

  private function asRss() {
    $chnl = $this->channel;
    $doc = new DOMDocument('1.0', 'UTF-8');
    $doc->xmlStandalone = true;
    $doc->formatOutput = true;
    $rss = $doc->createElement('rss');
    $rss->setAttribute('version', '2.0');
    $rss = $doc->appendChild($rss);
    $channel = $doc->createElement('channel');
    $nodes = array();
    $attrs = array('title', 'description', 'language', 'link', 'generator', 
                   'ttl', 'copyright');
    foreach ($attrs as $attr) {
      if (strlen($chnl[$attr]) < 1) continue ;
      $nodes[] = $doc->createElement($attr, $chnl[$attr]);
    }
    $nodes[] = $doc->createElement('lastBuildDate', date(DATE_RSS));
    foreach ($nodes as $node) {
      $channel->appendChild($node);
    }
    if (!empty($chnl['image'])) {
      $image = $doc->createElement('image');
      $nodes = array();
      $nodes[] = $doc->createElement('url', $chnl['image']);
      $nodes[] = $doc->createElement('title', $chnl['title']);
      $nodes[] = $doc->createElement('link', $chnl['link']);
      foreach ($nodes as $node) {
        $image->appendChild($node);
      }
      $channel->appendChild($image);
    }
    $rss->appendChild($channel);
    $attrs = array('title', 'description', 'category', 'link', 'comments',
                   'author', 'image');
    foreach ($this->entries as $entry) {
      $item = $doc->createElement('item');
      $nodes = array();
      foreach ($attrs as $attr) {
        if (strlen($entry[$attr]) < 1) continue ;
        $nodes[] = $doc->createElement($attr, $entry[$attr]);
      }
      $nodes[] = $doc->createElement('pubDate', 
                                     date(DATE_RSS, $entry['updated_at']));
      $guid = $doc->createElement('guid', $entry['guid']);
      $guid->setAttribute('isPermaLink', 'false');
      $nodes[] = $guid;
      foreach ($nodes as $node) {
        $item->appendChild($node);
      }
      $rss->appendChild($item);
    }
    return $doc->saveXML();
  }

  protected function protect($str) {
    return htmlspecialchars($str, ENT_COMPAT, 'UTF-8');
  }

  public static function getInstance($channel, $items) {
    return new FeedWriter($channel, $items);
  }

}
