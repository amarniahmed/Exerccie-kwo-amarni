<?php 

/***
lgue : FR EN DE IT ES NL
***/

class Cybermut {
  
  const KIT_PASSWORD = 'YouGotThisPassPhrase';
  const KIT_VERSION = '3.0';//1.2open

  private static function config() {
    $config = P('shop.psp');
    $config = $config['cybermut'];
    return $config;
  }

  public static function request($item) {
    $config = self::config();
    $params = $config['params'];
    $mode = $config['mode'] == 'test' ? 'test/' : '';

    if ($params['bank'] == 'cm') {
      $server_url = 'https://paiement.creditmutuel.fr/'.$mode.'paiement.cgi';
    }
    elseif ($params['bank'] == 'cic') {
      $server_url = 'https://ssl.paiement.cic-banques.fr/'.$mode.'paiement.cgi';
    }
    else {
      $server_url = 'https://ssl.paiement.banque-obc.fr/'.$mode.'paiement.cgi';
    }
    
    $url = H::url('/payment');
 
    $h = array('version' => self::KIT_VERSION,
               'TPE' => $params['tpe'],
               'date' => date('d/m/Y:H:i:s'),
               'montant' => round($item->total_amount, 2) . 'EUR',
               'reference' => $item->getModelId().'x'.$item->getId(),
               'url_retour' => $url.'.response/-/id/'.$item->getId(),
               'url_retour_ok' => $url.'.response/-/id/'.$item->getId(),
               'url_retour_err' => $url.'.cancel',
               'lgue' => 'FR',
               'societe' => $params['societe'],
               'texte-libre' => '-',
               'bouton' => 'Paiement CB - Card Payment');
    
    $data = '';
    $keys = array('TPE', 'date', 'montant', 'reference', 'texte-libre', 
                  'version', 'lgue', 'societe');
    foreach ($keys as $key) {
      $data .= $h[$key].'*';
    } 
 
    $h['MAC'] = self::hmac($params, $data);

    $form = Template::render('psp.cybermut', 
                             array('h' => $h, 
                                   'server_url' => $server_url));
    return $form;
  }
  
  public static function call($item, $card) {

    $config = self::config();
    $params = $config['params'];

    $user = $item->getUser();

    $h = array('version' => self::KIT_VERSION,
               'TPE' => $params['tpe'],
               'date' => date('d/m/Y:H:i:s'),
               'montant' => round($item->total_amount, 2) . 'EUR',
               'reference' => $item->getModelId().'x'.$item->getId(),
               'texte-libre' => '-',
               'lgue' => 'FR',
               'societe' => $params['societe'],
               'mail' => $user->email,
               'MAC' => '',
               'numero_carte' => $card['numero'],
               'annee_validite' => (int)$card['annee'],
               'mois_validite' => (int)$card['mois'],
               'cvx' => (int) $card['cvx']);
    
    $data = '';
    $keys = array('TPE', 'date', 'montant', 'reference', 'texte-libre', 
                  'version', 'lgue', 'societe', 'mail',
                  'nbrech', 
                  'dateech1', 'montantech1', 'dateech2', 'montantech2',
                  'dateech3', 'montantech3', 'dateech4', 'montantech4',);
    foreach ($keys as $key) {
      $data .= $h[$key].'*';
    } 

    $h['MAC'] = self::hmac($params, $data);

    if ($params['bank'] == 'cm') {
      $server = 'paiement.creditmutuel.fr';
    }
    elseif ($params['bank'] == 'cic') {
      $server = 'ssl.paiement.cic-banques.fr';
    }
    else {
      $server = 'ssl.paiement.banque-obc.fr';
    }
    $mode = $config['mode'] == 'test' ? 'test/' : '';
    $url = 'https://'.$server.'/'.$mode.'emulation3ds.cgi';
    
    $headers = array();
    $headers['Pragma'] = 'no-cache';
    $headers['Connection'] = 'close';
    $headers['User-Agent'] = 'AuthClient';

    $datas = HttpClient::post($url, $h, array('headers' => $headers));

    if (strpos($datas, '<xml>') === false) {
      S('log')->debug($context, $url);
      S('log')->error($datas, __METHOD__);
      return false;
    }
    
    //S('log')->trace($datas, __METHOD__);

    $xml = new SimpleXMLElement($datas);

    $code = (int) $xml->cdr;

    if ($code != 1) {
      S('log')->debug($context, $url);
      S('log')->debug($h);
      S('log')->error($datas, __METHOD__);
      $item->commit(array('payment_comment' => self::$errors[$code]));
      return false;
    }

    $item->commit(array('status' => Order::STATUS_PAID,
                        'payment_comment' => ''));
    
    return true;

  }

  public static function callback() {

    $req = S('req');

    $config = self::config();
    $params = $config['params'];
    
    $data = (string) $req->getAttribute('retourPLUS');
    $keys = array('TPE', 'date', 'montant', 'reference', 'texte-libre', 
                  'version', 'code-retour');
    foreach ($keys as $key) {
      $data .= $key === 'version' ? self::KIT_VERSION : (string) $req->getAttribute($key);
      $data .= '+';
    } 

    $retour = self::hmac($params, $data) == strtolower($req->MAC)
            ? 'OK'
            : 'Document Falsifie 0--';
    
    if ($retour != 'OK') {
      S('log')->debug($params);
      S('log')->trace('DATA : '.$data);
      S('log')->trace('CMN MAC : '.strtolower($req->MAC));
      S('log')->trace('KNX MAC : '.CMCIC_hmac($params, $data));
    }

    S('res')->write('Pragma: no-cache'."\n"
                    .'Content-type: text/plain'."\n"
                    .'Version: 1'."\n"
                    .$retour);
    try {
      list($model_id, $record_id) = explode('x', $req->reference);
      if ($model_id < 1 || $record_id < 1) {
        S('log')->error('invalid reference ['.$req->reference.']', __METHOD__);
        throw new Exception('invalid reference');
      }
      $item = Item::getInstance((int) $model_id,
                                (int) $record_id);
      $cr = $req->getAttribute('code-retour');
      if ($config['mode'] === 'test') {
        $status = $cr === 'payetest' ? Order::STATUS_PAID : Order::STATUS_ERROR;
      }
      else {
        $status = $cr === 'paiement' ? Order::STATUS_PAID : Order::STATUS_CANCELLED;
      }
      $item->commit(array('status' => $status));
    }
    catch (Exception $e) { throw $e; }
  }

  public static function response() {}

  public static function hmac($params, $data) {
    switch (self::KIT_VERSION) {
      case '3.0':
        $hexStrKey  = substr($params['key'], 0, 38);
        $hexFinal   = "" . substr($params['key'], 38, 2) . "00";
        
        $cca0 = ord($hexFinal); 
        
        if ($cca0 > 70 && $cca0 < 97) {
          $hexStrKey .= chr($cca0 - 23) . substr($hexFinal, 1, 1);
        }
        else { 
          if (substr($hexFinal, 1, 1) == "M") {
            $hexStrKey .= substr($hexFinal, 0, 1) . "0"; 
          }
          else {
            $hexStrKey .= substr($hexFinal, 0, 2);
          }
        }
        return strtolower(hash_hmac('sha1', $data, pack("H*", $hexStrKey)));
      case '1.2open':
        $k1 = pack('H*', sha1(self::KIT_PASSWORD));
        $l1 = strlen($k1);
        $k2 = pack('H*', $params['key']);
        $l2 = strlen($k2);
        if ($l1 > $l2) {
          $k2 = str_pad($k2, $l1, chr(0x00));
        }
        elseif ($l2 > $l1) {
          $k1 = str_pad($k1, $l2, chr(0x00));
        }
        return strtolower(hash_hmac('sha1', $data, $k1 ^ $k2));
    }
    S('log')->error('invalid KIT_VERSION');
  }
  
  private static $errors = array(0 => 'Paiement non effectué',
                                 1 => 'Mise en recouvrement effectuée',
                                 2 => 'Résultat de l’étape 1 pour une carte enrôlée 3DSecure',
                                 -1 => 'Problème technique',
                                 -2 => 'Commerçant non identifié',
                                 -3 => 'Commande non authentifiée',
                                 -4 => 'CB expirée',
                                 -5 => 'Numéro de CB erroné',
                                 -6 => 'Commande expirée',
                                 -7 => 'Montant erroné',
                                 -8 => 'Date erronée',
                                 -9 => 'CVX erroné',
                                 -10 => 'Paiement déjà autorisé',
                                 -11 => 'Paiement déjà accepté',
                                 -12 => 'Paiement déjà annulé',
                                 -13 => 'Traitement en cours',
                                 -14 => 'Commande grillée',
                                 -15 => 'Erreur paramètres',
                                 -16 => 'Erreur résultat d’authentification 3D-Secure',
                                 -17 => 'Le montant des échéances est erroné',
                                 -18 => 'La date des échéances est erronée',
                                 -19 => 'Le nombre d’échéance n’est pas correct',
                                 -20 => 'La version envoyée n’est pas correcte');

}







/***
Version 1.2open
Fonctions présentes dans le Kit : CMCIC_HMAC.inc.php 
***/

/* RFC2104 - HMAC: Keyed-Hashing for Message Authentication */
/*function CMCIC_hmac($CMCIC_Tpe, $data="") {
  $pass = "YouGotThisPassPhrase";
  $k1 = pack("H*", sha1($pass));
  $l1 = strlen($k1);
  $k2 = pack("H*", $CMCIC_Tpe['key']);
  $l2 = strlen($k2);
  if ($l1 > $l2):
    $k2 = str_pad($k2, $l1, chr(0x00));
  elseif ($l2 > $l1):
    $k1 = str_pad($k1, $l2, chr(0x00));
  endif;
    
  if ($data==""):
    $d = "CtlHmac".'1.2open'.$CMCIC_Tpe['tpe'];
  else:
    $d = $data;
  endif;
  //  return strtolower(hmac_sha1($k1 ^ $k2, $d));
  return strtolower(hash_hmac('sha1', $d, $k1 ^ $k2));
}*/

/*function hmac_sha1($key, $data) {
  $length = 64; 
  if (strlen($key) > $length) { $key = pack("H*",sha1($key)); }
  $key  = str_pad($key, $length, chr(0x00));
  $ipad = str_pad('', $length, chr(0x36));
  $opad = str_pad('', $length, chr(0x5c));
  $k_ipad = $key ^ $ipad ;
  $k_opad = $key ^ $opad;
  return sha1($k_opad  . pack("H*",sha1($k_ipad . $data)));
}*/



/***
Version 3.0
Fonction présente dans le le fichier php5/CMCIC_Tpe.inc.php : 
***/

/* 
Fonction pour transformer la clef hmac-sha1 qu'ils nous fournissent

 function _getUsableKey($key){
    $hexStrKey  = substr($key, 0, 38);
    $hexFinal   = "" . substr($key, 38, 2) . "00";
    
    $cca0=ord($hexFinal); 
    
    if ($cca0>70 && $cca0<97) 
      $hexStrKey .= chr($cca0-23) . substr($hexFinal, 1, 1);
    else { 
      if (substr($hexFinal, 1, 1)=="M") 
        $hexStrKey .= substr($hexFinal, 0, 1) . "0"; 
      else 
        $hexStrKey .= substr($hexFinal, 0, 2);
    }
    return pack("H*", $hexStrKey);
  }
*/

/*** 
pour tests 1.2open:
 - $data = '6058032*09/09/2008:15:33:44*13.00EUR*1263x35*-*1.2open*FR*annakaszer*';
 - $mac = '6b45197c3c300ace93a3e3cca6c6c25fb5a1652e';
pour tests 3.0:
 - $data = '6205797*14/05/2010:15:38:32*40EUR*507x123*-*3.0*FR*EMU_familytrip*thomas@kernix.com**********';
 - $mac = 'c6393acd799049646ad78534dd6134ebf90b3e46';
***/