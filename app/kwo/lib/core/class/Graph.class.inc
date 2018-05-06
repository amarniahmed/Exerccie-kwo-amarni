<?php

/***

/opt/neo4j/bin/neo4j start

http://pulsar.inerd:7474/webadmin/
http://pulsar.inerd:7474/db/data/application.wadl

http://localhost:7474/db/data/node/123/properties/foo

***/

/***

/etc/rc.d/rc.local

export JAVA_HOME="/opt/jre"
/opt/neo4j/bin/neo4j start


***/

class Graph
{
  const EXCEPTION_NODE_NOT_FOUND = 1;
  const EXCEPTION_INVALID_DATA = 2;

  const DEFAULT_CONNEXION_TYPE = 'KNOWS';
  const NODE_INDEX = 'nindex';

  const MAX_DEPTH = 3;

  /*  const CONNEXION_INDEX = 'cindex';*/

  private $server = 'http://localhost:7474/db/data';

  private $trace = 0;

  private $last_call = null;

  public function __construct() {
    
  }
  
  public function insert($item) {
    // force la création d'un s'il n'existe pas
    // + verification que le meta node_id correspond bien au bon item
    
  }

  public function hasConnexion($node1, $node2, $type=self::DEFAULT_CONNEXION_TYPE) {
    return (bool) $this->getRelationshipId($node1, $node2, $type=self::DEFAULT_CONNEXION_TYPE);
  }

  public function getRelationshipId($node1, $node2, $type=self::DEFAULT_CONNEXION_TYPE) {
    $id1 = $this->asNodeId($node1, __METHOD__);
    $id2 = $this->asNodeId($node2, __METHOD__);
    $request = array('path' => '/node/'.$id1.'/relationships/all/'.$type);
    $response = $this->call($request);
    $connexion = null;
    foreach ($response as $relationship) {
      $parts = explode('/', $relationship['start']);
      $start_id = array_pop($parts);
      if ($start_id == $id2) {
        $connexion = $relationship['self'];
        break ;
      }
      $parts = explode('/', $relationship['end']);
      $end_id = array_pop($parts);
      if ($end_id == $id2) {
        $connexion = $relationship['self'];
        break ;
      }
    }
    if (empty($connexion)) {
      return 0;
    }
    $parts = explode('/', $connexion);
    return array_pop($parts);
  }

  public function getDistance($node1, $node2, $opts=null) {
    $path = $this->getPath($node1, $node2, $opts);
    if (empty($path)) return 0;
    return count($path) - 1;
  }

  public function getPath($node1, $node2, $opts=null) {
    $paths = $this->getPaths($node1, $node2, $opts);
    if (empty($paths)) return array();
    return $paths[0];
  }

  public function getPaths($node1, $node2, $opts=null) {
    $type = $opts['type'] ?: self::DEFAULT_CONNEXION_TYPE;
    $max_depth = $opts['max_depth'] ?: self::MAX_DEPTH;
    $count = $opts['count'] ?: self::MAX_DEPTH;
    $id1 = $this->asNodeId($node1, __METHOD__);
    $id2 = $this->asNodeId($node2, __METHOD__);
    $request = array('path' => '/node/'.$id1.'/paths',
                     'method' => 'POST',
                     'datas' => array('to' => $this->server.'/node/'.$id2,
                                      'algorithm' => 'shortestPath',
                                      'max depth' => $max_depth,
                                      'relationships' => array('type' => $type,
                                                               'direction' => 'all')));
    $response = $this->call($request);
    $paths = array();
    $i = 1;
    $nodes = array();
    foreach ($response as $path) {
      $paths[$i -1] = array();
      foreach ($path['nodes'] as $node) {
        $parts = explode('/', $node);
        $id = array_pop($parts);
        $paths[$i -1][] = $id;
        $nodes[] = $id;
      }
      if ($i >= $count) break ;
      $i++;
    }
    $items = self::nodes2items($nodes);
    foreach ($paths as &$path) {
      foreach ($path as &$node) {
        $node = $items[$node];
      }
    }
    //    S('log')->debug($paths);
    return $paths;
  }

  public function getRelationship($id) {
    $request = array('path' => '/relationship/'.$id);
    $response = $this->call($request);
    return $response;
  }

  public function getConnexions($node, $opts=null) {
    $opts = (array) $opts;
    $id = $this->asNodeId($node, __METHOD__);
    $type = $opts['type'] ?: self::DEFAULT_CONNEXION_TYPE;
    $request = array('path' => '/node/'.$id.'/relationships/all/'.$type);
    $response = $this->call($request);
    $out = array();
    foreach ($response as $relationship) {
      $parts = explode('/', $relationship['start']);
      $start_id = array_pop($parts);
      $parts = explode('/', $relationship['end']);
      $end_id = array_pop($parts);
      $out[] = $id == $start_id ? $end_id : $start_id;
    }
    return $out;
  }

  public function addConnexion($from, $to, $opts=null) {
    if ($this->hasConnexion($from, $to)) return ;
    $opts = (array) $opts;
    $from_id = $this->asNodeId($from);
    $to_id = $this->asNodeId($to);
    $request = array('path' => '/node/'.$from_id.'/relationships',
                     'method' => 'POST');
    $key = $from_id < $to_id ? $from_id.'-'.$to_id : $to_id.'-'.$from_id;
    $datas = array('to' => $this->server . '/node/'.$to_id,
                   'type' => self::DEFAULT_CONNEXION_TYPE);
    foreach ($opts as $key => $value) {
      $datas[$key] = $value;
    }
    $request['datas'] = $datas;
    $response = $this->call($request);
    // INDEXATION ?
  }
  
  public function removeConnexion($id, $type=self::DEFAULT_CONNEXION_TYPE) {
    if (is_array($id)) {
      $id = $this->getRelationshipId($id[0], $id[1], $type);
    }
    if ($id < 1) return ;
    $request = array('path' => '/relationship/'.$id,
                     'method' => 'DELETE');
    $response = $this->call($request);
  }

  public function addNode($attributes=null) {
    $request = array('path' => '/node',
                     'method' => 'POST');
    $item = null;
    if (!empty($attributes)) {
      if ($attributes instanceof ActiveRecord) {
        $item = $attributes;
        $attributes = array('item' => $item->getModelId().'-'.$item->getId(),
                            'model_id' => $item->getModelId(),
                            'record_id' => $item->getId(),
                            'model' => $item->getModel(),
                            'name' => $item->getName());
      }
      $request['datas'] = $attributes;
    }
    $response = $this->call($request);
    $parts = explode('/', $response['self']);
    $id = array_pop($parts); 
    if ($id < 1) {
      $this->debug();
    }
    if ($item) {
      $item->setMeta(Meta::FIELD_NODE, $id);
      try {
        $this->indexNode($item, 'item', $item->getModelId().'-'.$item->getId());
        $this->indexNode($item, 'model', $item->getModelId());
      }
      catch (Exception $e) {}
    }
    return $response;
  }
  
  public function findNode($key, $value=null) {
    if ($key instanceof ActiveRecord) {
      $value = $key->getModelId().'-'.$key->getId();
      $key = 'item';
    }
    $key = urlencode($key);
    $value = urlencode($value);
    $request = array('path' => '/index/node/'.self::NODE_INDEX.'/'.$key.'/'.$value);
    $response = $this->call($request);
    if (empty($response)) return 0;
    $parts = explode('/', $response[0]['self']);
    $id = array_pop($parts);
    return $id;
  }

  public function indexNode($node, $key=null, $value=null) {
    $id = $this->asNodeId($node, __METHOD__);
    if ($node instanceof ActiveRecord && $key === null) {
      $this->indexNode($id, 'item', $node->getModelId().'-'.$node->getId());
      $this->indexNode($id, 'model', $node->getModelId());
      return ;
    }
    $key = urlencode($key);
    $value = urlencode($value);
    $request = array('path' => '/index/node/'.self::NODE_INDEX.'/'.$key.'/'.$value,
                     'method' => 'POST',
                     'datas' => $this->server.'/node/'.$id);
    $this->call($request);
  }

  public function getNode($node) {
    $id = $this->asNodeId($node, __METHOD__);
    $request = array('path' => '/node/'.$id);
    $response = $this->call($request);
    return $response;
  }
  
  public function getNodeAttributes($node) {
    $id = $this->asNodeId($node, __METHOD__);
    $request = array('path' => '/node/'.$id.'/properties');
    $response = $this->call($request);
    return $response;
  }

  public function getNodeAttribute($node, $key) {
    $id = $this->asNodeId($node, __METHOD__);
    $request = array('path' => $base.'/node/'.$id.'/properties/'.$key);
    return $this->call($request);
  }
  
  public function setNodeAttributes($node, $attributes) {
    if (empty($attributes)) return ;
    $id = $this->asNodeId($node, __METHOD__);
    $request = array('path' => '/node/'.$id.'/properties',
                     'method' => 'PUT');
    if (!empty($attributes)) {
      if ($attributes instanceof ActiveRecord) {
        $item = $attributes;
        $attributes = $attributes->asGraphNodeHash();
      }
      $request['datas'] = $attributes;
    }
    $this->call($request);
  }

  public function setNodeAttribute($node, $key, $value) {
    $id = $this->asNodeId($node, __METHOD__);
    $request = array('path' => '/node/'.$id.'/properties/'.$key,
                     'method' => 'PUT');
    $request['datas'] = $value;
    $this->call($request);
  }
  
  public function removeNode($node) {
    $node_id = $this->asNodeId($node, __METHOD__);
    $request = array('path' => '/node/'.$node_id.'/relationships/all');
    $response = $this->call($request);
    $relationships = array();
    foreach ($response as $relationship) {
      $parts = explode('/', $relationship['self']);
      $relationships[] = array_pop($parts);
    }
    foreach ($relationships as $relationship_id) {
      $request = array('path' => '/relationship/'.$relationship_id,
                       'method' => 'DELETE');
      $response = $this->call($request);
    }
    $request = array('path' => '/node/'.$node_id,
                     'method' => 'DELETE');
    $this->call($request);
    S('dbo')->exec('UPDATE '.Meta::TABLE
                   . ' SET '.Meta::FIELD_NODE.'=0'
                   .' WHERE '.Meta::FIELD_NODE.'='.$node_id);
  }
  
  private function call($request) {
    $url = $this->server . $request['path'];
    
    $opts = array('method' => $request['method'] ?: 'GET',
                  'timeout' => 2);
    
    $content = '';
    $json_decode = false;
    $headers = array();
    if (!in_array($opts['method'], array('PUT', 'DELETE'))) {
      $headers['Accept'] = 'application/json';
      $json_decode = true;
    }
    
    if (isset($request['datas'])) {
      $datas = $request['datas'];
      $datas = json_encode($datas);
      $content = $datas;
      $headers['Content-Type'] = 'application/json';
    }
    
    if (count($headers) >= 1) {
      $opts['headers'] = $headers;
    }

    $this->last_call = array('request' => array('url' => $url,
                                                'context' => $opts),
                             'response' => '');

/*    $hc = HttpClient::getInstance($url, $content, $opts);
    $response = $hc->fetch(); */

    $response = HttpRequest::getInstance($url, $content, $opts)->send()->getContent();

    if ($response === false) {
      S('log')->error('call failure', __METHOD__);
      return '';
    }
    $this->last_call['response']['content'] = $response;

    $code = $hres->getStatus(); 
    if ($code >= 400) {
      S('log')->trace('call failure ('.$code.') ['.$http['method'].' '.$url.']', __METHOD__);
      $this->debug();
      $e = new Exception();
      $e->response_code = $code;
      throw $e;
    }

    if ($this->trace > 0) {
      $this->debug();
    }

    if ($json_decode === true && !empty($response)) {
      $response = json_decode($response, true);
    }

    return $response;
  }


  private function call2($request) {
    $url = $this->server . $request['path'];
    
    $http = array('method' => $request['method'] ?: 'GET',
                  'timeout' => 2,
                  'ignore_errors' => true,
                  'header' => '');
    
    $json_decode = false;
    $headers = array();
    if (!in_array($http['method'], array('PUT', 'DELETE'))) {
      $headers[] = 'Accept: application/json';
      $json_decode = true;
    }
    
    if (isset($request['datas'])) {
      $datas = $request['datas'];
      $datas = json_encode($datas);
      $http['content'] = $datas;
      $headers[] = 'Content-Type: application/json';
    }
    
    if (count($headers) >= 1) {
      $http['header'] = join("\r\n", $headers);
    }

    $this->last_call = array('request' => array('url' => $url,
                                                'context' => $http),
                             'response' => '');

    $context = stream_context_create(array('http' => $http)); 

    $stream = @fopen($url, 'r', false, $context); 

    if ($stream === false) {
      S('log')->trace('unavailable url ['.$url.']', __METHOD__);
      return '';
    }

    $metas = stream_get_meta_data($stream);
    $this->last_call['response']['metas'] = $metas;

    if ($metas === false) {
      S('log')->trace('invalid metas ['.$url.']', __METHOD__);
      return '';
    }

    $response = stream_get_contents($stream);

    $this->last_call['response']['content'] = $response;

    $code = (int) substr($metas['wrapper_data'][0], 9, 3); 
    if ($code >= 400) {
      S('log')->trace('call failure ('.$code.') ['.$http['method'].' '.$url.']', __METHOD__);
      $this->debug();
      $e = new Exception();
      $e->response_code = $code;
      throw $e;
    }

    fclose($stream);

    if ($this->trace > 0) {
      $this->debug();
    }

    if ($json_decode === true && !empty($response)) {
      $response = json_decode($response, true);
    }

    return $response;
  }

  public function debug() {
    S('log')->debug($this->last_call);
  }

  
  private function asNodeId($node, $method=null) {
    if (is_array($node)) {
      $node = Item::getInstance($node[0], $node[1]);
    }
    if ($node instanceof ActiveRecord) {
      $id = (int) $node->getMeta(Meta::FIELD_NODE);
      if ($id < 1) {
        $id  = $this->findNode($node);
        if ($id >= 1) {
          $node->setMeta(Meta::FIELD_NODE, $id);
        }
        else {
          $this->addNode($node);
          $id = (int) $node->getMeta(Meta::FIELD_NODE);
        }
      }
    }
    else {
      $id = (int) $node;
    }
    if ($id < 1) {
      S('log')->error('invalid node_id ['.$method.']', __METHOD__);
    }
    return $id;
  }

  public function ping() {
    $request = array();
    $resp = $this->call($request);
    S('log')->debug($resp);
  }

  public function init() {
    $response = $this->call(array('path' => '/index/node'));
    if (!$response[self::NODE_INDEX]) {
      $this->call(array('path' => '/index/node',
                        'method' => 'POST',
                        'datas' => array('name' => self::NODE_INDEX)));
      S('log')->trace('index creation : '.self::NODE_INDEX,
                      __METHOD__);
    }
  }

  public function rebuild() {
    $this->ping();
    S('dbo')->exec('UPDATE '.Meta::TABLE
                   . ' SET '.Meta::FIELD_NODE.'=0');
    $this->init();
    foreach (User::collect() as $user) {
      try {
        $this->addNode($user);
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    $rs = S('dbo')->query('SELECT m1.'.Meta::FIELD_NODE.' AS from_id,'
                          .     ' m2.'.Meta::FIELD_NODE.' AS to_id'
                          .' FROM '.Network::TABLE.' AS n,'
                          .     ' '.Meta::TABLE.' AS m1,'
                          .     ' '.Meta::TABLE.' AS m2'
                          .' WHERE m1.'.Meta::FIELD_MODEL.'='.Model::id('user')
                          .  ' AND m1.'.Meta::FIELD_RECORD.'=n.user_id'
                          .  ' AND m2.'.Meta::FIELD_MODEL.'='.Model::id('user')
                          .  ' AND m2.'.Meta::FIELD_RECORD.'=n.friend_id'
                          .  ' AND n.status>0');
    foreach ($rs as $row) {
      S('log')->trace('friend : ('.$row['from_id'].') -> ('.$row['to_id'].')',
                      __METHOD__);
      $this->addConnexion((int) $row['from_id'], 
                          (int) $row['to_id']);
    }
    S('app')->hook('onGraphRebuild');
  }

  private static $singleton = null;
    
  public static function getInstance() {
    if (is_null(self::$singleton)) {
      self::$singleton = new Graph();
    }
    return self::$singleton;
  }

  public static function nodes2items($nodes) {
    if (empty($nodes)) return array();
    $rs = S('dbo')->query('SELECT node,'
                            .     ' '.Meta::FIELD_MODEL.','
                            .     ' '.Meta::FIELD_RECORD
                            .' FROM '.Meta::TABLE
                            .' WHERE '.Meta::FIELD_NODE.' IN ('.join(',', $nodes).')'); 
    $items = array();
    foreach ($rs as $row) {
      $items[$row[Meta::FIELD_NODE]] = Item::getInstance($row[Meta::FIELD_MODEL], 
                                              $row[Meta::FIELD_RECORD]);
    }
    return $items;
  }

  public static function getItemFromNode($graph_node_id) {
    $ht = S('dbo')->fetchHash('SELECT '.Meta::FIELD_MODEL.','
                              .     ' '.Meta::FIELD_RECORD
                              .' FROM '.Meta::TABLE
                              .' WHERE '.Meta::FIELD_NODE.'='.$graph_node_id);
    return Item::getInstance($ht[Meta::FIELD_MODEL], 
                             $ht[Meta::FIELD_RECORD]);
  }

}

