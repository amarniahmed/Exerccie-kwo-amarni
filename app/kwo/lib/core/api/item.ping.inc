<?php

try {

  $source_uri = $req->sourceURI;
  $target_uri = $req->targetURI;

  $html = (string) @file_get_contents($source_uri);
  if (strlen($html) < 8) {
    throw new Exception('failure ['.$source_uri.']');
  }

  $doc = new DOMDocument();
  @$doc->loadHTML($html);
  $found = false;
  foreach ($doc->getElementsByTagName('A') as $node) {
    if ($node->getAttribute('href') === $target_uri) {
      $found = true;
      break ;
    }
  }

  if ($found === false) {
    throw new Exception('failure ['.$source_uri.'] : ['.$target_uri.']');
  }

  $source_name = '';
  $nodes = $doc->getElementsByTagName('TITLE');
  if ($nodes->length >= 1) {
    $source_name = $nodes->item(0)->nodeValue;
    if (strtoupper($doc->encoding) != 'UTF-8') {
      $source_name = utf8_decode($source_name);
    }
  }

  $query = parse_url($target_uri, PHP_URL_QUERY);
  if (strpos($query, 'id=') !== false) {
    $vars = array();
    parse_str($query, $vars);
    $args = (int) $vars['id'];
  }
  else {
    $routes = P('front.routes');
    foreach ($routes as $regex => $route) {  
      if ($route[0] != 'blog') continue ;
      $matches = array();
      if (preg_match('#^'.$regex.'#', $target_uri, $matches)) { 
        $args = (int) $matches['id'];
        break ;
      }
    }
  }

  $ctx->message = 'Thanks for the ping.';

  $item = Item::getInstanceByKey($args);

  $row = array('model_id' => $item->getModelId(),
               'record_id' => $item->getId(),
               'source_uri' => $source_uri);

  if ($dbo->getTable(Item::TABLE_PING)->hasRow($row)) return ;

  $row['source_name'] = $source_name;

  $dbo->insert(Item::TABLE_PING, $row);
  
  $req->debug();

  $req->track();

}
catch (Exception $e) { $err->add($e); }
