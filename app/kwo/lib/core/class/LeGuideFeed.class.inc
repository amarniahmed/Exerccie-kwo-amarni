<?php

/****

https://marchands.leguide.com/faq/index?menu=498
http://img.leguide.com/extranet/FR/ExempleLeGuide.xml

****/

class LeGuideFeed extends ProductFeed
{
  private $fields = array('categorie', 'identifiant_unique', 'titre', 'prix',
                          'url_produit', 'url_image', 'description', 'frais_de_livraison',
                          'D3E', 'disponibilite', 'delai_de_livraison', 'marque', 'ean',
                          'garantie', 'prix_barre', 'type_promotion', 'reference_modele',
                          'occasion', 'devise', 'url_mobile');

  private $catalog = null;
  private $currency = null;
  private $locale = null;
  private $place = 1;

  protected $doc = null;

  public function __construct($opts=null) { 
    $this->type = ProductFeed::TYPE_LEGUIDE;
    $opts = (array) $opts;
    $this->currency = $opts['currency'] ?: 'EUR';
    $this->locale = $opts['locale'] ?: Locale::FR;
    $this->doc = $doc = new DOMDocument('1.0', 'UTF-8');
    $doc->xmlStandalone = true;
    $doc->formatOutput = true;
    $this->catalog = $doc->createElement('catalogue');
    $this->catalog = $doc->appendChild($this->catalog);
    $this->catalog->setAttribute('lang', 'FR');
    $this->catalog->setAttribute('date', DH::now());
//    $this->catalog->setAttribute('GMT', '+1');
  }

  public function addItem() {
    $product = $this->doc->createElement('product');
    $product->setAttribute('place', $this->place);
    $h = $item->asProductFeedHash($this);
    $h['devise'] = $h['devise'] ?: $this->currency;
    foreach ($h as $key => $value) {
      if (!in_array($key, $this->fields)) {
        S('log')->error('invalid key ['.$key.']', __METHOD__);
        continue ;
      }
      if (substr($key, 0, 4) === 'url_') {
        $value = H::url($value);
        if ($key === 'url_produit' && strpos($value, 'knxm=') === false) {
          $sep = strpos($value, '?') > 1 ? '&' : '?';
          $value .= $sep.'knxm='.Tracker::MEDIUM_SHOPPING.'&knxs='.self::$types[$this->type]; 
        }
      }
      if (preg_match('/[<>&\'"]/', $value)) {
        $value = $this->doc->createCDATASection($value);            
      }
      if ($value instanceof DOMCDATASection) {
        $node = $this->doc->createElement($key);
        $node->appendChild($value);
      }
      else {
        $node = $this->doc->createElement($key, $value);
      }
      if (substr($key, 0, 4) === 'prix') {
        $node->setAttribute('currency', $h['devise']);
      }
      $product->appendChild($node);
    }
    $this->catalog->appendChild($product);
    $this->place++;
  }
  
  public static function getInstance($opts=null) {
    return new LeGuideFeed($opts);
  }

}