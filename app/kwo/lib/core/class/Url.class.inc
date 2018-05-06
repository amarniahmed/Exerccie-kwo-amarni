<?php

class Url extends ActiveRecord
{
  protected function onBeforeCommit(&$hash) {
    if (isset($hash['name'])) {
      $hash['crc'] = sprintf('%u', crc32($hash['name']));
    }
  }

  protected function onBeforeInstantiate() {
    if (isset($this->_args['name'])) {
      $this->_args['crc'] = sprintf('%u', crc32($this->_args['name']));
    }
  }

  public static function asId($url) {
    if (is_numeric($url)) return $url;
    $url = str_replace(array('<', '>', '&amp;'),
                       array('', '', '&'),
                       $url);
    $crc = sprintf('%u', crc32($url));
    $id = S('dbo')->fetchValue('SELECT id'
                               .' FROM '.T('url')
                               .' WHERE name=:url:'
                               .  ' AND crc='.$crc,
                               array(':url:' => $url));
    if ($id < 1) {
      $id = S('dbo')->insert(T('url'),
                             array('name' => $url,
                                   'crc' => $crc));
      if ($id < 1) {
        throw new Exception('url insertion failure');
      }
    }
    return $id;
  }

  public static function asString($id) {
    $url = S('dbo')->fetchValue('SELECT name'
                                .' FROM '.T('url')
                                .' WHERE id='.intval($id));
    if ($url === false) {
      throw new Exception('url not found');
    }
    return $url;
  }

  public static function asItem($url) {
    $url = str_replace(array('<', '>', '&amp;'),
                       array('', '', '&'),
                       $url);
    try {
      $opts = array('ignore' => true);
      $item = Url::getInstance(array('name' => $url),
                              $opts);
    }
    catch (Exception $e) {
      $item = Url::getInstance();
      $item->commit(array('name' => $url));
    }
    return $item;
  }

  public function asIdentityHash($locale, $type=null, $options=null) {
    $description = $this->name == '/' ? H::url() : $this->name;
    return array('name' => 'adresse',
                 'description' => ' '.$description,
                 'image' => PIX_URL.'/share/identity-url.png');
  }

  public static function onAppReset() {
    $dbo = S('dbo');
    $tables = array(T('url'));
    foreach ($tables as $table) {
      if (!$dbo->hasTable($table)) {
        S('log')->trace('table unknown ['.$table.']', __METHOD__);
        continue ;
      }
      $dbo->getTable($table)->truncate();
    }
  }

}