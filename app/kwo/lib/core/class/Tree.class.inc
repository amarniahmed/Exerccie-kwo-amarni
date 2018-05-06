<?php

class Tree extends ActiveRecord {

  const FLAG_PUBLIC = 1;
  const FLAG_AUTOBUILD = 2;
  const FLAG_TRANSLATE = 4;

  public static $flags = array(self::FLAG_PUBLIC => 'public',
                               self::FLAG_AUTOBUILD => 'contruction auto',
                               self::FLAG_TRANSLATE => 'noeuds localisés');

  public function rootNodes($opts=null) {
    $opts = (array) $opts;
    $opts['filters']['tree_id'] = $this->getId();
    $opts['filters']['ancestor_id'] = 0;
    return Node::collect($opts);
  }

  public function appendChild($attrs) {
    $root = Node::getInstance();
    $attrs['tree_id'] = $this->getId();
    $attrs['parent_path'] = '';
    $root->commit($attrs);
    return $root;
  }

  public function asColumnHash($ancestor_id=0) {
    $rs = $this->dbo->query('SELECT id, name'
                            .' FROM '.T('node')
                            .' WHERE tree_id='.$this->getId()
                            .  ' AND ancestor_id='.$ancestor_id
                            .' ORDER BY position');
    return $rs->asHash();
  }

  public function build() {
    if (strlen($this->build_method) > 6) {
      list ($class, $method) = explode('::', $this->build_method);
      $class = trim($class);
      $method = trim($method, ' ()');
      $tree = call_user_func(array($class, $method), $this);
    }
    return $tree;
  }

  public function asOptions($value) {
    $tree = self::onBuild($this);
    $html = '';
    foreach ($tree as $parent) {
      if (!$parent['children']) continue ;
      $html .= '<optgroup label="'.$parent['node']['name'].'">'."\n";
      foreach ($parent['children'] as $child) {
        $selected = $value == $child['node']['id'] ? 'selected="selected"' : '';
        $html .= ' <option value="'.$child['node']['id'].'"'.$selected.'>'.$child['node']['name'].'</option>'."\n";
      }
      $html .= '</optgroup>';
    }
    return $html;
  }

  public static function onBuild($tree) {
    $pointers = $struct = array();
    $rs = S('dbo')->query('SELECT LEFT(path, LENGTH(path) - 2) AS P,'
                          .   ' '.T('node').'.*'
                          .' FROM '.T('node')
                          .' WHERE tree_id='.$tree->getId()
                          .' ORDER BY P, position');
    foreach ($rs as $node) {
      $n = array('node' => $node,
                 'item' => null,
                 'children' => null);
      if ($node['model_id'] >= 1 && $node['record_id'] >= 1) {
        try {
          $n['item'] = Item::getInstance($node['model_id'],
                                         $node['record_id'])->getAttributes();
        }
        catch (Exception $e) {
          S('log')->error($e, __METHOD__);
        }
      }
      if ($node['ancestor_id'] < 1) {
        $struct[$node['path']] = $n;
        $pointers[$node['path']] = &$struct[$node['path']];
        continue ;
      }
      $path = substr($node['path'], 0, -2);
      $pointers[$path]['children'][$node['path']] = $n;
      $pointers[$node['path']] = &$pointers[$path]['children'][$node['path']];
    }
    //S('log')->debug($struct);
    //print_r($struct);
    return $struct;
  }

  public static $out = '';

  public static function crossRecursive($level, $opts) {
    foreach ($level as $node) {
      self::$out .= Template::render($opts['template'], $node);
      if (is_array($node['children'])) {
        self::crossRecursive($node['children'], $opts);
      }
    }
  }

  public static function cross($tree, $opts=null) {
    $opts = (array) $opts;
    $opts['template'] = $opts['template'] ? $opts['template'] : 'tree.node';
    self::$out = '<div class="kwo-tree">'."\n";
    self::crossRecursive(self::onBuild($tree), $opts);
    self::$out .= '</div>'."\n";
    return self::$out;
  }

}