<?php

class Author extends ActiveRecord {

  public static function asHash() {
    return S('dbo')->query('SELECT id, name'
                           .' FROM '.T('author')
                           .' WHERE status>0'
                           .' ORDER BY name')->asHash();
  }

}