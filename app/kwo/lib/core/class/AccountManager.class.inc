<?php

class AccountManager
{
  public static function init() {
    $ctx = S('ctx');
    $user = S('ctx')->getAttribute('user');
    $res = S('res');
    $req = S('req');

    $menu = array();
    $selected_menu = null;
    $actions = self::actions();

    foreach ($actions as $code => &$hash) {
//      if ($code == $ctx->_menu[0]) {
      // if ($hash['action'] == $req->getPath()) { FIXED
      if ($hash['action'] == $req->getAction()) {
        $menu[$hash['group']]['selected'] = true;
        $hash['selected'] = true;
        $selected_menu = $hash['group'];
      }
      $menu[$hash['group']]['actions'][] = $hash;
    }
    foreach ($menu as $group => &$hash) {
      if (count($hash['actions']) == 1) {
        $hash['label'] = $hash['actions'][0]['label'];
      }
      else {
        $hash['label'] = $hash['actions'][0]['group_label'];
      }
      $hash['action'] = $hash['actions'][0]['action'];
    }
    if (!$ctx->hasAttribute('_tip')) {
      try {
        $snippet = Snippet::getInstance(array('code' => 'account.tips.'.$req->getAction()),
                                        array('ignore' => true));
        $tip['title'] = $snippet->getProperty('title');
        $tip['content'] = $snippet->getProperty('content');
        $ctx->setAttribute('_tip', $tip);
      }
      catch (Exception $e) { }
    }

    if ($user->status == User::STATUS_PENDING) {
      $msg = Snippet::content('warning.status.pending', 'content',
                              array('email' => $user->email,
                                    'url' => 'javascript:void(0)" onclick="Kwo.User.onEmailConfirm(this); return false;'));
      $res->setWarning($msg);
    }
    $ctx->setAttribute('_menu', $menu);
    $ctx->setAttribute('_selected_menu', $selected_menu);
  }

  public static function countEvaluations($user) {
    $orders = $user->getOrders();
    $num = 0;
    foreach ($orders as $order) {
      if ($order->status >= Order::STATUS_PROCESSED &&
          !$order->hasFlag(Order::FLAG_ITEM_RETURNED) &&
          !Valuation::isEvaluated($order, $order->user_id)) {
        $num++;
      }
    }
    return $num;
  }

  public static function actions() {
    $modules = array();
    $account_modules = (array) P('account.modules');
    foreach ($account_modules as $module) {
      if (!isset($module['group'])) {
        $module['group'] = $module['label'];
      }
      $module['group_label'] = $module['group'] ? l($module['group']) : null;
      $module['label'] = $module['label'] ? l($module['label']) : null;
      $modules[$module['code']] = $module;
    }
    //        uasort($modules, array(self, 'sort'));
    S('app')->hook('onAccountMenuBuild', array('modules' => &$modules));
    return $modules;

/*    $actions = array();
    foreach (Extension::all() as $extension) {
      if (!is_array($extension['account'])) continue;
      foreach ($extension['account'] as $action) {
        if (!isset($action['group'])) {
          $action['group'] = $action['label'];
        }
        $action['group_label'] = l($action['group']);
        $action['label'] = l($action['label']);
        $actions[$action['code']] = $action;
      }
    }
    uasort($actions, array(self, 'sort'));
    S('app')->hook('onAccountMenuBuild', array('actions' => &$actions));
    return $actions;*/
  }

/*  public static function sort($a, $b){
    return $a['position'] < $b['position'] ? -1 : 1;
  } */

}
