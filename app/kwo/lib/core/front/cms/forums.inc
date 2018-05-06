<?php

try { 

  $opts = array('include' => 'column',
                'order' => array('column_position', 
                                 'position', 
                                 'name'));
  $columns = array();
  $topic_ids = array();
  foreach (Forum::collect($opts) as $forum) {
    $columns[$forum->column_name][] = $forum;
    if ($forum->last_topic_id >= 1) {
      $topic_ids[] = $forum->last_topic_id;
    }
  }
  
  $ctx->topics = array();
  if (count($topic_ids) >= 1) {
    $ctx->topics = Topic::collect(
                                  array('filters' => array('id' => $topic_ids),
                                        'include' => 'user'))->asHash();
  }
  
  $ctx->columns = $columns;

  $res->setHead('forums');
  $res->setBreadcrumb(array('Forum' => '/board/forums'));
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
