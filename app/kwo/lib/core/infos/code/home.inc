<?php

function cmp($a, $b)
{
  return strcasecmp($a['label'], $b['label']);
}

try {
  
  $ctx->app_title = P('app.name');

  $ctx->admin = $admin;
  
  //  $ctx->files_flag = $admin->hasRight(EXTENSION_FILE);
  $ctx->files_flag = true;
  
  $ctx->iframe_main_width  = UiManager::WIDTH.'px';
  $ctx->iframe_main_height = (UiManager::HEIGHT + 2).'px';
  
  $ctx->extensions = array();
  $ctx->menus = array();
  foreach (Extension::all() as $name => $hash) {
    if (!$admin->hasRight($hash['id'])) {
      continue ;
    }
    if (!is_array($hash['back']) || count($hash['back']) === 0) continue ;
    $ctx->extensions[$name]['label'] = $hash['label'];
    if (is_array($hash['back'])) {
      if (count($hash['back']) === 1) {
        $ctx->extensions[$name]['onclick'] = "goTo('".$name."', '".current($hash['back'])."')";
      } 
      else {
        $ctx->menus[$name] = array();
        foreach ($hash['back'] as $n => $link) {
          if (is_numeric($n)) {
            $n = Model::label($link);
            $link = $link.'.home';
          }
          $ctx->menus[$name][$n] = "goTo('".$name."', '".$link."')";
        }
      }
    }
  }
  
  uasort($ctx->extensions, 'cmp');
  
  $res->setSeo($ctx->app_title);
  $res->useTemplate();
  
}
catch (Exception $e) {}