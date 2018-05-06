<?php

class Mailing extends ActiveRecord {

  const NEWLSETTER_ID = 1;

  const STATUS_CANCELED = -10;
  const STATUS_DRAFT = 0;
  const STATUS_SCHEDULED = 1;
  const STATUS_DELIVERED = 3;

  public static $statuses = array(self::STATUS_CANCELED => 'annulée',
                                  self::STATUS_DRAFT => 'en cours de rédaction',
                                  self::STATUS_SCHEDULED => 'prête pour l’envoi', 
                                  self::STATUS_DELIVERED => 'finalisée');

  protected function onBeforeCommit(&$attrs) { 
    if (isset($attrs['bases']) && empty($attrs['bases'])) {
        throw new Exception('au moins une base doit être sélectionnée');
    }
    if (is_array($attrs['bases'])) {
      $attrs['bases'] = ',' . join(',', $attrs['bases']);
    }
  }

  public function asFinderHash($h) {
    if ($this->status == self::STATUS_DRAFT) {
      $h['_color'] = 'yellow';
    }
    elseif ($this->status == self::STATUS_SCHEDULED) {
      $h['_color'] = 'blue';
    }
    return $h;
  }

  public function asCampaignHash() {
    $attrs = $this->getAttributes();
    $attrs['content_html'] = $this->getContentHtml();
    $attrs['content_text'] = $this->getContentText();
    $attrs['bases'] = SH::asSet($this->bases);
    $attrs['newsletter_id'] = self::NEWLSETTER_ID;
    return $attrs;
  }

  public function getContentHtml() { 
    $args = array('name' => $this->name,
                  'image' => $this->image,
                  'description' => $this->description,
                  'content' => $this->content);
    return Template::render($this->template, $args);
  }

  public function getContentText() {
    return $this->description;
  }

  public function getStats() {
    $args = array('model_id' => Model::id('mailing'),
                  'record_id' => $this->getId());
    if (!Campaign::exists($args)) return array();
    $campaign = Campaign::getInstance($args);
    if (!$campaign->isDelivered()) return array();
    return $campaign->getStats();
  }

}