<?php

try {

  //-- Advert

  $filters = array('|>|online_from' => '0000-00-00',
                   '|<=|online_from' => DH::now(),
                   '|>=|online_to' => DH::now(),
                   'status' => 0);
  $opts = array('filters' => $filters);
  foreach (Advert::collect($opts) as $advert) {
    $advert->commit(array('status' => 1));
  }
  
  $filters = array('|>|online_from' => '0000-00-00',
                   '|<|online_to' => DH::now(),
                   'status' => 1);
  $opts = array('filters' => $filters);
  foreach (Advert::collect($opts) as $advert) {
    $advert->commit(array('status' => 0));
  }

  //-- Story

  $filters = array('status' => Story::STATUS_SCHEDULED,
                   '|>=|scheduled_at' => DH::now());
  $opts = array('filters' => $filters);
  foreach (Story::collect($opts) as $story) {
    $story->setStatus(Story::STATUS_PUBLISHED);
  }

  //-- Slide

  $filters = array('|>|online_from' => '0000-00-00',
                   '|<=|online_from' => DH::now(),
                   '|>=|online_to' => DH::now(),
                   'status' => Slide::STATUS_SCHEDULED);
  $opts = array('filters' => $filters);
  foreach (Slide::collect($opts) as $slide) {
    $slide->setStatus(Slide::STATUS_ONLINE);
  }
  
  $filters = array('|>|online_from' => '0000-00-00',
                   '|<|online_to' => DH::now(),
                   'status' => Slide::STATUS_ONLINE);
  $opts = array('filters' => $filters);
  foreach (Slide::collect($opts) as $slide) {
    $slide->setStatus(Slide::STATUS_OFFLINE);
  }

  //-- Popup
  
  $filters = array('|>|online_from' => '0000-00-00',
                   '|<=|online_from' => DH::now(),
                   '|>=|online_to' => DH::now(),
                   'status' => Popup::STATUS_SCHEDULED);
  $opts = array('filters' => $filters);
  foreach (Popup::collect($opts) as $popup) {
    $popup->setStatus(Popup::STATUS_ONLINE);
  }
  
  $filters = array('|>|online_from' => '0000-00-00',
                   '|<|online_to' => DH::now(),
                   'status' => Popup::STATUS_ONLINE);
  $opts = array('filters' => $filters);
  foreach (Popup::collect($opts) as $popup) {
    $popup->setStatus(Popup::STATUS_OFFLINE);
  }

}
catch (Exception $e) { $err->add($e); }
