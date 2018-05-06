<?php

class Country extends ActiveRecord
{
  const TABLE_IP = 'core_country_ip';

  const FR = 250;
  const ES = 724;

  const FLAG_EUROPEAN = 1;
  const FLAG_TAXABLE = 2;
  const FLAG_SHIPPING = 4;
  const FLAG_DOM = 8;
  const FLAG_TOM = 16;
  const FLAG_3DS = 32;

  public static $flags = array(self::FLAG_EUROPEAN => 'européen',
                               self::FLAG_TAXABLE => 'soumis à la TVA',
//                               self::FLAG_SHIPPING => 'livraison',
                               self::FLAG_DOM => 'DOM',
                               self::FLAG_TOM => 'TOM',
                               self::FLAG_3DS => '3DS');

  const STATUS_OFFLINE = 0;
  const STATUS_ONLINE = 1;

  protected function onBeforeCommit(&$attrs) {
    if (!empty($attrs['name_fr'])) {
      $attrs['name'] = $attrs['name_fr'];
    }
  }

  public function getName($locale=null) {
    $locale = $locale ? $locale : H::locale();
    if ($locale == I18N::LOCALE_FR) return $this->name_fr;
    return $this->name_en;
  }

  public function destroy() { return true; }

  public function isTaxable() {
    return $this->hasFlag(self::FLAG_TAXABLE);
  }

  public function getShippingCost($shipping_id) {
    $rs = $this->dbo->query('SELECT z.cost'
                            .' FROM core_country_zone AS cz'
                            .' JOIN '.T('zone').' AS z'
                            .  ' ON z.id=zone_id'
                            . ' AND z.shipping_id='.intval($shipping_id)
                            .' WHERE cz.country_id='.$this->getId());
    if ($rs === false || $rs->numRows() < 1) {
      S('log')->error('invalid shipping_id ('.$shipping_id.')', __METHOD__);
      throw new ActiveRecordException('invalid shipping');
    }
    return $rs->fetchValue();
  }

  public static function code($id) {
    if (!is_numeric($id)) return $id;
    if ($id == self::FR) return 'FR';
   return S('dbo')->fetchValue('SELECT code'
                               .' FROM '.T('country')
                               .' WHERE id='.intval($id));
  }

  public static function name($id, $locale=null) {
    $field = 'name';
    if ($locale >= 1) {
      $field = $locale == I18N::LOCALE_FR ? 'name_fr' : 'name_en';
    }
    $name = S('dbo')->fetchValue('SELECT '.$field
                                 .' FROM '.T('country')
                                 .' WHERE id='.intval($id));
    return c($name);
  }

  public static function asBackHash() {
    $locale = BackManager::getLocale();
    $suffix = $locale === I18N::LOCALE_FR ? '_fr' : '_en';
    $countries = S('dbo')->asHash('SELECT id, name'.$suffix
                                  .' FROM '.T('country')
                                  .' WHERE status=1'
                                  .' ORDER BY name'.$suffix);
    return $countries;
  }

}