<?php

class Area extends ActiveRecord {

  const TYPE_ZONE = 1;
  const TYPE_COUNTRY = 2;
  const TYPE_ADMINS = 3;

  public static $types = array(self::TYPE_ZONE => 'zone',
                               self::TYPE_COUNTRY => 'pays',
                               self::TYPE_ADMINS => 'régions');

  public function asSuggestionHash($locale) {
    $hash = parent::asSuggestionHash($locale);
    $label = $this->getProperty('label', $locale);
    $precision = $this->getProperty('precision', $locale);
    $content  = $this->getProperty('content', $locale);
    $content .= (strlen($content) ? ',' : '').(strlen($precision) ? $precision.',' : '').$label.',';
    $content .= implode(',', $this->getTags());
    $hash['content'] = $content;
    $hash['label'] = '<strong>'.$label.'</strong>'.(strlen($precision) ? ', '.$precision : '');
    $hash['attributes'] = serialize(array('latitude' => $this->latitude,
                                          'longitude' => $this->longitude,
                                          'radius' => $this->radius,
                                          'country_id' => $this->country_id));
    return $hash;
  }

  public function onAfterStore() {
    if ($this->isOnline()) {
      Suggestion::append($this);
    }
    else {
      Suggestion::remove($this);
    }
  }

}