<?php

class Controller
{
  public function __construct() { }

  public function invoke() {

    $ctx = S('ctx');
    $dbo = S('dbo');
    $err = S('err');
    $log = S('log');
    $req = S('req');
    $res = S('res');

    if ($req->isApi()) {
      if (ApiManager::accept() === false) return ;
    }
    elseif (($req->isFront() || $req->isAccount() || $req->isMiddle())) {
      if (AuthManager::onStartUp() === false) return ;
      if ($req->isCached() && count($err) < 1) return ;
    }

    if ($req->isBack()) {
      $admin = S('admin');
      foreach ($req->getBranch() as $key => $value) {
        if ($ctx->hasAttribute($key)) continue ;
        $ctx->setAttribute($key, $value);
      }
      if ($req->hasAttribute('branches')) {
        foreach ($req->getAttribute('branches') as $key => $value) {
          if ($ctx->hasAttribute($key)) continue ;
          $ctx->setAttribute($key, $value);
        }
      }
      if ($ctx->getAttribute('_auth') == 1) {
        $admin->track(Interaction::TYPE_AUTH);
      }
    }
    elseif ($req->isAccount()) {
      $user = $ctx->getAttribute('user');
    }
    elseif ($req->isMiddle()) {
      $user = $ctx->getAttribute('user');
      $ctx->setAttribute('_modules', P('middle.modules'));
    }
    elseif ($req->isApi()) {
      if ($ctx->hasAttribute('user')) {
        $user = $ctx->getAttribute('user');
      }
    }
    elseif ($req->isFront()) {
      $ctx->setAttribute('_layout', $req->getParameter('layout'));
      if (true) {
        $agent = $req->getClient('agent');
        if (preg_match('/bot|crawler|preview|http:/', $agent) === 1) { 
          $dbo->insert(Tracker::TABLE_ROBOT,
                       array('uri' => $req->getUri(),
                             'address' => $req->getClient('address'),
                             'robot' => $agent,
                             'app_id' => $req->getAppId()));
        }
        unset($agent);
      }
    }

    if (!$req->isCachable()) {
      $res->disableCache();
    }

    $vars = S('app')->hook('on'.ucfirst($req->getScope()).'Start');
    if (is_array($vars)) {
      extract($vars, EXTR_SKIP);
    }

    $ctx->setAttribute('_scope', $req->getScope());
    $ctx->setAttribute('_extension', $req->getExtension());
    $ctx->setAttribute('_action', $req->getAction());

    include $req->getController();

    if ($req->isAccount()) {
      if (!$req->isXhr(Request::XHR_EXEC)) {
        if ($req->isXhr() || $req->isPhp()) {
          $res->useTemplate();
        }
        else {
          AccountManager::init();
          $res->setHead($ctx->getAttribute('_title').' | '.ucfirst(P('app.name')));
          $template = P('account.template');
          if ($err->count() >= 1) {
            $res->useTemplates($template);
          }
          else {
            $res->useTemplates($template, true);
          }
        }
      }
      if (!$req->isPhp()) {
//        $req->track($req->getAction(), 'account');
      }
    }
    elseif ($req->isMiddle()) {
//      $req->track($req->getAction(), 'middle');
    }

  }

  public static function getInstance() {
    $class = __CLASS__;
    return new $class;
  }

}
