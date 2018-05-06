<?php

class BackManager {

  public static function getMenus($admin=null) {
    $menus = array();

    foreach (Extension::all() as $name => $h) {
      if (!is_array($h['back']) || count($h['back']) === 0) continue ;
      if (!isset($menus[$name])) {
        $menus[$name] = array('label' => $h['label'] ?: $name,
                              'entries' => array());
      }
      foreach ($h['back'] as $label => $link) {
        if (is_array($link)) {
          if (!isset($menus[$label])) {
            $menus[$label] = array('label' => l($label),
                                   'entries' => array());
          }
          foreach ($link as $k => $v) {
            if ($v != '--' && strpos($v, '/') === false) {
              $v = '/back/'.$name.'/'.$v;
            }
            if ($admin && $v != '--' && !$admin->hasRight($v)) continue ;
            $key = is_numeric($k) ? $k : l($k);
            $menus[$label]['entries'][$key] = $v;
          }
        }
        else {
          if ($link != '--' && strpos($link, '/') === false) {
            $link = '/back/'.$name.'/'.$link;
          }
          if ($admin && $link != '--' && !$admin->hasRight($link)) continue ;
          $key = $link == '--' ? $label : l($label);
          $menus[$name]['entries'][$key] = $link;
        }
      }
    }

    function labelcmp($a, $b) {
      return strcasecmp($a['label'], $b['label']);
    }

    uasort($menus, 'labelcmp');

    $out = array();
    foreach ($menus as $name => $h) {
      if (empty($h['entries'])) continue ;
      $n = 0;
      foreach ($h['entries'] as $k => $v) {
        if ($v == '--') $n++;
      }
      if (count($h['entries']) == $n) continue ;
      $out[$name] = $h;
    }
    return $out;
  }

  public static function getLocale() {
    $locale = I18N::LOCALE_FR;
    if (Platform::hasSingleton('admin') === true && S('admin')->locale > 0) {
      $locale = S('admin')->locale;
    }
    return $locale;
  }

  public static function getBatchCollection() {
    $req = S('req');
    if ($req->hasAttribute('finder', true)) {
      $finder = $req->getAttribute('finder');
      $finder = Finder::getInstance($finder);
      $items = $finder->getEntireCollection();
    }
    elseif ($req->hasAttribute('items', true)) {
      $items = explode(',', $req->getAttribute('items'));
      $filters = array('id' => $items);
      $opts = array('filters' => $filters);
      $model = $req->getAttribute('model');
      $items = Item::collect($model, $opts);
    }
    else {
      S('log')->error('invalid input', __METHOD__);
      throw new Exception('invalid input');
    }
    return $items;
  }

}