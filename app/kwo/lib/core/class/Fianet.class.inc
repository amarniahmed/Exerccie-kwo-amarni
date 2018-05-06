<?php

/*

Paramétrage de l'application pour Fianet

$parameter['shop.fianet'] = array('log' => 'discount-deco',                               
                                  'pwd' => 'pr@uu77m',
                                  'siteid' => '37356',
                                  'privatekey' => '444e42674e07d671d1b593935c0c0e57',
                                  'mode' => 'test' / 'prod');

TABLE ORDER
Champs utilisés par Fianet :
user_ip = adresse ip de l'acheteur obligatoire
fianet_response = réponse fianet
*/

class Fianet 
{

  public static function request($order) {
  
    $parameters = P('shop.fianet');
    if ($parameters['mode'] === 'test') {
      $url = "https://www.fia-net.com/engine/preprod/sendrating.cgi";  
    } else {
      $url= "https://www.fia-net.com/engine/sendrating.cgi";
    }
    $control = FianetControl::getInstance($order);
    $xml = $control->asXml();
    $xml= self::prepareXml($xml);
    $data = 'SiteID='.$parameters['siteid']
            .'&XMLInfo='.$xml
            .'&CheckSum='.md5($xml);
    try {
     if ($response = self::post($url, $data)) {
       $order->setAttribute('fianet_comment',$response);
     }
    } catch (Exception $e) { S('log')->debug('order ('.$order->getId().') non garantie FIANET cause ERREUR connexion'); }
  }
  
  public static function prepareXml($xml) {
    $xml = trim($xml);
    $xml = str_replace("\t","",$xml);
    $xml = str_replace("\n","",$xml);
    $xml = str_replace("\r","",$xml);
    return $xml;
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
        curl_close($ch);
        return false;
      } else {
        curl_close($ch);
        return $answer;
      } 
    }
    return false;
  }
  
}

class FianetControl {
  const DEVISE = 'EUR';
  
  private $dom = null;

  public function __construct($order) {
    $this->dom = new DOMDocument('1.0', 'utf-8');
    $this->dom->xmlStandalone = true;
    $this->dom->formatOutput = true;
    $control = $this->createControl();
    $user = $order->getUser();
    $person = $user->getProfile();
    $utilisateur_hash = array('last_name' => trim($person->last_name),
                              'civility' => $person->civility+1,
                              'first_name' => trim($person->first_name),
                              'email' => $user->email);
    $utilisateur = $this->createUtilisateur($utilisateur_hash);
    $control->appendChild($utilisateur);
    $infosCommande = $this->createInfocommande($order);
    $control->appendChild($infosCommande);
    $crypt = $this->dom->createElement('crypt', self::encrypt($order));
    $control->appendChild($crypt);
    $this->dom->appendChild($control);
  }
  
  public function asXml() {
    return $this->dom->saveXML();
  }

  private function createControl() {
    return $this->dom->createElement('control');
  }
  
  private function createUtilisateur($hash) {
    $utilisateur = $this->dom->createElement('utilisateur');
    
    $nom = $this->dom->createElement('nom', $hash['last_name']);
    $nom->setAttribute('titre', $hash['civility']);
    $utilisateur->appendChild($nom);
  
    $prenom = $this->dom->createElement('prenom', $hash['first_name']);
    $utilisateur->appendChild($prenom);
    
    $email = $this->dom->createElement('email', $hash['email']);
    $utilisateur->appendChild($email);
    
    return $utilisateur;
  }
  
  private function createInfoCommande($order) {
    $infocommande = $this->dom->createElement('infocommande');

    $parameters = P('shop.fianet');
    
    $siteid = $this->dom->createElement('siteid',$parameters['siteid']);
    $infocommande->appendChild($siteid);
    
    $refid = $this->dom->createElement('refid',$order->getId());
    $infocommande->appendChild($refid);
    
    $montant = $this->dom->createElement('montant',$order->total_amount);
    $montant->setAttribute('devise', self::DEVISE);
    $infocommande->appendChild($montant);
    
    $ip = $this->dom->createElement('ip', $order->user_ip);
    $ip->setAttribute('timestamp', $order->paid_at);
    
    $infocommande->appendChild($ip);
    return $infocommande;
  }
  
  private static function encrypt($order) {
    $parameters = P('shop.fianet');
    $string = $parameters['privatekey'] 
              . '_' . $order->getId() 
              . '+' . $order->paid_at
              . '=' . strtolower($order->getUser()->email);
    return md5($string);
  }
  
  public static function getInstance($order) {
    return new FianetControl($order);
  }
}