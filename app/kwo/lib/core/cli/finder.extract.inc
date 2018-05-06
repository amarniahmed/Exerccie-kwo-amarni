<?php

try {

  foreach (glob('lib/*/xml/*.home.xml') as $file) {

    $dir = dirname($file);
    $filename = basename($file);
    list ($model, ) = explode('.', $filename);

    echo $file."\n";
    $doc = new DOMDocument;
    $doc->formatOutput = true;
    $doc->preserveWhiteSpace = false;
    $doc->load($file);
    //    $xpath = new DOMXPath($doc);
    $finder = $doc->getElementsByTagName('finder')->item(0);

    $out = new DOMDocument('1.0', 'UTF-8');
    $out->xmlStandalone = true;
    $out->formatOutput = true;
    $root = $out->createElement('kwo-interface');
    $node = $out->importNode($finder, true);
    $root = $out->appendChild($root);
    $root->appendChild($node);
    $outfile = $dir.'/'.$model.'.finder.xml';
    //    echo $out->saveXML() . "\n";
    $out->save($outfile);

    $newnode = $doc->createElement('finder');   
    $parent = $finder->parentNode;
    $parent->replaceChild($newnode, $finder);
        
    //    echo $doc->saveXML() . "\n";
    //    $doc->save($file);
  }

}
catch (Exception $e) { $err->add($e); }
