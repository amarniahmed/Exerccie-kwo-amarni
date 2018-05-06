<?php

class Post extends ActiveRecord
{
  const STATUS_CANCELED = -100;
  const STATUS_DRAFT = -50;
  const STATUS_PENDING = -20;
  const STATUS_SCHEDULED = -10;
  const STATUS_PUBLISHED = 10;

  public static $statuses = array(self::STATUS_PUBLISHED => 'publié',
                                  self::STATUS_SCHEDULED => 'planifié',
                                  self::STATUS_PENDING => 'relecture',
                                  self::STATUS_DRAFT => 'brouillon',
                                  self::STATUS_CANCELED => 'annulé');
  const FLAG_PINNED = 1;
  const FLAG_COMMENT = 2;
  const FLAG_SPONSORED = 4;

  public static $flags = array(self::FLAG_PINNED => 'en avant',
                               self::FLAG_COMMENT => 'commentaires',
                               self::FLAG_SPONSORED => 'sponsorisé');

  public static $colors = array(self::STATUS_PUBLISHED => 'green',
                                self::STATUS_SCHEDULED => 'blue',
                                self::STATUS_PENDING => 'yellow',
                                self::STATUS_DRAFT => 'orange',
                                self::STATUS_CANCELED => 'red');

  public function markAsEvolved() {
    parent::markAsEvolved();
    $this->getBlog()->markAsEvolved();
  }

  protected function onBeforeCommit(&$attrs) {
    if (empty($attrs['name'])) {
      throw new Exception('le nom est obligatoire');
    }
    if (isset($attrs['code']) && empty($attrs['code'])) {
      $attrs['code'] = SH::urlify($attrs['name']);
    }
  }

  public function getLocale() {
    return $this->getBlog()->getLocale();
  }

  public function getLocales() {
    return array($this->getBlog()->getLocale());
  }

  public function onPick($args) {
    $items = $args['items'];
    $filters = $items->getFilters();
    if ($items->getModel() === 'heading') {
      $filters['blog_id'] = $this->blog_id;
    }
    $items->setFilters($filters);
  }

  public function asEditHash() {
    $h = array();
    $blog_id = $this->isItem() ? $this->blog_id : S('ctx')->blog_id;
    if ($blog_id > 0) {
      $h['blog'] = Blog::getInstance($blog_id);
    }
    return $h;
  }

  public function asHeadHash($locale) {
    $h = parent::asHeadHash($locale);
    if (!empty($h['title'])) {
      $h['title'] = $this->name.' | '.$this->getBlog()->name;
    }
    $h['url'] = $this->asUrl();
    return $h;
  }

  public function asSitemapNewsHash($locale=null) {
    /* manque : access et stock_tickers */
    $blog = $this->getBlog();
    return array('publication_name' => (string) $blog,
                 'publication_language' => $blog->getLocale(),
                 'genres' => Sitemap::GENRE_BLOG,
                 'publication_date' => $this->published_at,
                 'title' => $this->getName(),
                 'keywords' => $this->getTags());
  }

}