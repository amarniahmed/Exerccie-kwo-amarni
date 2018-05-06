<?php

class Advert extends ActiveRecord
{
  const TABLE_SPACE = 'core_advert_space'; 

  const FLAG_BLANK = 1;
  const FLAG_TRACE = 2;
  const FLAG_AUTOPROMO = 4;

  public static $flags = array(self::FLAG_BLANK => 'blank',
                               self::FLAG_TRACE => 'display',
                               self::FLAG_AUTOPROMO => 'auto promo');

  const FORMAT_HBANNER = 1;
  const FORMAT_VBANNER = 2;
  const FORMAT_SQUARE = 3;

  public static $formats = array(self::FORMAT_HBANNER => 'horizontal',
                                 self::FORMAT_VBANNER => 'vertical',
                                 self::FORMAT_SQUARE => 'carre');

  const TYPE_IMAGE = 1;
  const TYPE_FLASH = 2;
  const TYPE_SCRIPT = 3;
  const TYPE_TEXT = 4;

  public static $types = array(self::TYPE_IMAGE => 'image',
                               self::TYPE_FLASH => 'flash',
                               self::TYPE_SCRIPT => 'script',
                               self::TYPE_TEXT => 'texte');

  const MEDIUM_WEB = 1;
  const MEDIUM_MOBILE = 2;

  public static $media = array(self::MEDIUM_WEB => 'web',
                               self::MEDIUM_MOBILE => 'mobile');

  public function asFinderHash($h) {
    $h['format'] = self::$formats[$h['format']];
    $h['type'] = self::$types[$h['type']];
    return $h;
  }

  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['source']) && empty($attrs['source'])) {
      throw new ActiveRecordException('source obligatoire');
    }
  }

  public static function display($format=null, $opts=null) {
    $dbo = S('dbo');
    $format = $format ? intval($format) : self::FORMAT_HBANNER;
    $space = $opts['space'] ? $opts['space'] : S('res')->space;
    if ($format >= 1) {
      $where .= ' AND format='.$format;
    }
    if (!empty($space)) {
      if (!is_numeric($space)) {
        //fix aleksandr
/*
        $space = $dbo->fetchValue('SELECT id'
                                  .' FROM '.T('space')
                                  .' WHERE name=:name:',
                                  array(':name:' => $space));
*/
        $space = $dbo->query('SELECT advert_id'
                                  .' FROM '.t('space').' s'
                                  .' JOIN '.self::TABLE_SPACE.' AS asp ON s.id = asp.space_id'
                                  .' WHERE s.name=:name:',
                                  array(':name:' => $space))->asCollection('advert');
        $space = $space->asHash('advert_id', 'advert_id');
        $space[] = -1;
      }
      //$where .= ' AND (spaces & '.NH::asBit($space).') >= 1';
      $where .= ' AND id IN ('.implode(',', (array)$space).')';
    }
    $obj = $dbo->fetchObject('SELECT *'
                             .' FROM '.T('advert')
                             .' WHERE status=1'
                             .  ' AND locale='.S('res')->getLocale()
                             .  $where
                             .' ORDER BY RAND()'
                             .' LIMIT 1');
    if (empty($obj)) {
      return '<div class="kwo-ad-error">your ad here</div>';
    }
    try {
      $advert = Advert::getInstance(true, array('nucleus' => $obj));
      if ($advert->hasFLag(self::FLAG_TRACE)) {
        $advert->log();
      }
      return Template::render('advert', array('advert' => $advert));
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      return '';
    }
  }
}



  /*  public function commit($attrs=null) {
    $spaces = null;
    if (isset($attrs['spaces'])) {
      $spaces = (array) $attrs['spaces'];
      unset($attrs['spaces']);
    }
    parent::commit($attrs);
    if (is_array($spaces)) {
      $this->setSpaces($spaces);
    }
  }

  public function setSpaces($attrs) {
    $spaces = $this->getSpaces();
    foreach ($spaces as $space_id) {
      if (!in_array($space_id, $attrs)) {
        $this->removeSpace($space_id);
      }
    }
    foreach ($attrs as $space_id) {
      if (!in_array($space_id, $spaces)) {
        $this->addSpace($space_id);
      }
    }
  }

  public function getSpaces() {
    return $this->dbo->query('SELECT space_id'
                             .' FROM adserver_ad_space'
                             .' WHERE model_id='.$this->getModelId()
                             .  ' AND record_id='.$this->getId())->asSet('space_id');
  }

  public function addSpace($space_id, $opts=null) {
    $row = array('space_id' => $space_id,
                 'model_id' => $this->getModelId(),
                 'record_id' => $this->getId());
    if (is_array($opts)) {
      $row += $opts;
    }
    $this->dbo->insert('adserver_ad_space', $row);
  }

  public function removeSpace($space_id) {
    $this->dbo->exec('DELETE FROM adserver_ad_space'
                     .    ' WHERE space_id='.$space_id
                     .      ' AND model_id='.$this->getModelId()
                     .      ' AND record_id='.$this->getId());
  }*/
