<?php

class Crawler {

  public $url = null;
  public $feed_url = null;
  public $sitemap_urls = array();
  public $contents = array();

  const CONTENTS_LIMIT = 10;


  public function __construct($url) {
    $this->url = trim(H::url($url), '/');
    $this->feed_url = $this->getFeedUrl();
    $this->sitemap_urls = $this->getSitemapUrls();
    $this->contents = array();
  }

  public function getLastContents($limit=self::CONTENTS_LIMIT) {
    if ($this->feed_url) {
      $this->contents = $this->getFeedContents($limit);
    }
    elseif (count($this->sitemap_urls) > 0) {
      $this->contents = $this->getSitemapContents($limit);
    }
    return $this->contents;
  }

  // RSS FEED ------------------------------------------------------------------

  private function getFeeds() {
    if (empty($this->url)) return array();
    try {
      $xpath = self::getUrlDomXPath($this->url);
      $nodes = $xpath->query('//head/link[@rel="alternate" and (@type="application/atom+xml" or @type="application/rss+xml")]');
      if ($nodes->length < 1) return array();
      $feeds = array();
      foreach ($nodes as $node) {
        if (!$feeds[$node->getAttribute('type')]) {
          $feeds[$node->getAttribute('type')] = $node->getAttribute('href');
        }
      }
      return $feeds;
    }
    catch (Exception $e) { return array(); }
  }

  private function getFeedUrl() {
    $feeds = (array) $this->getFeeds();
    if (empty($feeds)) return null;
    if ($feeds['application/rss+xml']) return $feeds['application/rss+xml'];
    return current($feeds);
  }

  public function getFeedContents($limit=self::CONTENTS_LIMIT) {
    if (empty($this->feed_url)) return;
    try {
      $feed = FeedReader::getInstance($this->feed_url);
    }
    catch (Exception $e) { return false; }
    foreach ($feed as $i => $item) {
      if ($i >= $limit) break;
      $is_atom = isset($item['id']) && !isset($item['guid']);
      $id = $is_atom ? $item['id'] : $item['guid'];
      $content = $is_atom ? $item['content'] : $item['description'];
      $published_at = $is_atom ? $item['published'] : $item['pubDate'];

      $crc = SH::crc32($id.$item['link']);

      $published_at = date('Y-m-d H:i:s', strtotime($published_at));
      $content = SH::clean(strip_tags($content));
      $content = SH::tidy($content);
      if (empty($content)) continue;
      $record = array('crc' => $crc,
                      'name' => $item['title'],
                      'content' => $content,
                      'guid' => $id,
                      'published_at' => $published_at,
                      'url' => $item['link']);
      $this->contents[$id] = $record;
    }
    return $this->contents;
  }

  // SITEMAP -------------------------------------------------------------------

  private function getSitemapUrls() {
    $urls = array($this->url.'/sitemap.xml',
                  $this->url.'/sitemap_index.xml',
                  $this->url.'/sitemap/sitemap_index.xml',
                  $this->url.'/sitemap/sitemap.xml');
    foreach ($urls as $url) {
      $hr = HttpRequest::getInstance($url);
      $hs = HttpResponse::getInstance($hr->getStream());
      if ($hs->isError()) continue;
      $content = trim($hs->getContent());
      if (empty($content)) continue;
      $doc = new DOMDocument();
      $doc->preserveWhiteSpace = false;
      @$doc->loadXML($content);
      if ($doc->documentElement->tagName == 'sitemapindex') {
        return $this->getSitemapIndexUrls($url);
      }
      return array($url);
    }
    return $this->getSitemapUrlsFromRobotsFile();
  }

  private function getSitemapIndexUrls($index_url) {
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    $ext = strtolower(pathinfo(basename($index_url), PATHINFO_EXTENSION));
    $hr = HttpRequest::getInstance($index_url);
    $hs = HttpResponse::getInstance($hr->getStream());
    $content = $hs->getContent();
    if ($ext == 'gz') {
      $content = SH::gzdecode($content);
    }
    @$doc->loadHTML($content);
    $nodes = $doc->getElementsByTagName('loc');
    $urls = array();
    foreach ($nodes as $node) {
      $urls[] = $node->nodeValue;
    }
    return $urls;
  }

  private function getSitemapPageUrls($limit) {
    $urls = array();
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    $i = 0;
    foreach ((array) $this->sitemap_urls as $url) {
      $ext = strtolower(pathinfo(basename($url), PATHINFO_EXTENSION));
      if ($ext == 'gz') {
        $html = SH::gzdecode(file_get_contents($url));
        @$doc->loadHTML($html);
      }
      else {
        @$doc->loadHTML(self::getUrlHtml($url));
      }
      $nodes = $doc->getElementsByTagName('url');
      foreach ($nodes as $node) {
        $url = array();
        foreach ($node->childNodes as $child) {
          if ($child->nodeName == 'loc') {
            $url['url'] = $child->nodeValue;
          }
          if ($child->nodeName == 'lastmod') {
            $url['lastmod'] = date('Y-m-d H:i:s', strtotime($child->nodeValue));
          }
        }
        $urls[] = $url;
        if (++$i >= $limit) return $urls;
      }
    }
    return $urls;
  }

  public function getSitemapContents($limit=self::CONTENTS_LIMIT) {
    if (!empty($this->sitemap_urls)) {
      foreach ($this->getSitemapPageUrls($limit) as $i => $h) {
        if ($i >= $limit) break;
        $crc = SH::crc32($h['url']);
        $record = array('crc' => $crc,
                        'name' => self::fetchTitle($h['url']),
                        'content' => self::fetchContent($h['url']),
                        'published_at' => $h['lastmod'],
                        'url' => $h['url']);
        $this->contents[$crc] = $record;
      }
    }
    return $this->contents;
  }

  // ROBOTS.TXT ----------------------------------------------------------------

  private function getSitemapUrlsFromRobotsFile() {
    $url = $this->url.'/robots.txt';
    $content = @file_get_contents($url);
    preg_match_all('/Sitemap: ([^\s]+)/', $content, $matches);
    return $matches[1];
  }

  // STATIC --------------------------------------------------------------------

  public static function getInstance($url) {
    return new Crawler($url);
  }

  public static function fetchContent($url) {
    $xpath = self::getUrlDomXPath($url);
    $markups = array('style', 'object', 'embed');
    $ancestors = array('header', 'nav', 'footer', 'embed', 'object', 'form');
    $classes = array('posts', 'hierarchy', 'post-share-buttons', 'blog-pager',
                     'post-feeds', 'comment-form');
    $regexp = 'not(self::script)';
    foreach ($markups as $markup) {
      $regexp .= ' and not(self::'.$markup.')';
    }
    foreach ($ancestors as $markup) {
      $regexp .= ' and not(ancestor::'.$markup.')';
    }
    foreach ($classes as $class) {
      $regexp .= ' and not(ancestor::*[contains(concat(" ", normalize-space(@class), " "), " '.$class.' ")])';
    }
    $nodes = $xpath->query('//body//*['.$regexp.']/text()');
    if ($nodes->length < 1) return '';
    $contents = array();
    foreach ($nodes as $node) {
      $text = trim($node->textContent);
      $text = SH::tidy(SH::clean(strip_tags($text)));
      $text = preg_replace('/(\t|\n|\r)/u', '', $text);
      $text = preg_replace('/^(\s|\W)/u', '', $text);
      $text = preg_replace('/ +/', ' ', $text);
      if (empty($text)) continue;
      $contents[] = $text;
    }
    return implode(' ', $contents);
  }

  public static function fetchTitle($url) {
    $xpath = self::getUrlDomXPath($url);
    $node = $xpath->query('//title');
    return $node->item(0)->textContent;
  }

  // HELPERS -------------------------------------------------------------------

  public static function getUrlHtml($url) {
    try {
      $html = file_get_contents($url);
      if (stripos($html, '<?xml ') === false) {
        $html = '<?xml encoding="UTF-8">' . "\n\n" . $html;
      }
      if (stripos($html, 'http-equiv="content-type"') === false) {
        $html = str_ireplace('<head>',
                             '<head>'."\n".'<meta http-equiv="content-type" content="text/html; charset=utf-8">',
                             $html);
      }
      return $html;
    }
    catch (Exception $e) { return ''; }
  }

  public static function getUrlDomXPath($url) {
    $doc = new DOMDocument();
    $doc->formatOutput = true;
    $doc->preserveWhiteSpace = true;
    $html = self::getUrlHtml($url);
    @$doc->loadHTML($html);
    $doc->encoding = 'UTF-8';
    return new DomXPath($doc);
  }

}