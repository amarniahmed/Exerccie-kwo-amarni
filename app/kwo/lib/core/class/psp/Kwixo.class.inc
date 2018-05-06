<?php

/*
  Nécessite l'utilisation de fonctions de hash MD5 spécifiques à Kwixo
  les classes HashMD532 et HAshMD564 sont fournies par Kwixo
  le choix de la classe se fait grace au parametre
  $register['app.kernel'] défini dans le fichier conf de l'application

*/

/*

Paramétrage de l'application pour Kwixo

$parameter['shop.psp'] = array('kwixo' => array('siteid' => '37356',
                                                'privatekey' => '869edfe0ed23c4732ae48ebf11678f48',
                                                'log' => 'discount-deco',
                                                'pwd' => '7tbwo2Pr',
                                                'carrier_name' => 'nom_transporteur',
                                                'mode' => 'test'));

Table Order :
Champs utilisés

shipping_estimated_date : date de livraison attendu


Table Method :

Options opur Paiement à réception

type=comptant
comptant-rnp=1
comptant-rnp-offert=1

Options pour crédit :

type=credit


*/

final class Kwixo
{

  public static function request($payment) {

    $amount = $payment->amount;
    $method = $payment->getMethod();
    $order = $payment->getOrder();

    $all_parameters = P('shop.psp');
    $parameters = $all_parameters['kwixo'];
    $mode = $parameters['mode'] == 'test' ? 'test' : 'production';
    $control = KwixoControl::getInstance($payment);
    $controlXml = $control->asXml();
    if ($mode === 'test') {
      $url_transaction = "https://recette.kwixo.com/payflow/transaction.htm";
    } else {
      $url_transaction = "https://secure.kwixo.com/transaction.html";
    }

    $datas_retour = array('id' => $order->getId());

    $retourXML = New DomDocument('1.0', 'utf-8');
    $paramcback = $retourXML->createElement('ParamCBack');
    foreach ($datas_retour as $key => $value) {
      $obj = $retourXML->createElement('obj');
      $name = $retourXML->createElement('name', $key);
      $obj->appendChild($name);
      $value = $retourXML->createElement('value', $value);
      $obj->appendChild($value);
      $paramcback->appendChild($obj);
    }
    $retourXML->appendChild($paramcback);
    $retour = $retourXML->saveXML();

    $html = '<form name="formulaire" method="post" action="'.$url_transaction.'">'."\n";;
    $html .= '<input type="hidden" name="MerchID" value="'.$parameters['siteid'].'">'."\n";;
    $html .= "<textarea type='hidden' name='XMLInfo' style='visibility:hidden' >".self::prepareXml($controlXml)."</textarea>"."\n";;
    $html .= '<input type="hidden" name="URLCall" value="'.H::url('/kwixo.response', array('id' => $order->getId())).'">'."\n";;
    $html .= '<input type="hidden" name="URLSys" value="'.H::url('/payment.callback/-/psp/kwixo').'">'."\n";;
    $html .= "<input type='hidden' name='XMLParam' value='".self::prepareXml($retour)."'>"."\n";
    $html .= '<input id="kwixo-button" type="submit" value="accéder au paiement" class="button" style="display:none;">'."\n";
    $html .= '</form>'."\n";
    $html .= '<img src="'.PIX_URL.'/empty.gif" onload="$(\'kwixo-button\').form.submit()" />';

    S('log')->debug($html);

    return $html;
  }

  public static function prepareXml($xml) {
    $xml = trim($xml);
    $xml = str_replace("\t","",$xml);
    $xml = str_replace("\n","",$xml);
    $xml = str_replace("\r","",$xml);
    return $xml;
  }

  public static function callback() {
    $req = S('req');

    $regex = str_replace('.', '\.', join('|', self::$range));
    $address = $req->getClient('address');
    if (!preg_match('/^('.$regex.')/', $address)) {
      throw new Exception('bad source IP ['.$address.']');
    }

    if (strlen($req->RefID) < 3) {
      throw new Exception('no RefID ('.$req->RefID.')');
    }

    list ($order_id, $payment_id, ) = explode('x', $req->RefID);

    $order = Order::getInstance((int) $order_id);

    $amount = number_format($order->total_amount, 2, '.', '');

    if ((int) ($amount * 100) != (int) $req->Montant) {
      throw new Exception('Order ('.$req->RefID.') Amount do not match ('.$req->Montant.' '.(int) ($amount * 100).')');
    }

    $all_parameters = P('shop.psp');
    $parameters = $all_parameters['kwixo'];
    if (P('app.kernel') == '64') {
      $MD5 = new HashMD564();
    } else {
      $MD5 = new HashMD532();
    }

    $HashControl = $MD5->hash($parameters['privatekey'].$req->RefID.$req->TransactionID);

    if ($HashControl != $req->HashControl) {
      throw new Exception('order ('.intval($req->RefID).') invalid HashControl');
    }

    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }
    $payment = Payment::getInstance((int) $payment_id);
    if ($payment->isReceived()) {
      throw new Exception('payment ('.$payment->getId().') already received [order='.$order->getId().']');
    }

    if ((int) $req->Tag == 1 ||
        (int) $req->Tag == 10 ||
        (int) $req->Tag == 13 ||
        (int) $req->Tag == 14 ||
        (int) $req->Tag == 100) {
      $comment = 'RefID='.$req->RefID."\n";
      $comment .= 'Montant='.$req->Montant."\n";
      $comment .= 'TransactionID='.$req->TransactionID."\n";
      $comment .= 'Tag='.$req->Tag."\n";
      $comment .= 'Score='.$req->Score."\n";
      $attributes = array('status' => Payment::STATUS_RECEIVED,
                          'datas' => array('payment_transaction_id' => $req->TransactionID,
                                           'payment_comment' => $comment));
    } else if ((int) $req->Tag == 2 ||
               (int) $req->Tag == 12 ||
               (int) $req->Tag == 11 ||
               (int) $req->Tag == 101) {
      $attributes = array();
      if ($req->Tag == 2) {
        $attributes['status'] = Payment::STATUS_ERROR;
        $str = 'paiement refusé par le centre d’autorisation';
      }
      if ($req->Tag == 11 ||
          $req->Tag == 12) {
        $attributes['status'] = Payment::STATUS_ERROR;
        $str = 'Surveillance KO';
      }
      if ($req->Tag == 101) {
        $attributes['status'] = Payment::STATUS_CANCELED;
        $str = 'Annulation Transaction';
      }
      $attributes['datas'] = array('payment_transaction_id' => $req->TransactionID,
                                   'payment_comment' => '('.$req->Tag.') '.$str);
    } else if ($req->Tag == 6 ||
               $req->Tag == 3 ||
               $req->Tag == 4 ||
               $req->Tag == 5 ||
               $req->Tag == 0) {
      $attributes = array();
      $attributes['datas'] = array('payment_transaction_id' => $req->TransactionID,
                                   'payment_comment' => 'awaiting');
    }
    $payment->commit($attributes);
    $payment->sendLetter();
    $method = $payment->getMethod();
    if ($payment->isLast() || $method->hasFlag(Method::FLAG_INSTALLEMENT)) {
      $order->setStatus(Order::STATUS_PAID);
    }
  }

  private static function post($url, $data) {
    if ($ch = curl_init()) {
      curl_setopt($ch, CURLOPT_VERBOSE, TRUE);
      curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
      curl_setopt($ch, CURLOPT_URL, $url );
      curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
      $answer = curl_exec($ch);
      if($answer == false) {
        S('log')->error('Warning : ' . curl_error($ch));
        curl_close($ch);
        return false;
      } else {
        curl_close($ch);
        return $answer;
      }
    }
    return false;
  }

    private static $range = array('81.88.104.23', '88.81.104.22', '192.168.0.102', '192.168.0.79');

}

class KwixoControl {
  const DEVISE = 'EUR';
  const FACTURATION_TYPE = 'facturation';
  const LIVRAISON_TYPE = 'livraison';
  const QUALITY = 2;
  private $dom = null;

  public function __construct($payment) {


    $method = $payment->getMethod();
    $this->order = $payment->getOrder();
    $this->payment = $payment;

    $this->dom = New DomDocument('1.0', 'utf-8');
    $control = $this->createControl();
    $user = $this->order->getUser();
    $person = $user->getProfile();

    $utilisateur_facturation_hash = array('last_name' => trim($person->last_name),
                                          'civility' => Person::$civilities[$person->civility],
                                          'first_name' => $person->first_name,
                                          'email' => $user->email);
    if (strlen(trim($person->organization)) > 0) {
      $utilisateur_facturation_hash['societe'] = trim($person->organization);
    }
    if (strlen(trim($person->phone)) > 0) {
      // traiter le téléphone pour se débarasser des caractères illégaux
      $utilisateur_facturation_hash['telhome'] = trim($person->phone);
    }
    if (strlen(trim($person->mobile)) > 0) {
      // traiter le téléphone pour se débarasser des caractères illégaux
      $utilisateur_facturation_hash['telmobile'] = trim($person->mobile);
    }

    if (strlen(trim($person->fax)) > 0) {
      // traiter le téléphone pour se débarasser des caractères illégaux
      $utilisateur_facturation_hash['telfax'] = trim($person->fax);
    }
    $utilisateur_facturation = $this->createUtilisateur($utilisateur_facturation_hash, self::FACTURATION_TYPE, self::QUALITY);
    $control->appendChild($utilisateur_facturation);

    $adresse_facturation_hash = array('address' => $person->address,
                                      'address_extra' => $person->address_extra,
                                      'postal_code' => $person->postal_code,
                                      'city' => $person->city,
                                      'country' => Country::getInstance($person->country_id)->code);
    $adresse_facturation = $this->createAdresse($adresse_facturation_hash, self::FACTURATION_TYPE);
    $control->appendChild($adresse_facturation);

    if ($order->addressee_id > 0) {
      $addressee = $order->getAddressee();


      $utilisateur_livraison_hash = array('last_name' => trim($addressee->last_name),
                                          'civility' => Person::$civilities[$addressee->civility],
                                          'first_name' => $addressee->first_name);
      if (strlen(trim($person->phone)) > 0) {
        // traiter le téléphone pour se débarasser des caractères illégaux
        $utilisateur_facturation_hash['telhome'] = trim($person->phone);
      }
      $utilisateur_livraison = $this->createUtilisateur($utilisateur_livraison_hash, self::LIVRAISON_TYPE, self::QUALITY);
      $control->appendChild($utilisateur_livraison);

      $adresse_livraison_hash = array('address' => $addressee->address,
                                      'address_extra' => $addressee->address_extra,
                                      'postal_code' => $addressee->postal_code,
                                      'city' => $addressee->city,
                                      'country' => Country::getInstance($addressee->country_id)->code);
      $adresse_livraison = $this->createAdresse($adresse_livraison_hash, self::FACTURATION_TYPE);
      $control->appendChild($adresse_livraison);
    }

    $infosCommande = $this->createInfocommande($order);
    $control->appendChild($infosCommande);
    $wallet = $this->createWallet($order);
    $control->appendChild($wallet);
    $optionspaiement = $this->createOptionsPaiement($order);
    $control->appendChild($optionspaiement);
    $this->dom->appendChild($control);

  }

  public function asXml() {
    return $this->dom->saveXML();
  }

  private function createControl() {
    return $this->dom->createElement('control');
  }

  private function createUtilisateur($hash, $type, $quality) {
    $utilisateur = $this->dom->createElement('utilisateur');
    $utilisateur->setAttribute('type', $type);
    $utilisateur->setAttribute('qualite', $quality);

    $nom = $this->dom->createElement('nom', $hash['last_name']);
    $nom->setAttribute('titre', $hash['civility']);
    $utilisateur->appendChild($nom);

    $prenom = $this->dom->createElement('prenom', $hash['first_name']);
    $utilisateur->appendChild($prenom);

    if (isset($hash['societe']) &&
        strlen($hash['societe']) > 1) {
      $societe = $this->dom->createElement('societe', $hash['societe']);
      $utilisateur->appendChild($societe);
    }

    if (isset($hash['telhome']) &&
        (strlen($hash['telhome']) == 10 ||
         strlen($hash['telhome']) == 9)) {
      if (strlen($hash['telhome']) == 9) {
        $hash['telhome'] = '+33' . $hash['telhome'];
      }
      $telhome = $this->dom->createElement('telhome', $hash['telhome']);
      $utilisateur->appendChild($telhome);
    }

    if (isset($hash['telmobile']) &&
        (strlen($hash['telmobile']) == 10 ||
         strlen($hash['telmobile']) == 9)) {
      if (strlen($hash['telmobile']) == 9) {
        $hash['telmobile'] = '+33' . $hash['telmobile'];
      }
      $telmobile = $this->dom->createElement('telmobile', $hash['telmobile']);
      $utilisateur->appendChild($telmobile);
    }

    if (isset($hash['telfax']) &&
        (strlen($hash['telfax']) == 10 ||
         strlen($hash['telfax']) == 9)) {
      if (strlen($hash['telfax']) == 9) {
        $hash['telfax'] = '+33' . $hash['telfax'];
      }
      $telfax = $this->dom->createElement('telfax', $hash['telfax']);
      $utilisateur->appendChild($telfax);
    }

    if (isset($hash['teloffice']) &&
        (strlen($hash['teloffice']) == 10 ||
         strlen($hash['teloffice']) == 9)) {
      if (strlen($hash['teloffice']) == 9) {
        $hash['teloffice'] = '+33' . $hash['teloffice'];
      }
      $teloffice = $this->dom->createElement('teloffice', $hash['teloffice']);
      $utilisateur->appendChild($teloffice);
    }

    $email = $this->dom->createElement('email', $hash['email']);
    $utilisateur->appendChild($email);
    return $utilisateur;
  }

  private function createAdresse($hash, $type) {
    $adresse = $this->dom->createElement('adresse');
    $adresse->setAttribute('type', $type);
    $adresse->setAttribute('format', '1');

    $rue1 = $this->dom->createElement('rue1', $hash['address']);
    $adresse->appendChild($rue1);

    $rue2 = $this->dom->createElement('rue2', $hash['address_extra']);
    $adresse->appendChild($rue2);

    $cpostal = $this->dom->createElement('cpostal', $hash['postal_code']);
    $adresse->appendChild($cpostal);

    $ville = $this->dom->createElement('ville', $hash['city']);
    $adresse->appendChild($ville);

    $pays = $this->dom->createElement('pays', $hash['country']);
    $adresse->appendChild($pays);

    if (isset($hash['appartement']) &&
        is_array($hash['appartement'])) {
      $appartement = $this->dom->createElement('appartement');
      foreach ($hash['appartement'] as $key => $value) {
        $item = $this->dom->createElement($key, $value);
        $appartement->appendChild($item);
      }
      $adresse->appendChild($appartement);
    }
    return $adresse;
  }

  private function createInfoCommande() {
    $infocommande = $this->dom->createElement('infocommande');
    $all_parameters = P('shop.psp');
    $parameters = $all_parameters['kwixo'];

    $refid = $this->dom->createElement('refid',$this->order->getId().'x'.$this->payment->getId());
    $infocommande->appendChild($refid);

    $siteid = $this->dom->createElement('siteid',$parameters['siteid']);
    $infocommande->appendChild($siteid);



    $amount = number_format($this->order->total_amount, 2, '.', '');

    $montant = $this->dom->createElement('montant',$amount);
    $montant->setAttribute('devise', self::DEVISE);
    $infocommande->appendChild($montant);

    $transporteur = $this->order->getCarrier()->name;
    $purchases = $this->order->getPurchases();

    // Cas normal à étoffer, DiscountDecp bloqué sur 4
    //$transport_type = ($order->shipping_method == Order::SHIPPING_METHOD_DELIVERY) ? '4' : '1';
    $transport_type = 4;
    $transport = $this->dom->createElement('transport');
    $type =  $this->dom->createElement('type', $transport_type);
    $transport->appendChild($type);

    // Cas DiscountDeco, voir comment gérer la rapidité de manière pérenne
    $rapidite =  $this->dom->createElement('rapidite', '2');
    $transport->appendChild($rapidite);

    // Bonne façon de nommer le transporteur, DicountDeco = Cas spécial
    // $nom =  $this->dom->createElement('nom', $transporteur);
    $nom =  $this->dom->createElement('nom', $parameters['carrier_name']);
    $transport->appendChild($nom);

    $infocommande->appendChild($transport);

    $list = $this->dom->createElement('list');
    $list->setAttribute('nbproduit', $purchases->foundRows());
    foreach ($purchases as $purchase) {
      $produit = $this->dom->createElement('produit', $purchase->name);
      $produit->setAttribute('type', 4); // fixé pour DiscountDeco
      $produit->setAttribute('ref', $purchase->code);
      $produit->setAttribute('nb', $purchase->quantity);
      $list->appendChild($produit);
    }
    $infocommande->appendChild($list);
    return $infocommande;
  }

  private function createWallet() {

    $timestamp = time();
    $wallet = $this->dom->createElement('wallet');
    $datelivr = $this->dom
                     ->createElement('datelivr',($this->order->shipping_estimated_date != '0000-00-00') ?
                                                date('Y-m-d', strtotime($this->order->shipping_estimated_date.' +120 hours'))
                                                : date('Y-m-d', strtotime('+192 hours')));
    $wallet->appendChild($datelivr);
    $datecom = $this->dom->createElement('datecom', date('Y-m-d H:i:s', $timestamp));
    $wallet->appendChild($datecom);

    $crypt = new EncodingKey();
    $all_parameters = P('shop.psp');
    $parameters = $all_parameters['kwixo'];
    $amount = number_format($this->order->total_amount, 2, '.', '');

    $crypt_value = $crypt->giveHashCode2($parameters['privatekey'],
                            date('s', $timestamp),
                            $this->order->getUser()->email,
                            $this->order->getId().'x'.$this->payment->getId(),
                            $amount,
                            $this->order->getUser()->getProfile()->last_name);
    $crypt = $this->dom->createElement('crypt', $crypt_value);
    $crypt->setAttribute('version', '2.0');
    $wallet->appendChild($crypt);

    return $wallet;
  }

  private function createOptionsPaiement() {
    $optionspaiement = $this->dom->createElement('options-paiement');
    $method = $this->order->getMethod();
    $options = String::asHash($method->options);
    foreach ($options as $key => $value) {
      $optionspaiement->setAttribute($key,$value);
    }

    return $optionspaiement;
  }

  private static function encrypt($refid, $montant, $email, $nom, $second) {
    $refid = trim($refid);
    $montant = trim($montant);
    $email = trim($email);
    $nom = trim($nom);

    $all_parameters = P('shop.psp');
    $parameters = $all_parameters['kwixo'];
    $modulo = $second % 4;
    switch($modulo) {
      case 0:
          $select = $montant;
      break;
      case 1:
          $select = $email;
      break;
      case 2:
          $select = $refid;
      break;
      case 3:
          $select = $nom;
      break;
      default:
      break;
    }
		return md5($parameters['privatekey'].$refid.$montant.$email.$select);
  }

  public static function getInstance($order) {
    return new KwixoControl($order);
  }

}

Class HashMD532 {
  var $a;
  var $b;
  var $c;
  var $d;

  var $Save_a;
  var $Save_b;
  var $Save_c;
  var $Save_d;

  function HashMD532() {
    /*
    $a =  1732584190;
    $b = -271733879;
    $c = -1732584194;
    $d =  271733878; */
    $this->a = -279229019;
    $this->b = -1875190530;
    $this->c = 1737040641;
    $this->d = 315143286;

    $this->Save_a = $this->a;
    $this->Save_b = $this->b;
    $this->Save_c = $this->c;
    $this->Save_d = $this->d;
  }

  function FlushKey($k0, $k1, $k2, $k3) {
    $this->a = $k0;
    $this->b = $k1;
    $this->c = $k2;
    $this->d = $k3;

    $this->Save_a = $this->a;
    $this->Save_b = $this->b;
    $this->Save_c = $this->c;
    $this->Save_d = $this->d;
  }

  function Init() {
    $this->a = $this->Save_a;
    $this->b = $this->Save_b;
    $this->c = $this->Save_c;
    $this->d = $this->Save_d;
  }

  function rhex($num) {
    $hex_chr = "0123456789abcdef";
    $str = "";
    for($j = 0; $j <= 3; $j++)
      $str .= substr($hex_chr, ($num >> ($j * 8 + 4)) & 0x0F, 1) . substr($hex_chr, ($num >> ($j * 8)) & 0x0F, 1);
    return $str;
  }

  function str2blks_MD5($str) {
    $nblk = ((strlen($str) + 8) >> 6) + 1;
    for($i = 0; $i < $nblk * 16; $i++) $blks[$i] = 0;
    for($i = 0; $i < strlen($str); $i++)
      $blks[$i >> 2] |= ord(substr($str, $i, 1)) << (($i % 4) * 8);
    $blks[$i >> 2] |= 0x80 << (($i % 4) * 8);
    $blks[$nblk * 16 - 2] = strlen($str) * 8;
    return $blks;
  }

  function add($x, $y) {
    $lsw = ($x & 0xFFFF) + ($y & 0xFFFF);
    $msw = ($x >> 16) + ($y >> 16) + ($lsw >> 16);
    return ($msw << 16) | ($lsw & 0xFFFF);
  }

  function rol($num, $cnt) {
    return ($num << $cnt) | $this->zeroFill($num, 32 - $cnt);
  }

  function zeroFill($a, $b) {
    $bin = decbin($a);
    if (strlen($bin) < $b) $bin = 0;
    else $bin = substr($bin, 0, strlen($bin) - $b);
    for ($i=0; $i < $b; $i++) {
      $bin = "0".$bin;
    }
    return bindec($bin);
  }

  function cmn($q, $a, $b, $x, $s, $t) {
    return $this->add($this->rol($this->add($this->add($a, $q), $this->add($x, $t)), $s), $b);
  }

  function ff($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn(($b & $c) | ((~$b) & $d), $a, $b, $x, $s, $t);
  }

  function gg($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn(($b & $d) | ($c & (~$d)), $a, $b, $x, $s, $t);
  }

  function hh($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn($b ^ $c ^ $d, $a, $b, $x, $s, $t);
  }

  function ii($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn($c ^ ($b | (~$d)), $a, $b, $x, $s, $t);
  }

  function hash($str) {
    $x = $this->str2blks_MD5($str);

    $this->Init();

    for($i = 0; $i < sizeof($x); $i += 16) {
      $olda = $this->a;
      $oldb = $this->b;
      $oldc = $this->c;
      $oldd = $this->d;

      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+ 0], 7 , -680876936);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+ 1], 12, -389564586);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+ 2], 17,  606105819);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+ 3], 22, -1044525330);
      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+ 4], 7 , -176418897);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+ 5], 12,  1200080426);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+ 6], 17, -1473231341);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+ 7], 22, -45705983);
      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+ 8], 7 ,  1770035416);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+ 9], 12, -1958414417);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+10], 17, -42063);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+11], 22, -1990404162);
      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+12], 7 ,  1804603682);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+13], 12, -40341101);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+14], 17, -1502002290);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+15], 22,  1236535329);

      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+ 1], 5 , -165796510);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+ 6], 9 , -1069501632);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+11], 14,  643717713);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+ 0], 20, -373897302);
      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+ 5], 5 , -701558691);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+10], 9 ,  38016083);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+15], 14, -660478335);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+ 4], 20, -405537848);
      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+ 9], 5 ,  568446438);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+14], 9 , -1019803690);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+ 3], 14, -187363961);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+ 8], 20,  1163531501);
      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+13], 5 , -1444681467);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+ 2], 9 , -51403784);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+ 7], 14,  1735328473);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+12], 20, -1926607734);

      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+ 5], 4 , -378558);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+ 8], 11, -2022574463);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+11], 16,  1839030562);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+14], 23, -35309556);
      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+ 1], 4 , -1530992060);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+ 4], 11,  1272893353);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+ 7], 16, -155497632);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+10], 23, -1094730640);
      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+13], 4 ,  681279174);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+ 0], 11, -358537222);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+ 3], 16, -722521979);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+ 6], 23,  76029189);
      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+ 9], 4 , -640364487);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+12], 11, -421815835);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+15], 16,  530742520);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+ 2], 23, -995338651);

      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+ 0], 6 , -198630844);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+ 7], 10,  1126891415);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+14], 15, -1416354905);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+ 5], 21, -57434055);
      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+12], 6 ,  1700485571);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+ 3], 10, -1894986606);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+10], 15, -1051523);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+ 1], 21, -2054922799);
      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+ 8], 6 ,  1873313359);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+15], 10, -30611744);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+ 6], 15, -1560198380);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+13], 21,  1309151649);
      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+ 4], 6 , -145523070);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+11], 10, -1120210379);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+ 2], 15,  718787259);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+ 9], 21, -343485551);

      $this->a = $this->add($this->a, $olda);
      $this->b = $this->add($this->b, $oldb);
      $this->c = $this->add($this->c, $oldc);
      $this->d = $this->add($this->d, $oldd);
    }
    return $this->rhex($this->a) . $this->rhex($this->b) . $this->rhex($this->c) . $this->rhex($this->d);
  }
}

Class HashMD564 {
    var $a;
    var $b;
    var $c;
    var $d;

    var $Save_a;
    var $Save_b;
    var $Save_c;
    var $Save_d;

  function HashMD564() {
    /*
    $a =  1732584190;
    $b = -271733879;
    $c = -1732584194;
    $d =  271733878; */
    $this->a = -279229019;
    $this->b = -1875190530;
    $this->c = 1737040641;
    $this->d = 315143286;

    $this->Save_a = $this->a;
    $this->Save_b = $this->b;
    $this->Save_c = $this->c;
    $this->Save_d = $this->d;
  }

  function FlushKey($k0, $k1, $k2, $k3) {
    $this->a = $k0;
    $this->b = $k1;
    $this->c = $k2;
    $this->d = $k3;

    $this->Save_a = $this->a;
    $this->Save_b = $this->b;
    $this->Save_c = $this->c;
    $this->Save_d = $this->d;
  }

  function Init() {
    $this->a = $this->Save_a;
    $this->b = $this->Save_b;
    $this->c = $this->Save_c;
    $this->d = $this->Save_d;
  }

  function rhex($num) {
    $hex_chr = "0123456789abcdef";
    $str = "";
    for($j = 0; $j <= 3; $j++)
      $str .= substr($hex_chr, ($num >> ($j * 8 + 4)) & 0x0F, 1) . substr($hex_chr, ($num >> ($j * 8)) & 0x0F, 1);
    return $str;
  }

  function str2blks_MD5($str) {
    $nblk = ((strlen($str) + 8) >> 6) + 1;
    for($i = 0; $i < $nblk * 16; $i++) $blks[$i] = 0;
    for($i = 0; $i < strlen($str); $i++)
      $blks[$i >> 2] |= ord(substr($str, $i, 1)) << (($i % 4) * 8);
    $blks[$i >> 2] |= 0x80 << (($i % 4) * 8);
    $blks[$nblk * 16 - 2] = strlen($str) * 8;
    return $blks;
  }

  function parse_unsigned_int($s) {
    $x = (float)$s;
    if($x > (float)2147483647)
       $x -= (float)"4294967296";
    if($x < (float)-2147483648)
       $x += (float)"4294967296";
    return (int)$x;
  }

  function add($x, $y) {
    $lsw = ($x & 0xFFFF) + ($y & 0xFFFF);
    $msw = ($x >> 16) + ($y >> 16) + ($lsw >> 16);
    return $this->parse_unsigned_int(($msw << 16) | ($lsw & 0xFFFF));
  }

  function rol($num, $cnt) {
    $num = $num & 0xFFFFFFFF;
    return $this->parse_unsigned_int(($num << $cnt) & 0xFFFFFFFF | $this->zeroFill($num, 32 - $cnt));
  }

  function zeroFill($a, $b) {
    $bin = decbin($a);
    if (strlen($bin) < $b) $bin = 0;
    else $bin = substr($bin, 0, strlen($bin) - $b);
    for ($i=0; $i < $b; $i++) {
      $bin = "0".$bin;
    }
    return bindec($bin);
  }

  function cmn($q, $a, $b, $x, $s, $t) {
    return $this->add($this->rol($this->add($this->add($a, $q), $this->add($x, $t)), $s), $b);
  }

  function ff($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn(($b & $c) | ((~$b) & $d), $a, $b, $x, $s, $t);
  }

  function gg($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn(($b & $d) | ($c & (~$d)), $a, $b, $x, $s, $t);
  }

  function hh($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn($b ^ $c ^ $d, $a, $b, $x, $s, $t);
  }

  function ii($a, $b, $c, $d, $x, $s, $t) {
    return $this->cmn($c ^ ($b | (~$d)), $a, $b, $x, $s, $t);
  }

  function hash($str) {

    $x = $this->str2blks_MD5($str);
    $this->Init();

    for($i = 0; $i < sizeof($x); $i += 16) {
      $olda = $this->a;
      $oldb = $this->b;
      $oldc = $this->c;
      $oldd = $this->d;

      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+ 0], 7 , -680876936);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+ 1], 12, -389564586);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+ 2], 17,  606105819);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+ 3], 22, -1044525330);
      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+ 4], 7 , -176418897);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+ 5], 12,  1200080426);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+ 6], 17, -1473231341);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+ 7], 22, -45705983);
      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+ 8], 7 ,  1770035416);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+ 9], 12, -1958414417);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+10], 17, -42063);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+11], 22, -1990404162);
      $this->a = $this->ff($this->a, $this->b, $this->c, $this->d, $x[$i+12], 7 ,  1804603682);
      $this->d = $this->ff($this->d, $this->a, $this->b, $this->c, $x[$i+13], 12, -40341101);
      $this->c = $this->ff($this->c, $this->d, $this->a, $this->b, $x[$i+14], 17, -1502002290);
      $this->b = $this->ff($this->b, $this->c, $this->d, $this->a, $x[$i+15], 22,  1236535329);

      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+ 1], 5 , -165796510);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+ 6], 9 , -1069501632);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+11], 14,  643717713);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+ 0], 20, -373897302);
      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+ 5], 5 , -701558691);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+10], 9 ,  38016083);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+15], 14, -660478335);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+ 4], 20, -405537848);
      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+ 9], 5 ,  568446438);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+14], 9 , -1019803690);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+ 3], 14, -187363961);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+ 8], 20,  1163531501);
      $this->a = $this->gg($this->a, $this->b, $this->c, $this->d, $x[$i+13], 5 , -1444681467);
      $this->d = $this->gg($this->d, $this->a, $this->b, $this->c, $x[$i+ 2], 9 , -51403784);
      $this->c = $this->gg($this->c, $this->d, $this->a, $this->b, $x[$i+ 7], 14,  1735328473);
      $this->b = $this->gg($this->b, $this->c, $this->d, $this->a, $x[$i+12], 20, -1926607734);

      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+ 5], 4 , -378558);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+ 8], 11, -2022574463);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+11], 16,  1839030562);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+14], 23, -35309556);
      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+ 1], 4 , -1530992060);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+ 4], 11,  1272893353);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+ 7], 16, -155497632);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+10], 23, -1094730640);
      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+13], 4 ,  681279174);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+ 0], 11, -358537222);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+ 3], 16, -722521979);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+ 6], 23,  76029189);
      $this->a = $this->hh($this->a, $this->b, $this->c, $this->d, $x[$i+ 9], 4 , -640364487);
      $this->d = $this->hh($this->d, $this->a, $this->b, $this->c, $x[$i+12], 11, -421815835);
      $this->c = $this->hh($this->c, $this->d, $this->a, $this->b, $x[$i+15], 16,  530742520);
      $this->b = $this->hh($this->b, $this->c, $this->d, $this->a, $x[$i+ 2], 23, -995338651);

      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+ 0], 6 , -198630844);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+ 7], 10,  1126891415);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+14], 15, -1416354905);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+ 5], 21, -57434055);
      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+12], 6 ,  1700485571);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+ 3], 10, -1894986606);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+10], 15, -1051523);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+ 1], 21, -2054922799);
      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+ 8], 6 ,  1873313359);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+15], 10, -30611744);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+ 6], 15, -1560198380);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+13], 21,  1309151649);
      $this->a = $this->ii($this->a, $this->b, $this->c, $this->d, $x[$i+ 4], 6 , -145523070);
      $this->d = $this->ii($this->d, $this->a, $this->b, $this->c, $x[$i+11], 10, -1120210379);
      $this->c = $this->ii($this->c, $this->d, $this->a, $this->b, $x[$i+ 2], 15,  718787259);
      $this->b = $this->ii($this->b, $this->c, $this->d, $this->a, $x[$i+ 9], 21, -343485551);

      $this->a = $this->add($this->a, $olda);
      $this->b = $this->add($this->b, $oldb);
      $this->c = $this->add($this->c, $oldc);
      $this->d = $this->add($this->d, $oldd);
    }
    return $this->rhex($this->a) . $this->rhex($this->b) . $this->rhex($this->c) . $this->rhex($this->d);
  }
}

Class EncodingKey {
  var $clMD5;
  function EncodingKey() {
    if (P('app.kernel') == '64') {
      $this->clMD5 = new HashMD564();
    } else {
      $this->clMD5 = new HashMD532();
    }
  }

  function giveHashCode($pkey, $second, $email, $refid, $montant, $nom) {
    $modulo = $second % 4;
    switch($modulo) {
      case 0:
          $select = $montant;
      break;
      case 1:
          $select = $email;
      break;
      case 2:
          $select = $refid;
      break;
      case 3:
          $select = $nom;
      break;
      default:
      break;
    }
    return $this->clMD5->hash($pkey.$refid.$select);
  }

  function giveHashCode2($pkey, $second, $email, $refid, $montant, $nom) {
    $modulo = $second % 4;
    switch($modulo) {
      case 0:
          $select = $montant;
      break;
      case 1:
          $select = $email;
      break;
      case 2:
          $select = $refid;
      break;
      case 3:
          $select = $nom;
      break;
      default:
      break;
    }
    return $this->clMD5->hash($pkey.$refid.$montant.$email.$select);
  }
}