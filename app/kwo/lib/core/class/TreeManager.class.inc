<?php

class TreeManager
{
  const HEIGHT_MAX = 6;
  
  public static function onNodeCreate($node, &$hash) { 
    $ctx = S('ctx'); 
    $tree_key = self::treeModel($node).'_id';
    $tree_id = $hash[$tree_key] >= 1
             ? $hash[$tree_key]
             : $ctx->getBranchAttribute($tree_key);
    if (isset($hash['parent_path'])) {
      $path = $hash['parent_path'];
      unset($hash['parent_path']);
    }
    else {
      $path = $ctx->getBranchAttribute('parent_path');
    }
    $hash = array_merge($hash, TreeManager::nextChild($node, $tree_id, $path));
  }

  public static function columns($tree, $node_id=null) {
    $path = '';
    $node_model = self::nodeModel($tree); 
    if ($node_id >= 1) {
      $node = Item::getInstance($node_model, $node_id); 
      $path = $node->path;
    }
    $n = strlen($path) / 2;
    $pathes = array();
    for ($i = 0; $i <= $n; $i++) {
      $pathes[] = substr($path, 0, ($i * 2)).'__';
    }
    $where = '';
    foreach ($pathes as $regex) {
      $where .= 'OR path LIKE "'.$regex.'" ';
    }
    $where = ' AND ('.trim(ltrim($where, 'OR ')).')';
    $rs = S('dbo')->query('SELECT * FROM '.T($node_model)
                          .' WHERE '.$tree->_foreign_key.'='.$tree->getId().$where
                          .' ORDER BY position');
    $columns = array();
    foreach ($rs as $row) {
      $column_num = strlen($row['path']) / 2;
      $classes = array();
      if (!empty($path) && strpos($path, $row['path']) === 0) {
        $classes[] = 'parent';
      }
      if ($node_id == $row['id']) {
        $classes[] = 'selected';
        $columns[$column_num]['has_selection'] = true;
      }
      $row['classes'] = join(' ', $classes);
      $columns[$column_num]['path'] = substr($row['path'], 0, -2);
      $columns[$column_num]['nodes'][] = $row;
    }
    $height_max = $tree->height_max >= 1 ? $tree->height_max : self::HEIGHT_MAX;
    if (($columns[count($columns)]['has_selection'] === true || count($columns) === 0) &&
        (count($columns) < $height_max)) {
      $columns[count($columns) + 1] = array('path' => $path,
                                            'nodes' => array());
    }
    ksort($columns);
    //S('log')->debug($columns);
    return $columns;
  }

  public static function children($node, $opts=null) {
    $opts = $opts === null
          ? array('filters' => array('ancestor_id' => $node->isRoot() ? 0 : $node->getId()),
                  'order' => array('position'))
          : (array) $opts;
    if ($opts['recursive'] === true) {
      if (!$node->isRoot()) {
        $opts['filters']['|><|path'] = array($node->path, $node->path.'zz');
      }
      unset($opts['filters']['ancestor_id']);
    }
    else {
      $opts['filters']['ancestor_id'] = $node->getId();
    }
    if ($opts['recursive'] !== true && !isset($opts['order'])) {
      $opts['order'] = array('position');
    }
    if ($opts['last'] >= 1) {
      $opts['order'] = array('position' => 'DESC');
    }
    return Item::collect($node->getModel(), $opts);
  }

  public static function nextChild($node, $tree_id, $path) {
    if ($tree_id < 1) {
      throw new Exception('unknown tree ('.__METHOD__.')');
    }
    $ancestor_id = 0;
    $tree_model = self::treeModel($node);
    $tree_key = $tree_model.'_id';
    if (!empty($path)) {
      $ancestor_id = S('dbo')->fetchValue('SELECT id'
                                        .' FROM '.T($node)
                                        .' WHERE path=:path:'
                                        .  ' AND '.$tree_key.'='.$tree_id,
                                        array(':path:' => $path));
    }
    $max = S('dbo')->fetchHash('SELECT MAX(path) AS path_max,'
                               .     ' MAX(position) AS position_max'
                               .' FROM '.T($node)
                               .' WHERE ancestor_id='.$ancestor_id
                               .  ' AND '.$tree_key.'='.$tree_id);
    $hash['path'] = strlen($max['path_max']) >= 2 ? self::nextKey($max['path_max']) : $path . self::nextKey();
    $hash['position'] = $max['position_max'] >= 1 ? $max['position_max'] + 1 : 1;
    $hash['ancestor_id'] = $ancestor_id;
    $hash[$tree_key] = $tree_id;
    if (Model::hasField($tree_model, 'model_id')) {
      $tree = Item::getInstance($tree_model, $tree_id);
      $hash['model_id'] = (int) $tree->model_id;
    }
    return $hash;
  }


  public static function walk($tree, $opts=null) {
    $opts = (array) $opts; 
    $node_model = self::nodeModel($tree); 
    $node_table = T($node_model);
    $pointers = $nodes = array();
    $rs = S('dbo')->query('SELECT LEFT(path, LENGTH(path) - 2) AS parent_path, n.*'
                          .' FROM '.$node_table.' AS n'
                          .' WHERE '.$tree->_foreign_key.'='.$tree->getId()
                          .' ORDER BY parent_path, position');
    foreach ($rs as $node) {
      $n = array('node' => $node,
                 'children' => null);
      if ($node['model_id'] >= 1 && $node['record_id'] >= 1) {
        try {
          $item = Item::getInstance((int) $node['model_id'], 
                                    (int) $node['record_id']);
          $n['item'] = $item->getAttributes();
        }
        catch (Exception $e) {
          S('log')->error($e, __METHOD__);
        }
      }
      if ($node['ancestor_id'] < 1) {
        $nodes[$node['path']] = $n;
        $pointers[$node['path']] = &$nodes[$node['path']];
        continue ;
      }
      $path = substr($node['path'], 0, -2);
      $pointers[$path]['children'][$node['path']] = $n;
      $pointers[$node['path']] = &$pointers[$path]['children'][$node['path']];
    }
    //S('log')->debug($nodes);
    //print_r($nodes);
    if ($opts['flat'] == true) {
      $out = array();
      self::flatten($nodes, $out);
      $nodes = $out;
    }
    return $nodes;
  }

  public static function flatten($children, &$nodes) {
    if (empty($children)) return ;
    foreach ($children as $path => $child) {
      $nodes[$path] = $child['node'];
      self::flatten($child['children'], $nodes);
    }
  }

  public static function asHtml($tree, $opts=null) {
    $nodes = self::walk($tree);
    $html = self::html($nodes, 0);
    return $html."\n";
  }

  public static function html($children, $level) {
    if (empty($children)) return '';
    $s = str_repeat('  ', $level);
    $level++;
    $html = $s.'<div class="branch branch-level-'.$level.'">'."\n";
    $position = 1;
    foreach ($children as $child) {
      $num = self::numbering($level, $position);
      $html .= $s.' <div class="node node-level-'.$level.'">'."\n";
      $html .= $s.'  <strong><span>'.$num.'</span>'.$child['node']['title'].'</strong>'."\n";
      $html .= $s.'  <p>'.$child['node']['content'].'</p>'."\n";
      $html .= self::html($child['children'], $level);
      $html .= $s.' </div>'."\n";
      $position++;
    }
    $html .= $s.'</div>';
    return $html."\n";
  }

  public static function asOutline($tree, $opts=null) {
    $nodes = self::walk($tree);
    $html = self::outline($nodes, 0);
    return $html."\n";
  }

  public static function outline($children, $level) {
    if (empty($children)) return '';
    $s = str_repeat('  ', $level);
    $level++;
    $html = $s.'<div class="branch branch-level-'.$level.'">'."\n";
    $position = 1;
    foreach ($children as $child) {
      $num = self::numbering($level, $position);
      $html .= $s.' <div class="node node-level-'.$level.'">'."\n";
      $html .= $s.'  <a href="javascript:void(0)"><span>'.$num.'</span>'.$child['node']['title'].'</a>'."\n";
      $html .= self::outline($child['children'], $level);
      $html .= $s.' </div>'."\n";
      $position++;
    }
    $html .= $s.'</div>';
    return $html."\n";
  }

  public static function numbering($level, $position) {
    if ($level == 4) {
      return chr(97 + $position - 1);
    }
    if ($level == 3) {
      return $position;
    }
    if ($level == 2) {
      return chr(65 + $position - 1);
    }
    if ($level == 1) {
      return NH::asRoman($position);
    }
  }

  public static function appendChild($node, $attributes) {
    $child = Item::getInstance($node->getModel());
    $attributes['tree_id'] = $node->getAttribute(self::treeModel($node).'_id');
    $attributes['parent_path'] = $node->path;
    $child->commit($attributes);
    return $child;
  }

  public static function isNode($model) {
    $relations = Model::parameter($model, 'belongs_to');
    if (empty($relations)) return false;
    foreach ($relations as $key => $hash) {
      if ($hash['structure'] === 'tree') return true;
    }
    return false;
  }

  public static function isTree($model) {
    $relations = Model::parameter($model, 'has_many');
    if (empty($relations)) return false;
    foreach ($relations as $key => $hash) {
      if ($hash['structure'] === 'tree') return true;
    }
    return false;
  }

  public static function nodeModel($tree) {
    $relations = Model::parameter($tree, 'has_many'); 
    if (empty($relations)) return null;
    foreach ($relations as $key => $hash) { 
      if ($hash['structure'] === 'tree') return $key;
    } 
    return null;
  }

  public static function treeModel($node) {
    $relations = Model::parameter($node, 'belongs_to');
    if (empty($relations)) return null;
    foreach ($relations as $key => $hash) {
      if ($hash['structure'] === 'tree') return $key;
    }
    return null;
  }

  private static function nextKey($key='') {
    if (empty($key)) return 'aa';
    $n = strlen($key) - 1;
    if ($key[$n] === 'z') {
      $key[$n] = 'a';
      $key[$n - 1] = chr(ord($key[$n - 1]) + 1);
    }
    else {
      $key[$n] = chr(ord($key[$n]) + 1);
    }
    return $key;
  }

  /*  public static function getParent($node) {
    if ($node->ancestor_id >= 1) {
      return Node::getInstance(__CLASS__, $node->ancestor_id);
    }
    return null;
    }*/


}