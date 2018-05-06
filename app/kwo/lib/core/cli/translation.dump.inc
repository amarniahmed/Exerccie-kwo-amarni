<?php

try {

  $table = Item::TABLE_PROPERTY;
  $lang = 'fr';

  $rs = $dbo->query('SELECT model_id, record_id, value'
                    .' FROM '.$table
                    .' WHERE locale='.Locale::toNum($lang)//.' WHERE locale='.I18N::locale2num($lang)
                    .  ' AND VALUE<>""'
                    .' ORDER BY value');

  $doc = new DOMDocument('1.0', 'UTF-8');
  $doc->xmlStandalone = true;
  $doc->formatOutput = true;
  $root = $doc->createElement('contents');
  $doc->appendChild($root);
  foreach ($rs as $row) {
    $node = $doc->createElement('content');
    $cdata = $doc->createCDATASection($row['value']);
    $node->appendChild($cdata);
    $node->setAttribute('model_id', $row['model_id']);
    $node->setAttribute('record_id', $row['record_id']);
    $root->appendChild($node);
  }
  echo $doc->saveXML();

}
catch (Exception $e) { $err->add($e); }
