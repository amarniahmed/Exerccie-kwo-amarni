<?php

try {
  
  $ctx->admin = $admin;
  $ctx->menus = BackManager::getMenus($admin);
  $ctx->iframe_main_height = UiManager::HEIGHT.'px';
  $ctx->iframe_main_width  = UiManager::WIDTH.'px';

  $res->setHead(P('app.name'));
  $res->useTemplate();
  
}
catch (Exception $e) {}
