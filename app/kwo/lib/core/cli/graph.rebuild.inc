<?php

try {

  /*  system('/opt/neo4j/bin/neo4j stop');
  system('rm -r /opt/neo4j/data/graph.db');
  system('/opt/neo4j/bin/neo4j start');*/

  Graph::getInstance()->rebuild();

}
catch (Exception $e) { $err->add($e); }
