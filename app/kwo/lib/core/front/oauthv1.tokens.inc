<?php

try {
  
  $url = '/oauthv1.tokens';
    
  $provider = OAuth::PROVIDER_TUMBLR;
  $button = null;
  $session = $req->getSession();
  $tokens = (array) $session->getAttribute('tokens');

  if (!$req->hasAttribute('oauth_token') && !$req->hasAttribute('consumer_key')) {
    $title = 'consumer tokens';
    $tokens = array();
    $button = array('label' => 'send',
                    'url' => $url);
    $session->setAttribute('tokens', $tokens);
  }
  elseif ($req->hasAttribute('consumer_key') && $req->hasAttribute('consumer_secret')) { 
    $title = 'request tokens';
    $tokens = array('consumer_key' => $req->consumer_key,
                    'consumer_secret' => $req->consumer_secret); 
    $client = OAuth::getClient($provider, $tokens);
//    $callback_url = H::url($url);
//    $callback_url = 'http://www.scopalto.com/sys/oauth.callback';
    $h = $client->getRequestTokens($req->callback_url);
    $tokens['oauth_token'] = $h['oauth_token'];
    $tokens['oauth_token_secret'] = $h['oauth_token_secret'];
    // twitter accepte t il le auth_callback ?
    $auth_url = $client->getAuthorizationUrl($tokens['oauth_token'], 
                                             array('callback_url' => $callback_url));
    $button = array('label' => 'Authorization',
                    'url' => $auth_url);
    $session->setAttribute('tokens', $tokens);
  }
  elseif ($req->hasAttribute('oauth_token') && $req->hasAttribute('oauth_verifier')) {
    $title = 'access tokens';
    $tokens['oauth_token'] = $req->oauth_token;
    $client = OAuth::getClient($provider, $tokens);
    $params = null;
    // le passage de "oauth_verifier" n'est peut etre pas utile pour twitter
    $params = array('oauth_verifier' => $req->oauth_verifier);
    $h = $client->getAccessTokens($params); 
    $tokens['oauth_token'] = $h['oauth_token'];
    $tokens['oauth_token_secret'] = $h['oauth_token_secret'];
  }

  $ctx->title = $title;
  $ctx->url = $url;
  $ctx->button = $button;
  $ctx->tokens = $tokens;

  $ctx->consumer_key = ''; 
  $ctx->consumer_secret = ''; 

  $res->useTemplate();

}
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}
