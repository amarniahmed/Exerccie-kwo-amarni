<?php

try { 

  $item = Item::getInstance(MODEL, $req->id);

  $locales = $dbo->asSet('SELECT locale'
                         .' FROM '.Item::TABLE_SEO
                         .' WHERE model_id='.$item->getModelId()
                         .  ' AND record_id='.$item->getId(),
                         null, 'locale');

  $filters = array('model_id' => $item->getModelId(),
                   'record_id' => $item->getId());
  
  foreach ($req->records as $locale => $hash) {
    $locale = (int) $locale;
    $tags = (array) $hash['tags'];
    if (strpos($tags[0], ',') > 0) {
      $tmp = explode(',', $tags[0]);
      unset($tags[0]);
      $tags = array_merge($tags, $tmp);
    }
    unset($hash['tags']);
    $item->setTags($tags, $locale);
    $tmp = join('', $tags);
    if (!empty($tmp)) {
      $tmp = array();
      foreach ($tags as $tag) {
        if (empty($tag)) continue ;
        $tmp[] = $tag;
      }
      $hash['keywords'] = join(', ', $tmp);
    }
    $filters['locale'] = $locale;
    if (in_array($locale, $locales)) {
      $dbo->update(Item::TABLE_SEO, $hash,
                   array('filters' => $filters));  
    }
    else { 
      $dbo->insert(Item::TABLE_SEO,
                   $filters + $hash); 
    }
  }
}
catch (Exception $e) { $err->add($e); }
