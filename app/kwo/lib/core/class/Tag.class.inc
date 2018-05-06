<?php

class Tag extends ActiveRecord
{
  const FLAG_IMPORTANT = 1;
  const FLAG_CLOUD_EXCLUDE = 2;

  public static $flags = array(self::FLAG_IMPORTANT => 'important',
                               self::FLAG_CLOUD_EXCLUDE => 'exclu du cloud');

  public static function set($item, $tags=null, $locale=null) { 
    if (!is_array($tags)) {
      return false;
    }
    $dbo = S('dbo');
    $locale = $item->_locale($locale); 
    $tags = array_unique($tags); 
    $tags = array_slice($tags, 0, 16);
    $record = array('model_id' => $item->getModelId(),
                    'record_id' => $item->getId(),
                    'locale' => $locale,
                    'created_on' => DH::now());
    $dbo->exec('DELETE FROM '.Item::TABLE_TAG
               .' WHERE model_id='.$item->getModelId()
               .  ' AND record_id='.$item->getId()
               .  ' AND locale='.$locale); 
    $records = array(); 
    foreach ($tags as $tag) {
      $tag = str_replace(array(','), array(' '), $tag);
      $tag = trim(strtolower($tag));
      $tag = trim($tag, './*');
      $tag = SH::clean($tag);
      if (strlen($tag) < 2) continue ;
      $tag = substr($tag, 0, 32);
      $records[$tag] = array('name' => $tag,
                             'locale' => $locale);
    }
    if (count($records) < 1) return false;
    $dbo->massInsert(T('tag'), $records, array('modifiers' => 'ignore'));
    $ids = $dbo->asHash('SELECT name, id'
                        .' FROM '.T('tag')
                        .' WHERE name IN ([tags])'
                        .  ' AND locale='.$locale,
                        array('[tags]' => array_keys($records))); 
    $inserts = array();
    foreach ($records as $tag => $hash) {
      if (($id = $ids[$tag]) < 1) continue ;
      $inserts[] = $record + array('tag_id' => $id);
    } 
    $opts = array('modifiers' => array('ignore'));
    $dbo->massInsert(Item::TABLE_TAG, $inserts, $opts); 
  }

  public static function asId($name, $locale=null) {
    $locale = $locale ?: S('res')->getLocale();
    return S('dbo')->fetchValue('SELECT id'
                                .' FROM '.T('tag')
                                .' WHERE name=LOWER(:name:)'
                                .  ' AND locale='.intval($locale),
                                array(':name:' => substr($name, 0, 32)));
  }

  public static function getByItem($item) {
     if (!$item->isItem()) return array();
    return $item->dbo->asSet('SELECT t.name'
                             .' FROM '.Item::TABLE_TAG.' AS i'
                             .' JOIN '.T('tag').' as t'
                             .  ' ON (t.id=i.tag_id)'
                             .' WHERE i.model_id='.$item->getModelId()
                             .  ' AND i.record_id='.$item->getId());
  }

}
