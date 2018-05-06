<?php

try {

  $args = array('id' => $req->id,
                'is_public' => 1);

  $tree = Tree::getInstance($args);

  $res->setHead($tree);

  $res->write(Tree::cross($tree));

}
catch (Exception $e) { $err->add($e); }