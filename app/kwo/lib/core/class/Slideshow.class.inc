<?php

class Slideshow extends ActiveRecord {

  public function getSlides() {
    $filters = array('slideshow_id' => $this->getId(),
                     'status' => Slide::STATUS_ONLINE);
    $opts = array('filters' => $filters,
                  'order' => array('position' => 'asc'));
    return Slide::collect($opts);
  }

  public function asHtml() {
    if (!$this->isOnline()) return '';
    $slides = $this->getSlides();
    if (count($slides) < 1) return '';
    $args = array('slides' => $slides);
    $template = $this->template ?: 'slideshow.default';
    $out = Template::render($template, $args);
    return $out;
  }

}