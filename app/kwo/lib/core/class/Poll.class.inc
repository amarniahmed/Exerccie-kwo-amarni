<?php

class Poll extends ActiveRecord 
{
  public function getDetails() {
    $h = array();
    $results = $this->getResults();
    $h['nbre de réponses'] = $results['total'];
    foreach ($results['choices'] as $choice => $count) {
      $h[$choice] = $count;
    }
    return $h;
  }

  public function getResults() {
    $h = array();
    $counts = $this->dbo->asHash('SELECT value AS choice_id, COUNT(*) AS n'
                                 .' FROM '.Interaction::TABLE
                                 .' WHERE model_id='.$this->getModelId()
                                 .  ' AND record_id='.$this->getId()
                                 .  ' AND type='.Interaction::TYPE_VOTE
                                 .' GROUP BY value');
    $opts = array('order' => array('position'));
    $h['choices'] = $this->getChoices($opts)->asHash('name', 'id');
    foreach ($h['choices'] as $key => &$value) {
      $value = (int) $counts[$value];
    }
    $h['total'] = array_sum($counts);
    return $h;
  }
  
  public function asShortCodeContent($args=null) {
    return W::poll($this->code);
  }
  
  public function asHtml($locale) {
    if (!$this->isOnline()) return '';
    $locale = $locale ?: S('res')->getLocale();
    $this->getProperties($locale, true);
    $template = $this->template ?: 'poll';
    $filters = array('|>|status' => 0,
                     '|&|locales' => $locale);
    $opts = array('filters' => $filters,
                  'order' => array('position' => 'asc'));
    $choices = Choice::collect($opts);
    if ($choices->count() < 1) return '';
    $args = array('poll' => $this,
                  'choices' => $choices);
    return Template::render($template, $args);
  }

}

