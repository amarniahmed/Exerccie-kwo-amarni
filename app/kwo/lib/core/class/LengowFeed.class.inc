<?php

/***

ne pas oublier tracking
<img src="https://tracking.lengow.com/lead.php?idClient=#ID_CLIENT_LENGOW#&idGroup=#ID_GROUPE#&price=#PRICE# &idCommande=#ID_COMMANDE#&modePaiement=#MODE_PAIEMENT#&listingProduit=#LISTING_PRODUIT#" alt="" border="0" />

<img src="https://tracking.lengow.com/lead.php?idClient=2188&idGroup=5369&price=<?=$order->total_amount?>&idCommande=<?=$order->id?>"
     alt="" border="0" />

***/

class LengowFeed extends ProductFeed
{
  private $fields = array('categorie', 'categories', 'sous_categorie1', 'sous_categorie2',
                          'identifiant_unique', 'titre', 'prix',
                          'url_produit', 'url_image', 'description', 'frais_de_livraison',
                          'description_de_livraison',
                          'D3E', 'disponibilite', 'delai_de_livraison', 'marque', 'ean',
                          'garantie', 'prix_barre', 'prix_ttc', 'type_promotion', 'reference_modele',
                          'occasion', 'devise', 'url_mobile', 'quantite_en_stock',
                          'couleur', 'genre', 'matiere', 'poids', 'taille', 'occasion', 'soldes',
                          'promo_texte',
                          'pourcentage_promo', 'date_de_debut_de_promo', 'date_de_fin_de_promo',
                          'description_code_promo', 'code_promo', 'shopinfo', 'bundle', 'MPN',
                          'id_parent', 'type_declinaison');
  
  private $catalog = null;
  private $currency = null;
  private $locale = null;

  protected $doc = null;
  
  public function __construct($opts=null) { 
    $this->type = ProductFeed::TYPE_LENGOW;
    $opts = (array) $opts;
    $this->currency = $opts['currency'] ?: 'EUR';
    $this->locale = $opts['locale'] ?: Locale::FR;
    $this->doc = $doc = new DOMDocument('1.0', 'UTF-8');
    $doc->xmlStandalone = true;
    $doc->formatOutput = true;
    $this->catalog = $doc->createElement('products');
    $this->catalog = $doc->appendChild($this->catalog);
  }
  
  public function addItem($item) {
    $product = $this->doc->createElement('product');
    $h = $item instanceOf ActiveRecord ? $item->asProductFeedHash($this) : $item;
    $h['devise'] = $h['devise'] ?: $this->currency;
    foreach ($h as $key => $value) {
      if (!in_array($key, $this->fields)) {
        S('log')->error('invalid key ['.$key.']');
        continue ;
      }
      if (substr($key, 0, 4) === 'url_') {
        $value = H::url($value);
        if ($key === 'url_produit' && strpos($value, 'knxm=') === false) {
          $sep = strpos($value, '?') > 1 ? '&' : '?';
          $value .= $sep.'knxm='.Tracker::MEDIUM_SHOPPING.'&knxs='.self::$types[$this->type]; 
        }
      }
      $value = $this->doc->createCDATASection($value);            
      $node = $this->doc->createElement($key);
      $node->appendChild($value);
      $product->appendChild($node);
    }
    $this->catalog->appendChild($product);
  }
  
  public static function getInstance($opts=null) {
    return new LengowFeed($opts);
  }

}