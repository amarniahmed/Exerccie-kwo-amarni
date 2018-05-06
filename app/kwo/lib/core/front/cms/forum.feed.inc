<?php

try {

  $res->setContentType('rss');

  $forum = Forum::getInstance($req->id);

  $res->write($forum->asFeed());

}
catch (Exception $e) { $err->add($e); }