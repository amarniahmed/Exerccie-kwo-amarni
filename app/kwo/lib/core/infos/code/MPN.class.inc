<?php

class MPN
{
  const S7_URL = 'http://usineadesign.scene7.com/ir/render/usineadesignrender/';
  const S7_360_URL = 'http://s7g4.scene7.com/is-viewers/flash/genericzoom.swf?contentRoot=http://s7g4.scene7.com/skins/&serverURL=http://s7g4.scene7.com/is/image&config=usineadesign/uadzoom&image=usineadesignrender%3Flayer%3D0%26src%3Dir';

  public static $flat = array();
  public static $attributes = array();
  public static $trees = null;

  public static function path2mpn($path, $produit=null) {
    list ($path, $produit) = self::getProduit($path, $produit);
    $tree = self::getTree($produit);
    self::flattenTree($tree, array('value' => 'mpn'));
    $branches = explode('|', $path);
    $mpn_parts = array();
    foreach ($branches as $branch) {
      $n = (strlen($branch) / 2);
      $chunks = array();
      for ($i = 1; $i <= $n; $i++) {
        $path = substr($branch, 0, ($i * 2));
        $chunks[] = self::$flat[$path];
      }
      $mpn_parts[] = join('-', $chunks);
    }
    $mpn = join('_', $mpn_parts);
    return $mpn;
  }

  public static function mpn2path($mpn, $produit=null) {
    list ($mpn, $produit) = self::getProduit($mpn, $produit);
    $tree = self::getTree($produit);
    self::flattenTree($tree, array('value' => 'mpn'));
    $parts = explode('_', $mpn);
    $branches = array();
    foreach ($parts as $part) {
      $path = '';
      $blocs = explode('-', $part);
      foreach ($blocs as $bloc) {
        foreach (self::$flat as $key => $value) {
          if ($value == $bloc /*&& $value != '0'*/ && (empty($path) || strpos($key, $path) === 0)) {
            $path = $key;
            continue ;
          }
        }
      }
      $branches[] = $path;
    }
    return join('|', $branches);
  }

  public static function path2price($path, $produit=null) {
    $path = self::toPath($path, $produit);
    list ($path, $produit) = self::getProduit($path, $produit);
    $tree = self::getTree($produit);
    self::flattenTree($tree, array('value' => 'price'));
    $branches = explode('|', $path);
    $price = 0;
    foreach ($branches as $branch) {
      $n = (strlen($branch) / 2) - 1;
      $chunks = array();
      for ($i = 1; $i <= $n; $i++) {
        $path = substr($branch, 0, (2 + $i * 2));
        $price += self::$flat[$path];
      }
    }
    return $produit->price + $price;
  }

  public static function path2img($path, $produit=null, $zoom=false) {
    $path = self::toPath($path, $produit);
    list ($path, $produit) = self::getProduit($path, $produit);
    $tree = self::getTree($produit);
    self::flattenTree($tree, array('value' => 'img'));
    $produit_id = $produit->getId();
    $branches = explode('|', $path);
    $chunks = array();
    foreach ($branches as $branch) {
      $n = (strlen($branch) / 2);
      $forms = $textures = $chunk = '';
      $chunk = 'obj='.$produit_id.'_';
      for ($i = 1; $i <= $n; $i++) {
        $path = substr($branch, 0, ($i * 2));
        $value = self::$flat[$path];
        if (empty($value)) continue ;
        list (, $option) = explode('=', $value);
        if ($i == 1) {
          $chunk .= $option;
          continue ;
        }
        if ($value[0] == 't') {
          $textures .= '_'.$option;
        }
        else {
          $forms .= '_'.$options;
        }
      }
      if (empty($forms) && empty($textures)) {
        continue ;
      }
      if (!empty($forms)) {
        $chunk .= $forms;
      }
      if (!empty($textures)) {
        $chunk .= '&src='.ltrim($textures, '_');
      }
      $chunk .= '&res=resolution&show';
      $chunks[] = $chunk;
    }
    //    $mpn_parts[] = join('&', $chunks);
    $url = ($zoom === false) ? self::S7_URL : '';
    $url .= $produit_id.'?';
    $url .= ($zoom === false) ? 'wid=width&' : '';
    $url .= 'qlt=quality&';
    $chunks = join('&', $chunks);
    $url .= $chunks;
    return $url;
  }

  public static function path2txt($path, $produit=null) {
    $path = self::toPath($path, $produit);
    list ($path, $produit) = self::getProduit($path, $produit);
    $tree = self::getTree($produit);
    self::flattenTree($tree, array('value' => 'txt'));
    $branches = explode('|', $path);
    $mpn_parts = array();
    foreach ($branches as $branch) {
      $n = strlen($branch) / 2;
      $chunks = array();
      for ($i = 1; $i <= $n; $i++) {
        $path = substr($branch, 0, ($i * 2));
        $chunks[] = self::$flat[$path];
      }

      $mpn_parts[] = join(' - ', $chunks);
    }
    return join(' | ', $mpn_parts);
  }


  public static function toPath($path, $produit) {
    //if (strpos($path, '|') !== false) return $path;
    if (strpos($path, '.') === false) return $path;

    $out = self::mpn2path($path, $produit);
    if (strpos($path, ':') !== false) {
      list ($produit_id, ) = explode(':', $path);
      $out = $produit_id.':'.$out;
    }
    return $out;
  }

  public static function getProduit($path, $produit=null) {
     if (strpos($path, ':') !== false) {
      list ($produit_id, $path) = explode(':', $path);
     }
     else {
       $produit_id = $produit;
     }
     $prod = $produit instanceof Produit ? $produit : Produit::getInstance($produit_id);
     return array($path, $prod);
   }

  public static function getTree($produit) {
     if (self::$trees[$produit->getId()] === null) {
       self::$trees[$produit->getId()] = $produit->asTree();
     }
     return self::$trees[$produit->getId()];
   }


  public static function isPath($str) {
    return strpos($str, '|') !== false;
  }

  public static function flattenTree($tree, $opts) {
    self::$flat = null;
    self::$attributes[1] = Attribut::getInstance('attribut', 1)->getAttributes();
    self::cross($tree, $opts);
  }

  public static function cross($level, $opts) {
    foreach ($level as $node) {
      $children = $node['children'];
      $node = $node['node'];
      if ($node['children_attribut_id'] >= 1 && empty(self::$attributes[$node['children_attribut_id']])) {
        self::$attributes[$node['children_attribut_id']] = $node['children_attribut'];
      }
      if (strlen($node['path']) > 1) {
        $option = $node['option'];
        $attribut = self::$attributes[$node['option']['attribut_id']];
        if ($opts['value'] === 'mpn') {
          $value = (string)$attribut['code'].'.'.(string)$node['option']['code'];
        }
        elseif ($opts['value'] === 'img') {
          if ($attribut['type'] >= 1 || $attribut['id'] == 1) {
            $value = $attribut['type'] == 1 ? 'f' : 't';
            $value .= ':'.$attribut['code'].'='.$node['option']['code'];
          }
          else {
            $value = '';
          }
        }
        elseif ($opts['value'] === 'price') {
          //$value = $node['price'] / 1.196;
          $value = $node['price'] / 1.2;
        }
        elseif ($opts['value'] === 'txt') {
          $value = $node['option']['name'];
        }
        self::$flat[$node['path']] = $value;
      }
      if (is_array($children)) {
        self::cross($children, $opts);
      }
    }
  }


}
