<?php

class Template
{
  private $current = null;
  private $stack = array();
  private $tiles = array();

  const PATH = 'templates';

  public function __construct() {}

  private function setTiles($tiles) {
    $this->stack = array();
//    $tiles = is_array(func_get_arg(0)) ? func_get_arg(0) : func_get_args();
    foreach ($tiles as $tile) {
      if ($tile === null) {
      	$this->stack = array();
      	continue ;
      }
      if (!array_key_exists($tile, $this->tiles)) {
        $this->addTile($tile);
      }
      $this->stack[] = $tile;
    }
  }

  /***
  true (= nom action)
  template
  extension:dir:template
  extension:template (dir=scope)
  ***/

  private function addTile($name) {
    $key = $name;
    $req = S('req');
    if (strpos($name, ':') > 0) {
      $parts = explode(':', $name);
      if (strpos($name, '/') !== false) {
        S('log')->error('invalid template name ['.$name.']', __METHOD__);
        throw new Exception('invalid template');
      }
      if (!Extension::exists($parts[0])) {
        S('log')->error('invalid template, extension unknown ['.$name.']', __METHOD__);
        throw new Exception('invalid template');
      }
      if (count($parts) === 3) {
        if ($parts[1] === 'back' && !$req->isBack()) {
          S('log')->error('invalid template usage ['.$name.']', __METHOD__);
          throw new Exception('invalid template');
        }
        $path = WEB_PATH.'/'.$parts[0].'/'.self::PATH.'/'.$parts[1].'/'.$parts[2].'.psp';
      }
      else {
        $path = WEB_PATH.'/'.$parts[0].'/'.self::PATH.'/'.$req->getScope().'/'.$parts[1].'.psp';
        // TODO : A supprimer
        if (!file_exists($path)) {
//          S('log')->trace('DEPRECATED: move template in a scope subdirectory ['.$path.']');
          $path = WEB_PATH.'/'.$parts[0].'/'.self::PATH.'/'.$parts[1].'.psp';
        }
      }
    }
    // TODO : a supprimer
    elseif ($name[0] === '/') {
//      S('log')->trace('DEPRECATED: ['.$name.']');
      if ($name[1] == '/') {
        $name = substr($name, 2);
        $extension = CORE;
        if (strpos($name, ':') > 1) {
          list ($extension, $name) = explode(':', $name);
        }
        if (file_exists(WEB_PATH.'/'.$extension.'/'.self::PATH.'/back/'.$name.'.psp')) {
          $path = WEB_PATH.'/'.$extension.'/'.self::PATH.'/back/'.$name.'.psp';
        }
        else {
          $path = LIB_PATH.'/'.$extension.'/'.self::PATH.'/'.$name.'.psp';
        }
      }
      else {
        $path = WEB_PATH.'/'.CORE.'/'.self::PATH.'/'.substr($name, 1).'.psp';
      }
    }
    else {
/*      if (strpos($name, ':') > 0) {
        list ($extension, $file) = explode(':', $name);
        $path = WEB_PATH.'/'.$extension.'/'.self::PATH.'/'.$file.'.psp';
        }
        else { */
      $extension = $req->getExtension();
      // TODO : a supprimer
      if ($req->isBack()) {
        if (file_exists(WEB_PATH.'/'.$extension.'/'.self::PATH.'/back/'.$name.'.psp')) {
          $path = WEB_PATH.'/'.$extension.'/'.self::PATH.'/back/'.$name.'.psp';
        }
        else {
          $path = LIB_PATH.'/'.$extension.'/'.self::PATH.'/'.$name.'.psp';
        }
      }
      else {
        $path = WEB_PATH.'/'.$extension.'/'.self::PATH.'/'.$req->getScope().'/'.$name.'.psp';
        if (FrontManager::inWhitelist($name) && !file_exists($path)) {
          $path = WEB_PATH.'/'.CORE.'/'.self::PATH.'/'.$name.'.psp';
        }
        // TODO : a supprimer
        elseif (!file_exists($path)) {
          $path = WEB_PATH.'/'.$extension.'/'.self::PATH.'/'.$name.'.psp';
        }
      }
      if ($extension === DEVEL && (empty($path) || !file_exists($path))) {
        foreach (array('account', 'cms', 'middle', 'shop', 'social') as $dir) {
          $path = WEB_PATH.'/'.CORE.'/'.self::PATH.'/'.$dir.'/'.$name.'.psp';
          if (file_exists($path)) break ;
        }
      }
//      }
    }
//    $this->tiles[$key] = $path;
//if (array_key_exists($name, $this->sources)) return true;
    $src = @file_get_contents($path);
    if ($src === false || strlen($src) < 1) {
      if ($src === false) {
        S('log')->error('error [name='.$name.' path='.$path.']', __METHOD__);
      }
      $src = '';
    }
//    $this->sources[$name] = $src;

    $this->tiles[$key] = array('path' => $path, 'source' => $src);

  }

  private function bind($bindings=null) {
    $out = '';
    if ($bindings instanceof Context || $bindings instanceof ActiveRecord) {
      $bindings = $bindings->getAttributes();
    }
    elseif (is_object($bindings)) {
      $bindings = get_object_vars($bindings);
    }
    elseif (!is_array($bindings)) {
      $bindings = array();
    }

    $__bindings = $bindings;
    $__stack = array_reverse($this->stack);
    $__handler = set_error_handler(array($this, 'onTemplateError'), E_ALL & ~E_NOTICE);

    unset($bindings);

    extract($__bindings, EXTR_SKIP);

    //    error_log(E_ALL & ~(E_NOTICE | E_WARNING));

    foreach ($__stack as $__tile) {
      $__path = $this->tiles[$__tile]['path'];
      $__source = $this->tiles[$__tile]['source'];
      $this->current = $__path;
      ob_start();
      if (eval('?>'.$__source.'<?php ') === false) {
        $__err = error_get_last();
        S('log')->error($__err['message'].' on line '.$__err['line'].' ['.$__err['type'].']',
                        $__path);
      }
      $_content = ob_get_clean();
    }
    restore_error_handler();
    $out = $_content;
    return $out;
  }

  public function onTemplateError($errno, $errstr, $errfile, $errline) {
    if ($errno & E_WARNING) return ;
    S('log')->error($this->current.' (line='.$errline.':err='.$errno.')'.' '.trim($errstr),
                    __METHOD__);
    return true;
  }

  /*** STATIC ***/

  public static function render($templates, $bindings=null) {
    $ctx = S('ctx');
    $templates = (array) $templates;
    if (empty($templates)) {
      S('log')->debug($templates, __METHOD__);
      S('log')->error('invalid template', __METHOD__);
      return '';
    }
    if ($bindings === true) {
      $bindings = $ctx->getAttributes();
    }
    elseif ($bindings instanceof Context || $bindings instanceof ActiveRecord) {
      $bindings = $bindings->getAttributes();
    }
    elseif (is_array($bindings)) {
      $out = array();
      foreach ($bindings as $key => $value) {
        if (intval($key) && $value === true) {
          foreach ($ctx->getAttributes() as $k => $v) {
            $out[$k] = $v;
          }
        }
        else {
          $out[$key] = $value;
        }
      }
      $bindings = $out;
      $out = null;
    }
    $template = self::getInstance();
    $template->setTiles($templates);
    $out = $template->bind($bindings);
    $template = $bindings = null;
    return $out;
  }

  private static $singleton = null;

  public static function getInstance() {
    if (self::$singleton === null) {
      $class = __CLASS__;
      self::$singleton = new $class;
    }
    return self::$singleton;
  }

}


/*  private function loadSource($name) {
    if (array_key_exists($name, $this->sources)) return true;
    $src = @file_get_contents($this->tiles[$name]);
    if ($src === false || strlen($src) < 1) {
      if ($src === false) {
        S('log')->error('error [name='.$name.' path='.$this->tiles[$name].']',
                        __METHOD__);
      }
      $src = '';
    }
    $this->sources[$name] = $src;
  } */


/*

  public function setTile($name, $src) {
    $this->tiles[$name] = $src;
    $this->sources[$name] = $src;
  }

  public static function addTile($name, $code) {
    self::getInstance()->setTile($name, $code);
  }

*/
