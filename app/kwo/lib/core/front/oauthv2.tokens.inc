<?php

try {

  $url = '/oauthv2.tokens';

  $button = null;
  $session = $req->getSession();
  $oauthv2 = (array) $session->getAttribute('oauthv2'); 
  
  if (!$req->hasAttribute('client_id') && !$req->hasAttribute('code')) {
    $title = 'client tokens';
    $oauthv2 = array();
    $button = array('label' => 'send',
                    'url' => $url);
  }
  elseif ($req->hasAttribute('client_id') && $req->hasAttribute('client_secret')) { 
    $title = 'request code';
    $oauthv2 = array('client_id' => $req->client_id,
                     'client_secret' => $req->client_secret,
                     'redirect_uri' => $req->redirect_uri,
                     'scope' => $req->scope,
                     'provider' => $req->provider,
                     'access_type' => 'offline',
                     'approval_prompt' => 'force'); 
    $client = OAuth::getClient($oauthv2['provider'], $oauthv2);
    $auth_url = $client->getAuthorizationUrl($oauthv2);
    $button = array('label' => 'Authorization',
                    'url' => $auth_url);
    $session->setAttribute('oauthv2', $oauthv2);
    // ==> récupérer le code dans l'url de redirection
  }
  elseif ($req->hasAttribute('code')) {
    $title = 'access tokens';
    $client = OAuth::getClient($oauthv2['provider'], $oauthv2);
    $oauthv2['code'] = $req->code;
    $h = $client->getAccessToken($oauthv2); 
    S('log')->debug($h);
    $oauthv2['access_token'] = $h['access_token'];
    $oauthv2['expires_in'] = $h['expires_in'];
    $oauthv2['refresh_token'] = $h['refresh_token'];
  }

  $ctx->title = $title;
  $ctx->url = $url;
  $ctx->button = $button;
  $ctx->oauthv2 = $oauthv2;

  $res->useTemplate();

}
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}
