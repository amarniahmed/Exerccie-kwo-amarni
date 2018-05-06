<?php

try {

  $args = null;
  if ($req->hasAttribute('code', true)) {
    $args = array('code' => $req->code);
  }
  elseif ($req->hasAttribute('id', true)) {
    $args = (int) $req->id;
  }

  $page = Page::getInstance($args, array('properties' => true));
  if (!$page->isOnline()) throw new Exception(404);
  $page->track();

  $ctx->page = $page;
  $ctx->blocks = array();
  $opts = array('filters' => array('status' => 1,
                                   'core_block_page.status' => 1),
                'properties' => true,
                'order' => array('position' => 'ASC'));
  foreach ($page->getBlocks($opts) as $block) $ctx->blocks[$block->code] = $block;

  $req->track($page);
  $res->setHead($page);
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }