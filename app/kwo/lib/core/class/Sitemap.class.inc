<?php

/*
** http://www.sitemaps.org/protocol.php
** http://www.google.com/support/webmasters/bin/topic.py?topic=8476
** http://en.wikipedia.org/wiki/Sitemaps

News Sitemaps:
http://www.google.com/support/news_pub/bin/answer.py?hl=en&answer=74288
http://www.google.com/support/news_pub/bin/answer.py?answer=93992

TODO : Sitemap Index
http://www.google.com/support/webmasters/bin/answer.py?hl=fr&answer=71453

TODO : Media
http://www.google.com/support/webmasters/bin/answer.py?answer=34657

**
*/

class Sitemap
{
  const ACCESS_SUBSCRIPTION = 'Subscription';
  const ACCESS_REGISTRATION = 'Registration';

  const FREQ_ALWAYS = 'always';
  const FREQ_HOURLY = 'hourly';
  const FREQ_DAILY = 'daily';
  const FREQ_WEEKLY = 'weekly';
  const FREQ_MONTHLY = 'monthly';
  const FREQ_YEARLY = 'yearly';
  const FREQ_NEVER = 'never';

  const GENRE_PRESSRELEASE = 'PressRelease';
  const GENRE_SATIRE = 'Satire';
  const GENRE_BLOG = 'Blog';
  const GENRE_OPED = 'OpEd';
  const GENRE_OPINION = 'Opinion';
  const GENRE_USERGENERATED = 'UserGenerated';

  private $type = '';
  private $changefreq = '';
  private $priority = '';
  private $locale = '';

  public function __construct($set, $opts) {
    foreach ($opts as $key => $value) {
      $this->{$key} = $value;
    }
    $this->changefreq = empty($this->changefreq) ? 'monthly' : $this->changefreq;
    $this->priority = empty($this->priority) ? 0.5 : $this->priority;
    $this->load($set);
  }

  public function load($set) {
    $is_news = $this->type === 'news';
    $namespace = $is_news ? ' xmlns:n="http://www.google.com/schemas/sitemap-news/0.9"' : '';
    $xml = '<?xml version="1.0" encoding="UTF-8"?>'."\n";
    $xml .= '<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"'.$namespace.'>'."\n";
    $set = $set instanceof Collection ? array($set) : $set;
    foreach ($set as $items) {
      foreach ($items as $item) {
        $xml .= '<url>';
        $url = $item instanceof ActiveRecord ? $item->asUrl() : $item->url;
        $url = strpos($url, '://') > 1 ? $url : H::url($url);
        $xml .= '<loc>'.$url.'</loc>';
        $xml .= '<lastmod>'.date(DATE_ATOM, strtotime($item->getMeta(Meta::FIELD_UPDATED_AT))).'</lastmod>';
        $xml .= '<changefreq>'.$this->changefreq.'</changefreq>';
        $xml .= '<priority>'.$this->priority.'</priority>';
        if ($is_news === true) {
          $locale = empty($this->locale) ? $item->getLocale() : $this->locale;
          $news = $item->asSitemapNewsHash($locale);
          $xml .= '<n:news>'."\n";
          $xml .= '<n:publication>';
          $xml .= ' <n:name>'.$this->protect($news['publication_name']).'</n:name>';
          $language = is_numeric($news['publication_language'])
                    ? Locale::toCode($news['publication_language'])//I18N::locale2code($news['publication_language'])
                    : $news['publication_language'];
          $xml .= ' <n:language>'.$language.'</n:language>';
          $xml .= '</n:publication>';
          if (!empty($news['access'])) {
            $xml .= ' <n:access>'.$news['access'].'</n:access>';
          }
          if (!empty($news['genres'])) {
            $xml .= ' <n:genres>'.$news['genres'].'</n:genres>';
          }
          $xml .= '<n:publication_date>'.substr($news['publication_date'], 0, 10).'</n:publication_date>';
          $xml .= '<n:title>'.$this->protect($news['title']).'</n:title>';
          if (!empty($news['keywords'])) {
            $news['keywords'] = is_array($news['keywords']) ? join(', ', $news['keywords']) : $news['keywords'];
            $xml .= '<n:keywords>'.$this->protect($news['keywords']).'</n:keywords>';
          }
          if (!empty($news['stock_tickers'])) {
            $xml .= ' <n:stock_tickers>'.$this->protect($news['stock_tickers']).'</n:stock_tickers>';
          }
          $xml .= '</n:news>'."\n";
        }
        $xml .= '</url>'."\n";
      }
    }
    $xml .= '</urlset>'."\n";
    $this->xml = $xml;
  }

  public function asXml() {
    return $this->xml;
  }

  protected function protect($str) {
    return htmlspecialchars($str, ENT_COMPAT, 'UTF-8');
  }

  public static function getInstance($set, $opts=null) {
    $opts = (array) $opts;
    return new Sitemap($set, $opts);
  }

  public static function index($sitemaps) {
    $xml = '<?xml version="1.0" encoding="UTF-8"?>'."\n";
    $xml .= '<sitemapindex xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">'."\n";
    foreach ($sitemaps as $sitemap) {
       $xml .= '<sitemap>';
       $url = $sitemap['url'];
       $url = strpos($url, '://') > 1 ? $url : H::url($url);
       $xml .= '<loc>'.$url.'</loc>';
       $xml .= '</sitemap>';
    }
    $xml .= '</sitemapindex>'."\n";
    return $xml;
  }


}
