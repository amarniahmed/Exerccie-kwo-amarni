<?php

$csv = CsvReader::getInstance('lang.csv',
                              array('fields' => array('code', 'code2', 'code3',
                                                      'name', 'name2', 'cmt')));

foreach ($csv as $lang) {
  AH::trim($lang);
  $record_arr = array('code' => $lang['code'],
                      'code2' => $lang['code2'],
                      'code3' => $lang['code3'],
                      'name' => $lang['name']);
  $dbo->insert('static_lang', $record_arr);
}