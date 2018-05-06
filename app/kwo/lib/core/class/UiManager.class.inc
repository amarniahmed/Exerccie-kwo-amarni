<?php

/* Element is a subinterface of Node.
An Element represents an element in an XML document, so something that begins with a start tag and ends with an end tag. An Element is a Node, but there are other things besides Element which are also Nodes. For example, attributes, comments etc. are also Nodes, but not Elements. */

class UiManager
{
  private $ctx = null;
  private $branch = array();
  private $branches = array();
  private $dialogs = array();
  private $doc = null;
  private $elt_num = 0;
  private $item = null;
  private $level = 0;
  private $model = null;
  private $nl = '~~~~';
  private $nl_replace_flag = false;
  private $req = null;
  private $scripts = array();
  private $tab = '   ';
  private $xml_file = '';

  const IFRAME_HEIGHT = '650px';
  const COLOR1 = '#222';
  const COLOR2 = '#F66013';
  const COLOR3 = '#F8ECDF';
  const FONTSIZE = 12;
  const WIDTH = '960';
  const HEIGHT = '670';

  const SELECT_CHAR = '&middot;';
  const RESET_CHAR = 'x';

  public static $containers = array('box', 'datagrid', 'deck', 'dialogheader',
                                    'elem',
                                    'fieldset', 'window',
                                    'finder', 'form', 'grid', 'grid', 'list',
                                    'hbox', 'iframe', 'launcher', 'link', 'list2',
                                    'notebook', 'scrollbox', 'spacer',
                                    'stackpanel', 'tree', 'trigger', 'triggers',
                                    );

  public static $elems = array('text', 'password', 'hidden', 'select',
                               'richtext', 'radio', 'lang',
                               'checkbox', 'date', 'datetime', 'period',
                               'folder', 'file',
                               'image', 'yesno',
                               'resources', 'relations', 'acl',
                               'geo', 'item', 'grid',
                               'password', 'biselect', 'flowselect', 'coords');

  public static $properties = null;

  public static $locales = null;

  public function __construct($file, $ctx=null) {
    $this->req = S('req');
    $this->action = $this->req->getAction();
    $this->log = S('log');
    $this->ctx = S('ctx');
    $this->branch = $this->req->getBranch();
    $this->branches = $this->req->getBranch();
    $this->dialogs = $this->req->getDialogs();
    $this->rand = rand(100, 999);
    $this->doc = new DOMDocument();
    $this->doc->preserveWhiteSpace = false;
    $file_path = LIB_PATH.'/'.$this->req->getExtension().'/xml/'.$file.'.xml';
    if (!file_exists($file_path)) {
      list ($model, $action) = explode('.', basename($file), 2);
      $extension = $model === 'item' ? CORE : Model::parameter($model, 'extension');
      $file_path = LIB_PATH.'/'.$extension.'/xml/'.$model.'.'.$action.'.xml';
      if (!file_exists($file_path)) {
        $file_path = LIB_PATH.'/'.CORE.'/xml/item/'.$action.'.xml';
      }
    }
    $this->xml_file = $file_path;
    if ($this->doc->load($file_path) === false) {
      throw new Exception();
    }
    if ($this->ctx->item instanceof ActiveRecord) {
      $this->item = $this->ctx->item;
      $this->model = $this->item->getModel();
    }
    $this->expandDom();
  }

  private function expandDom() {
    $xpath = new DomXPath($this->doc);
    if ($this->action == 'user.edit') {
      $nodes = $xpath->query('//link[@icon="profile"]');
      if ($nodes->length == 1) {
        $node = $nodes->item(0);
        $arr = array($node);
        $n = count(P('profiles'));
        $parent = $node->parentNode;
        for ($i = 1; $i < $n; $i++) {
          $arr[] = $parent->insertBefore($this->doc->createElement('link'), $node);
        }
        $deck = $this->doc->getElementsByTagName('deck')->item(0);
        $i = 0;
        foreach (P('profiles') as $model) {
          $elt = $arr[$i];
          $elt->setAttribute('icon', 'profile');
          $elt->setAttribute('label', Model::label($model));
          $elt->setAttribute('action', "Kwo.F('" . $model . "').onEdit(this)");
          $box = $this->doc->createElement('box');
          $box->setAttribute('class', $model.'-box');
          $deck->appendChild($box);
          $i++;
        }
      }
    }
    foreach ($xpath->query('//*[@expander]') as $node) {
      // TODO : method that will expand the node
      // $out = call_user_func(array($class, $method), $node, this);
    }
    foreach ($xpath->query('//*[@localized="true"]') as $node) {
      $this->expandLocalized($node);
    }
    $nodes = $xpath->query('//elem[@field="app" or @field="app_id"]');
    if ($nodes->length > 0) {
      if (App::count() <= 1) {
          $this->removeNode($nodes->item(0));
      }
    }
    if (!$this->hasItem()) {
      foreach ($xpath->query('//elem[@type="relations" or @type="acl" or @type="resources"]') as $node) {
        $this->removeNode($node);
      }
    }
    foreach ($xpath->query('//*[@built]') as $node) {
      if ($this->isBuilt($node)) continue ;
      $this->removeNode($node);
    }
    foreach ($xpath->query('//hbox') as $node) {
      $is_empty = true;
      foreach ($node->childNodes as $vbox) {
        if ($vbox->hasChildNodes()) {
          $is_empty = false;
          break ;
        }
      }
      if ($is_empty === true) {
        $this->removeNode($node);
      }
    }
    // TODO : expandLink, expandLauncher
  }

  private function expandLocalized($node) {
    if (is_null(self::$properties)) {
      self::$properties = Model::properties($this->item);
      if ($this->item->isItem()) {
        $properties = $this->item->getPropertiesById();
        foreach ($properties as $locale => $hash) {
          foreach ($hash as $key => $value) {
            $this->ctx->setAttribute('property_'.$locale.'_'.$key, $value);
          }
        }
      }
    }
    self::$locales = self::$locales ?: self::locales();
    if ($this->item->isItem() &&
        $this->item->isA('string') &&
        $this->item->type == String::TYPE_BACK &&
        !empty(P('back.admin.locales'))) {
      $locale = BackManager::getLocale();
      $locales = array($locale => Locale::toCode($locale));
      foreach (P('back.admin.locales') as $_locale) {
        if ($_locale == $locale) continue;
        $locales[$_locale] = Locale::toCode($_locale);
      }
    }
    else {
      $locales = self::$locales;
    }
    if (count($locales) < 1) return ;
    $children = array();
    foreach ($node->childNodes as $child) {
      if ($child->nodeType != XML_ELEMENT_NODE) continue ;
      $children[] = $child;
    }
    $branches = array();
    foreach ($children as $child) {
      $branches[] = $node->removeChild($child);
    }
    $notebook = $this->doc->createElement('notebook');
    foreach ($locales as $locale => $lang) {
      $page = $this->doc->createElement('page');
      $page->setAttribute('label', strtoupper($lang));
      if (count($locales) <= 10) {
        $page->setAttribute('ico', PIX_URL.'/lang/'.Locale::toCode($locale).'.png');
      }
      foreach ($branches as $branch) {
        $clone = $branch->cloneNode(true);
        $page->appendChild($clone);
        foreach ($page->getElementsByTagName('elem') as $elt) {
          foreach ($elt->attributes as $attr) {
            if (strpos($attr->value, '$locale') === false) continue ;
            $attr->value = str_replace('$locale',
                                       "'".Locale::toCode($locale)."'",
                                       $attr->value);
          }
          $h = self::$properties[$elt->getAttribute('field')];
          $name = 'properties['.$locale.']['.$h['id'].']';
          $elt->setAttribute('name', $name);
          $elt->setAttribute('lang', Locale::toCode($locale));
          $value = '$property_'.$locale.'_'.$h['id'];
          $elt->setAttribute('value', $value);
          if (!$elt->hasAttribute('label')) {
            $elt->setAttribute('label', $elt->getAttribute('field'));
          }
        }
      }
      $notebook->appendChild($page);
    }
    $node->appendChild($notebook);
  }

  private function removeNode($node) {
    return $node->parentNode->removeChild($node);
  }

  public function render() {
    $html = '';
    $root = $this->doc->documentElement;
    if ($root->tagName != 'kwo-interface') {
      $root = $this->doc;
    }
    $html .= $this->renderContainer($root);
    if ($this->nl_replace_flag === true) {
      $html = str_replace($this->nl, "\n", $html);
    }
    $html = ltrim($html);
    foreach ($this->scripts as $script) {
      $html .= '<script src="'.$script.'" type="text/javascript"></script>'."\n";
    }
    if (substr($this->action, -5) === '.edit' && $this->ctx->item instanceof ActiveRecord) {
      $item = $this->ctx->item;
      $attrs = array('class' => array('box-edit', 'data-item', 'data-branch', 'model-'.$item->getModel()),
                     'data' => array('id' => 0,
                                     'branch-key' => $item->getModel().'_id',
                                     'branch-value' => 0,
                                     'model' => $item->getModel(),
                                     'model-id' => $item->getModelId()));
      if ($item->isItem()) {
        $attrs['data']['id'] = $item->getId();
        $attrs['data']['branch-value'] = $item->getId();
        $attrs['data']['item'] = $item->getKey();
      }
      $html = H::elt('div', "\n".$html, $attrs)."\n";
    }
    return $html;
  }

  private function renderContainer($node) {
    $html = '';
    $this->level++;
    if ($node instanceof DOMDocument) {
      $children[] = $this->doc->documentElement;
    }
    else {
      $children = $node->childNodes;
    }
    foreach ($children as $child) {
      if ($child->nodeType != XML_ELEMENT_NODE) continue ;
      if (in_array($child->nodeName, self::$containers)) {
        $method = 'render'.ucfirst($child->nodeName);
        $out = call_user_func(array($this, $method), $child);
      }
      else {
        S('log')->trace('container: ['.$child->nodeName.']', __METHOD__);
      }
      $html .= "\n".$out;
    }
    $this->level--;
    if ($this->level >=1) {
      $html = preg_replace('/^/m',$this->tab, $html);
    }
    $html .= "\n";
    return $html;
  }

  private function getNodeAttribute($node, $attr, $skip_eval=false) {
    if ($node->hasAttribute($attr) === false) return null;
    $value = trim($node->getAttribute($attr));
    if (strlen($value) < 1) {
      return '';
    }
    if (strpos($value, '$(') === false &&
        (strpos($value, '$') !== false ||
         strpos($value, '!') !== false ||
         strpos($value, '::') !== false ||
         substr($value, 0, 2) === 'P(')) {
      if ($value[0] === '$' || $value[0] === '!') {
        if ($value[0] === '!') {
          $negate = true;
          $value = substr($value, 1);
        }
        $value = substr($value, 1);
        if (!$this->ctx->hasAttribute($value)) {
          if ($node->hasAttribute('default')) {
            return $this->getNodeAttribute($node, 'default');
          }
        }
        $value = '$this->ctx->'.$value;
      }
      $ret = '';
      eval('$ret = '.$value.';');
      if ($negate === true) {
        $ret = !$ret;
      }
      return $ret;
    }
    if (is_string($value)) $value = $this->convert($value);
    return $value;
  }

  private function protect($str) {
    return self::input_protect($str);
  }

  public static function input_protect($str) {
    return strtr($str,
                 array("'" => '&#039;',
                       '"' => '&quot;',
                       '&' => '&amp;'));
  }

  private function convert($str) {
    return $str;
  }

  private function getValue($s) {
    if ($s[0] === '$') {
      $attr = substr($s, 1);
      if (substr($attr, -1, 1) === ']') {
        list($attr, $cell) = explode('[', $attr);
        $cell = rtrim($cell, ']');
        $attr = $this->ctx->getAttribute($attr);
        return $attr[$cell];
      }
      return $this->ctx->getAttribute($attr);
    }
    return $this->convert($s);
  }

  private function isBuilt($node) {
    if (!$node->hasAttribute('built')) return true;
    $value = $this->getNodeAttribute($node, 'built');
    if ($value === true || $value === 'true' || $value === 1 ||
        is_object($value) ||
        (is_array($value) && count($value) >= 1)) return true;
    return false;
  }

  private function isActive($node) {
    if (!$node->hasAttribute('active')) return true;
    $value = $this->getNodeAttribute($node, 'active');
    if ($value===true || $value==='true') return true;
    return false;
  }

  private function isVisible($node) {
    if (!$node->hasAttribute('visible')) return true;
    $value = $this->getValue($node->getAttribute('visible'));
    return $value === true || $value === 'true';
  }

  private function isChecked($node) {
    if (!$node->hasAttribute('checked')) return false;
    $value = $this->getValue($node->getAttribute('checked'));
    if ($value===true || $value==='true') return true;
    return false;
  }

  private function isDisabled($node) {
    if (!$node->hasAttribute('disabled')) return false;
    $value = $this->getNodeAttribute($node, 'disabled');
    return ($value === true || $value === 1 || $value === 'true' ||
            is_object($value) ||
            (is_array($value) && count($value) >= 1));
  }

  private function isReadonly($node) {
    if (!$node->hasAttribute('readonly')) return false;
    $value = $this->getNodeAttribute($node, 'readonly');
    return ($value === true || $value === 1 || $value === 'true' ||
            is_object($value) ||
            (is_array($value) && count($value) >= 1));
  }

  private function isEditable($node) {
    if (!$node->hasAttribute('editable')) return true;
    $value = $this->getNodeAttribute($node, 'editable');
    return $value === true || $value === 'true';
  }

  private function useAttribute($node, $attribute) {
    if (!$node->hasAttribute($attribute)) return false;
    $value = $this->getNodeAttribute($node, $attribute);
    return $value === true || $value === 'true' || $value == 1;
  }

  private function debug_node($o) {
    print("DEBUG [$o->nodeName] $o->nodeType");
    if ($o->nodeName==='elem') print("| ".$o->getAttribute('type'));
    print("<hr/>");
  }

  private function buildTagsUI($item) {
    foreach ($this->ctx->hash as $locale => $hash) {
      foreach ($hash as $key => $value) {
        $this->ctx->setAttribute('field_'.$locale.'_'.$key, $value);
      }
    }
    $has_notebook = true;
    $locales = self::locales();
    if ($this->item->hasAttribute('locales')) {
      $locales = $this->item->getLocales();
      $locales = array_flip($locales);
      foreach ($locales as $locale => &$code) $code = Locale::toCode($locale);//foreach ($locales as $locale => &$code) $code = I18N::locale2code($locale);
    }
    elseif ($item->getLocale() >= 1 ||
        (!Model::hasProperties($item->getModelId()) && !$item->hasAttribute('type_id') && !$item->isA(Model::id('action')))) {
      $locale = $item->getLocale();
      $locales = array(intval($locale) => $locales[$locale]);
      $has_notebook = false;
    }
    if ($has_notebook === true) {
      $notebook = $this->doc->createElement('notebook');
    }
    else {
      $container = $this->doc->createElement('form');
    }
    $elems = array('tags' => 'tags');
    foreach ($locales as $locale => $name) {
      if ($has_notebook === true) {
        $page = $this->doc->createElement('page');
        $container = $page;
        $page->setAttribute('label', strtoupper($name));
        $page->setAttribute('ico', PIX_URL.'/lang/'.Locale::toCode($locale).'.png');
      }
      foreach ($elems as $key => $value) {
        $elem = $this->doc->createElement('elem');
        $elem->setAttribute('value', '$field_'.$locale.'_'.$key);
        $elem->setAttribute('name', 'tags['.$locale.']');
        $elem->setAttribute('label', $value);

        if (in_array($key, array('tags'))) {
          $elem->setAttribute('count', 12);
        }
        $container->appendChild($elem);
      }
      if ($has_notebook === true) {
        $notebook->appendChild($page);
      }
    }
    if ($has_notebook === true) {
      $form = $this->doc->createElement('form');
      $form->appendChild($notebook);
    }
    else {
      $form = $container;
    }
    $launcher = $this->doc->createElement('launcher');
    $button = $this->doc->createElement('button');
    $button->setAttribute('name', 'store');
    $button->setAttribute('action', ".onExec('tags.set', this)");
    $launcher->appendChild($button);
    $form->appendChild($launcher);
    return $form;
  }

  private function buildSeoUI($item) {
    foreach ($this->ctx->hash as $locale => $hash) {
      foreach ($hash as $key => $value) {
        $this->ctx->setAttribute('field_'.$locale.'_'.$key, $value);
      }
    }
    $has_notebook = true;
    $locales = self::locales();
    if ($this->item->hasAttribute('locales')) {
      /* $locales = $this->item->getLocales();
      $locales = array_flip($locales);
      foreach ($locales as $locale => &$code) $code = I18N::$locales[$locale]; */
      $locales = self::locales();
    }
    elseif ($item->hasAttribute('locale') ||
            (!Model::hasProperties($item->getModelId()) && !$item->hasAttribute('type_id') && !$item->isA(Model::id('action')))) {
      $locales = array(intval($item->locale) => intval($item->locale));
      $has_notebook = false;
    }
    if ($has_notebook === true) {
      $notebook = $this->doc->createElement('notebook');
    }
    else {
      $container = $this->doc->createElement('form');
    }

    //    S('log')->debug('buildSeoUI');

    $elems = array('title' => l('titre'),
                   'description' => l('description'),
                   'tags' => l('tags'));
    foreach ($locales as $locale => $name) {
      if ($has_notebook === true) {
        $page = $this->doc->createElement('page');
        $container = $page;
        $page->setAttribute('label', strtoupper($name));
        $page->setAttribute('ico', PIX_URL.'/lang/'.Locale::toCode($locale).'.png');
      }
      foreach ($elems as $key => $value) {
        $elem = $this->doc->createElement('elem');
        $elem->setAttribute('value', '$field_'.$locale.'_'.$key);
        $elem->setAttribute('name', 'records['.$locale.']['.$key.']');
        $elem->setAttribute('label', $value);
        if (in_array($key, array('description'))) {
          $elem->setAttribute('rows', 5);
        }
        if (in_array($key, array('tags'))) {
          $elem->setAttribute('count', 12);
        }
        $container->appendChild($elem);
      }
      if ($has_notebook === true) {
        $notebook->appendChild($page);
      }
    }
    if ($has_notebook === true) {
      $form = $this->doc->createElement('form');
      $form->appendChild($notebook);
    }
    else {
      $form = $container;
    }
    $launcher = $this->doc->createElement('launcher');
    $button = $this->doc->createElement('button');
    $button->setAttribute('name', 'store');
    $button->setAttribute('action', ".onExec('seo.set', this)");
    $launcher->appendChild($button);
    $form->appendChild($launcher);
    return $form;
  }

  private function renderBox($node) {
    $style = '';
    $id = $node->getAttribute('id');
    $label = $this->getValue($node->getAttribute('label'));
    if ($label == 'seo') {
      $label = l('référencement');
      $node->appendChild($this->buildSeoUI($this->item));
    }
    elseif ($label == 'tags') {
      $node->appendChild($this->buildTagsUI($this->item));
    }
    if ($node->hasAttribute('height')) {
      $height = $node->getAttribute('height');
      $style .= 'height:'.$height.'; overflow:auto;';
    }
    if ($node->hasAttribute('width')) {
      $height = $node->getAttribute('width');
      $style .= 'width:'.$height.';';
    }
    if ($node->hasAttribute('border')) {
      $style .= 'border:1px solid #aaa;';
    }
    $divid = '';
    $display = $node->getAttribute('visible') === 'false' ? 'display:none;' : '';
    if (empty($label)) {
      if (strlen($id) > 0) $divid = ' id="'.$id.'"';
    }
    else {
      $html .= '<fieldset id="'.$id.'" style="'.$display.'"><legend>'.c(l($label)).'</legend>'."\n";
      $display = '';
    }
    $class = $node->hasAttribute('class') ? ' class="'.$node->getAttribute('class').'"' : ' class="box"';
    $style .= $display;
    $style = empty($style) ? '' : ' style="'.$style.'"';
    $datas = '';
    foreach ($node->attributes as $attr) {
      if (substr($attr->name, 0, 5) != 'data-') continue ;
      $datas .= ' '.$attr->name.'="'.$attr->value.'"';
    }
    $html .= '<div' . $style . $class . $divid . $datas . '>';
    //    $html .= '<div style="overflow-y:auto;'.$style.$display.'"'.$class.$divid.'>';
    if ($node->hasAttribute('value')) {
      $records = $this->getNodeAttribute($node, 'value');
      if (is_array($records)) {
        if ($node->hasAttribute('template')) {
          $i = 1;
          foreach ($records as $record) {
            $content = Template::render($node->getAttribute('template'), $record);
            $html .= ' <div>'.$content.'</div>'."\n";
          }
        }
/*        elseif ($node->hasAttribute('overlay')) {
          $template = Template::getInstance();
          $src = $this->doc->getElementById($node->getAttribute('overlay'))->nodeValue;
          $src = $this->convert($src);
          Template::addTile('_tpl', $src);
          $i = 1;
          foreach ($records as $record) {
            $content = Template::render('_tpl', $record);
            $html .= ' <div>'.$content.'</div>'."\n";
          }
        }*/
        elseif (is_array($records[0])) {
          $html .= "\n".'<table>'."\n";
          foreach ($records as $arr) {
            $html .= ' <tr>'."\n";
            $arr = (array) $arr;
            foreach ($arr as $v) {
              $html .= '  <td>'.$v.'</td>'."\n";
            }
            $html .= ' </tr>'."\n";
          }
          $html .= '</table>'."\n";
        }
        else {
          $i = 1;
          foreach ($records as $v) {
            $html .= ' <div>'.$v.'</div>'."\n";
          }
        }
      }
      else $html .= $records;
    }
    //    elseif (XML_TEXT_NODE)
    elseif ($node->firstChild) {
      $html .= $this->renderContainer($node);
    }
    $html .= '</div>';
    if (!empty($label)) $html .= '</fieldset>';
    //    $html .= "\n";
    return $html;
  }

  private function renderScrollbox($node) {
    $style = '';
    $id = $node->getAttribute('id');
    $height = $node->getAttribute('height');
    if (!empty($height)) $style .= "height:$height;";
    $width = $node->getAttribute('width');
    if (!empty($width)) $style .= "width:$width;";
    $label = $this->getValue($node->getAttribute('label'));
    $divid = '';
    if (!empty($label)) {
      $html .= '<fieldset style="margin:0 0 9px 0;" id="'.$id.'"><legend>'.$label.'</legend>'."\n";
    }
    else $divid = " id='$id'";
    $html .= "<div style='overflow-y:auto;$style'".$divid.">\n";
    if ($node->hasAttribute('value')) {
      $value = $this->getValue($node->getAttribute('value'));
      if (is_array($value)) {
        $html .= "<table>\n";
        foreach ($value as $arr) {
          $html .= " <tr>\n";
          foreach ($arr as $v) {
            $html .= "  <td>$v</td>\n";
          }
          $html .= " </tr>\n";
        }
        $html .= '</table>'."\n";
      }
      else $html .= $value;
    }
    else {
      $html .= $this->renderContainer($node);
    }
    $html .= '</div>';
    if (!empty($label)) $html .= '</fieldset>';
    return $html;
  }

  private function renderDialogheader($node) {
    // TODO : ferrer à droite
    $title = c($this->getNodeAttribute($node, 'title'));
    return '<div class="dialogheader">'.$title.'</div>';
  }

  private function renderTriggers($node) {
    $html  = '<!-- Triggers -->'."\n";
    $html .= '<script>';
    $html .= $this->renderContainer($node);
    $html .= '</script>'."\n";
    $html .= '<!-- /Triggers -->';
    return $html;
  }

  private function renderTrigger($node) {
    $target = $this->getNodeAttribute($node, 'target');
    if (!in_array($target, array('document', 'window'))) $target = "'".$target."'";
    $event  = $this->getNodeAttribute($node, 'event');
    $action = $this->getNodeAttribute($node, 'action');
    $html = ' if ($('.$target.')) Event.observe('.$target.', "'.$event.'", function(e) { '.$action.' }, false);';
    return $html;
  }

  private function renderFinder($node) {
    $model = $node->hasAttribute('model') ? $node->getAttribute('model') : MODEL;
/*    $params = array('model' => $model,
                    'branch' => $this->branch);
    $finder = Finder::getInstance($params);*/
    $html  = '<div class="finder-container finder-'.$model.'">'."\n";
//    $html .= $finder->asHtml();
    $html .= '<span data-model="'.$model.'"></span>'; // new
    $html .= '</div>';
    $html .= H::trigger('Kwo.Finder.load(this)'); // new
    return $html;
  }

  private function renderPicker($node) {
    $html  = '<div id="picker">'."\n";
    $html .= (string) Picker::getInstance(array('model' => S('ctx')->model, 'open' => 1));
    $html .= '</div>';
    return $html;
  }

  private function renderDeck($node) {
    $id = $node->hasAttribute('id') ? ' id="'.$node->getAttribute('id').'"' : '';
    $html .= '<div'.$id.' class="deck">';
    foreach ($node->childNodes as $child) {
      $child->setAttribute('visible', 'false');
      $classes = array('deck-pane');
      $class = $child->getAttribute('class');
      if (!empty($class)) {
        $classes[] = $class;
      }
      $child->setAttribute('class', join(' ', $classes));
    }
    $html .= $this->renderContainer($node);
    $html .= '</div>';
    return $html;
  }

  private function renderStackpanel($stackpanel) {
    $stack_index = $stackpanel->hasAttribute('index') ? $this->getNodeAttribute($stackpanel, 'index') : 1;
    $i = 1;
    $index_found = false;
    foreach ($stackpanel->childNodes as $panel) {
      $panel_index = $panel->hasAttribute('index') ? $this->getNodeAttribute($panel, 'index') : $i;
      if ($panel_index == $stack_index) {
        $panel->setAttribute('opened', 1);
        $index_found = true;
        break ;
      }
      $i++;
    }
    if ($index_found === false) {
      $stackpanel->firstChild->setAttribute('opened', 1);
    }
    $html  = '';
    $html .= '<div class="stackpanel">'."\n";
    foreach ($stackpanel->childNodes as $panel) {
      $style = '';
      $class = ' opened';
      if (!$panel->hasAttribute('opened')) {
        $style = ' style="display:none;"';
        $class = '';
      }
      $html .= ' <label class="panel-title">'.c($panel->getAttribute('label')).'</label>'."\n";
      $html .= ' <div class="panel'.$class.'"'.$style.'>';
      $html .= $this->renderContainer($panel);
      $html .= ' </div>'."\n";
    }
    $html .= '</div>'."\n";
    $html .= H::trigger('new kwo.managers.Stackpanel(this)');
    return $html;
  }

  private function renderSpacer($node) {
    $style = '';
    $width = $node->getAttribute('width');
    if (!empty($width)) $style .= "width:".$width.";";
    $height = $node->getAttribute('height');
    if ($node->hasAttribute('label')) {
      $label = c($this->getNodeAttribute($node,'label'));
      $html .= '<br/><label># <i>'.$label.'</i></label>'."\n";
      $html .= '<div style="margin:0; padding:0; width:100%; height:12px; margin-top:4px; border-top:#ccc thin dashed;" class="spacer"></div>';
    }
    else {
      if ($node->hasAttribute('line')) {
        if ($height<1) $height = 20;
        $height = round($height / 2);
        $html .= '<div style="margin:0; padding:0; width:100%; height:'.($height+1).'px; margin-top:'.($height).'px; border-top:#ccc thin dashed;" class="spacer"></div>';
      }
      else {
        if (!empty($height)) $style .= "height:$height;";
        $html .= '<div style="'.$style.'" class="spacer"></div>';
      }
    }
    return $html;
  }

  private function renderList($node) {
    $items = $this->getNodeAttribute($node, 'value');
    $html = '<div class="item-list">';
    foreach ($items as $item) {
      if ($item instanceof ActiveRecord) {
        $h = $item->asListHash();
        $html .= '<div data-item="'.$item->getKey().'">';
      }
      else {
        $h = $item;
        $html .= '<div>';
      }
      if (is_array($h)) {
        if (!empty($h['name'])) {
          $html .= '<strong>'.$h['name'].'</strong>';
        }
        $html .= '<ul>';
        foreach ($h as $key => $value) {
          if ($key == 'name') continue ;
          $html .= '<li><span>'.$key.':</span> '.$value.'</li>'."\n";
        }
        $html .= '</ul>';
      }
      else {
        $html .= $h;
      }
      $html .= '<div class="clear"></div></div>';
    }
    $html .= '</div>';
    return $html;
  }

  private function renderList2($node) {
    $style  = 'overflow:auto;';
    $style .= $node->hasAttribute('height') ? 'height:'.$node->getAttribute('height').'px;' : '';
    $html = '<div style="'.$style.'" class="list layout-inline">'."\n";
    $edit_overlay = $add_overlay = '';
    foreach ($node->childNodes as $child) {
      if ($child->nodeName == 'edit') $edit_overlay = $child;
      elseif ($child->nodeName == 'add' && $this->isBuilt($child)) $add_overlay = $child;
    }
    foreach ($this->getNodeAttribute($node, 'data') as $i => $row) {
      $this->ctx->setAttribute('_row', $row);
      $html .= '<div class="'.($i%2===0?'alt':'').'">';
      $html .= $this->renderContainer($edit_overlay);
      $html .= '</div>';
    }
    if ($add_overlay) {
      $html .= '<div class="'.(++$i%2===0?'alt':'').'">';
      $html .= $this->renderContainer($add_overlay);
      $html .= '</div>';
    }
    $html .= '</div>';
    return $html;
  }

  private function renderGrid($node) {
    if ($node->hasChildNodes()) { return $this->renderGridComplex($node); }
    $id = $node->getAttribute('id');
    $label = $this->getNodeAttribute($node, 'label');
    $value = SH::asSet($this->getNodeAttribute($node, 'value'));
    $class = 'grid'.(($node->getAttribute('layout') === 'auto') ? ' auto': '' );

    if (!$node->hasAttribute('cols')) {
      $mapping_arr = null;
      if ($node->hasAttribute('mapping')) {
	      $mapping_arr = $this->getNodeAttribute($node, 'mapping');
      }
      $html  = '<table class="'.$class.' gridsimple">'."\n";
      if (!empty($label)) {
        $html .= ' <thead><tr><th colspan="2">'.c($label).'</th></tr></thead>'."\n";
      }
      $i = 1;
      $style = '';
      if ($node->hasAttribute('height')) {
        $style .= 'height:'.$node->getAttribute('height').'px; max-height:'.$node->getAttribute('height').'px; overflow-y:auto; overflow-x:hidden;';
      }
      $html .= '<tbody style="'.$style.'">'."\n";
      foreach ($value as $k => $v) {
      	$class = '';
      	if ($i++%2 == 0) $class = ' class="alt"';
      	if ($mapping_arr!=null) $key = $mapping_arr[$k];
      	else $key = $k;
      	$html .= ' <tr'.$class.'>'."\n";
        if (!is_int($key)) {
          $html .= '  <td>'.c($key).'</td>'."\n";
        }
        $html .= '  <td>';
        if (is_array($v)) {
          $html .= '<table class="grid">'."\n";
          foreach ($v as $k1 => $v1) {
            $html .= '<tr><td>'.$k1.'</td><td>'.$v1.'</td></tr>'."\n";
          }
          $html .= '</table>'."\n";
        }
        else {
          $html .= $v;
        }
        $html .= '</td>'."\n";
        $html .= ' </tr>'."\n";
      }
      $html .= '</tbody>'."\n";
      $html .= '</table>';
      return $html;
    }

    $html  = '<table class="'.$class.' gridmult">'."\n";
    $col_arr = explode(',',$this->getNodeAttribute($node,'cols'));
    $html  .= '<thead><tr>'."\n";
    foreach ($col_arr as $col) {
      list($name,$width) = explode(':',$col);
      $style = '';
      if (!empty($width)) $style = ' style="width:'.$width.'%;"';
      $html .= '<th'.$style.'>'.$name.'</th>';
    }
    $n = count($col_arr);
    $html  .= '</tr></thead>'."\n";
    $html .= '<tbody>'."\n";
    $i = 0;
    foreach ($value as $line_arr) {
      $class = '';
      if ($i++%2==0) $class = ' class="alt"';
      $html .= '<tr'.$class.'>';
      for ($j=0;$j<$n;$j++) { $html .= '<td>'.$line_arr[$j].'</td>'; }
      $html .= '</tr>'."\n";
    }
    $html .= '</tbody>'."\n";
    $html .= '</table>';
    return $html;
  }

  // avoir notion de H et VNoteBook
  // http://www.adobe.com/aboutadobe/

  //

  private function renderGridComplex($grid) {
    $id = $grid->getAttribute('id');
    $label = $this->getNodeAttribute($grid, 'label');
    $value = $this->getNodeAttribute($grid, 'value');

    $info_arr = SH::asSet($value);
    $class = 'grid';

    if ($grid->firstChild->nodeName != 'cols') {
      return '';
    }
    $cols = $grid->firstChild->childNodes;
    $cols_count = count($cols);
    $mapping_arr = range(0, ($cols_count-1));
    $labels_arr = array();

    $has_width_flag = false;
    $has_labels_flag = false;
    $cols_arr = array();
    $i = 0;
    foreach ($cols as $col) {
      $col_arr = array();
      if ($col->hasAttribute('label')) {
        $col_arr['label'] = $this->getNodeAttribute($col, 'label');
        $has_labels_flag = true;
      }
      if ($col->hasAttribute('width')) {
        $col_arr['width'] = $this->getNodeAttribute($col, 'width');
        $has_width_flag = true;
      }
      $col_arr['key'] = $col->hasAttribute('key') ? $col->getAttribute('key') : $i;
      $cols_arr[] = $col_arr;
      $i++;
    }
    $thead = '';
    if ($has_width_flag===true) {
      $thead .= '<colgroup>'."\n";
      foreach ($cols_arr as $col_arr) {
        $style = '';
        if (strlen($col_arr['width'])>=1) $style = 'width:'.$col_arr['width'].';';
        $thead .= ' <col style="'.$style.'" />'."\n";
      }
      $thead .= '</colgroup>'."\n";
    }
    if ($has_labels_flag===true) {
      $thead  .= '<thead>'."\n";
      foreach ($cols_arr as $col_arr) {
        $thead .= ' <th>'.$col_arr['label'].'</th>';
      }
      $thead .= '</thead>'."\n";
    }

    $tbody  .= '<tbody>'."\n";
    $i = 0;
    foreach ($info_arr as $line_arr) {
      $class = $i++ % 2 == 0 ? ' class="alt"' : '';;
      $tbody .= ' <tr'.$class.'>';
      foreach ($cols_arr as $col_arr) {
          $tbody .= '<td>'.$line_arr[$col_arr['key']].'</td>';
      }
      $tbody .= '</tr>'."\n";
    }
    $tbody .= '</tbody>'."\n";

    $class = $cols_count === 2 ? ' gridsimple' : ' gridmult';
    if ($has_width_flag!==true) $class .= ' auto';
    $html  = '<table class="grid'.$class.'">'."\n";
    $html .= $thead;
    $html .= $tbody;
    $html .= '</table>';
    return $html;
  }

  private function renderNotebook($node) {
    $html .= '<div class="tabbox">'."\n";
    $html .= '<div class="tabs">'."\n";
    $i = 0;
    $skips = array();
    $selected_page_num = 0;

    foreach ($node->childNodes as $page) {
      $i++;
      $tab_label = $page->hasAttribute('label')
                 ? l($this->getNodeAttribute($page, 'label'))
                 : Model::label(MODEL);
      if (!$this->isVisible($page) || !$this->isBuilt($page)) {
        $skips[] = $i;
        continue;
      }
      $pages[] = $i;
      $selected_page_num = $selected_page_num == 0 ? $i : $selected_page_num;
      $class = 'tab';
      if ($i == $selected_page_num) {
        $class .= ' active';
        $selected_page_num = $i;
      }
      $class = ' class="'.$class.'"';

      $onclick = 'Kwo.NoteBook.onSwitchPage(this)';
      if ($page->hasAttribute('onclick')) {
        $onclick .= $page->getAttribute("onclick");
      }
      $onclick = ' onclick="'.$onclick.'"';
      $tab_label = c($tab_label);
      if ($page->hasAttribute('ico')) {
        $tab_label = '<img src="'.$page->getAttribute('ico').'" />'.$tab_label;
      }
      $html .= ' <a href="javascript:void(0)" data-num="'.$i.'"'.$onclick.$class.'>'.$tab_label.'</a>'."\n";
      $tab_labels[$i] = $tab_label;
    }
    $html .= '</div>'."\n";
    $html .= '<div class="tabpanels">'."\n";

    $i = $j = 0;
    foreach ($node->childNodes as $page) {
      $i++;
      if (in_array($i, $skips)) continue;
      $j++;
      $display = ($selected_page_num === $i) ? '' : ' style="display:none;"';
      $html .= '<!-- '.$tab_labels[$i].' -->'."\n";
      $html .= '<div class="tabpanel tabpanel-'.$i.'" '.$display.'>';
      $html .= $this->renderContainer($page);
      $html .= '</div>'."\n";
      $html .= '<!-- /'.$tab_labels[$i].' -->'."\n";
    }
    $html .= '</div>';
    $html .= '</div>';
    return $html;
  }

  private function renderNotebook2($node) {
    $html .= '<table class="notebook">'."\n";
    $html .= '<thead><tr><th>'."\n";
    $i = 0;
    $skips = array();
    $selected_page_num = 0;

    foreach ($node->childNodes as $page) {
      $i++;
      $tab_label = $page->hasAttribute('label')
                 ? $this->getNodeAttribute($page, 'label')
                 : Model::label(MODEL);
      if (!$this->isVisible($page) || !$this->isBuilt($page)) {
        $skips[] = $i;
        continue;
      }
      $pages[] = $i;
      $selected_page_num = $selected_page_num == 0 ? $i : $selected_page_num;
      $class = '';
      if ($i == $selected_page_num) {
        $class = ' class="active"';
        $selected_page_num = $i;
      }

      $onclick = 'Kwo.NoteBook.onSwitchPage(this)';
      if ($page->hasAttribute('onclick')) {
        $onclick .= $page->getAttribute("onclick");
      }
      $onclick = ' onclick="'.$onclick.'"';
      $tab_label = c($tab_label);
      if ($page->hasAttribute('ico')) {
        $tab_label = '<img src="'.$page->getAttribute('ico').'" />'.$tab_label;
      }
      $html .= ' <a href="javascript:void(0)" data-num="'.$i.'"'.$onclick.$class.'>'.$tab_label.'</a>'."\n";
      $tab_labels[$i] = $tab_label;
    }
    $html .= '</th></tr></thead>'."\n";
    $html .= '<tbody><tr><td class="pages">'."\n";
    $i = $j = 0;
    foreach ($node->childNodes as $page) {
      $i++;
      if (in_array($i, $skips)) {
        continue ;
      }
      $j++;
      $display = ($selected_page_num === $i) ? 'block' : 'none';
      $html .= '<!-- '.$tab_labels[$i].' -->'."\n";
      $html .= '<div class="page page-'.$i.'" style="display:'.$display.';">';
      $html .= $this->renderContainer($page);
      $html .= '</div>'."\n";
      $html .= '<!-- /'.$tab_labels[$i].' -->'."\n";
    }
    $html .= '</td></tr></tbody>';
    $html .= '</table>';
    return $html;
  }

  private static $icons = array('exec' => 'fa-caret-right',
                                'send' => 'fa-envelope',
                                'store' => 'fa-check',
                                'storeandadd' => 'fa-plus',
                                'download' => 'fa-download',
                                'destroy' => 'fa-times',
                                'remove' => 'fa-times',
                                'export' => 'fa-upload',
                                'import' => 'fa-download',
                                'search' => 'fa-search',
                                'next' => 'fa-chevron-right',
                                'prev' => 'fa-chevron-left');

  private static $labels = array('exec' => 'exécuter',
                                 'send' => 'envoyer',
                                 'store' => 'enregistrer',
                                 'storeandadd' => ' ',
                                 'download' => 'télécharger',
                                 'destroy' => 'supprimer',
                                 'remove' => 'supprimer',
                                 'export' => 'exporter',
                                 'import' => 'importer',
                                 'search' => ' ',
                                 'next' => ' ',
                                 'prev' => ' ');

  private function renderLauncher($node) {
    if (!$node->hasChildNodes() && !$node->hasAttribute('buttons')) {
      $node->setAttribute('buttons', 'store,destroy');
    }
    if ($node->hasAttribute('buttons')) {
      $buttons = explode(',', $node->getAttribute('buttons'));
      foreach ($buttons as $name) {
        $button = $this->doc->createElement('button');
        $button->setAttribute('name', $name);
        $node->appendChild($button);
      }
    }
//    if (!(Model::isProfile(MODEL) && S('req')->getBranchValue('user_id') >= 1) &&
//        !TreeManager::isNode($item) && $h['disable_home'] != true) {
    if ( /*$_SERVER['REMOTE_ADDR'] == '192.168.0.102' && */
        !in_array(MODEL, array('field', 'resource')) &&
        $this->isEdition() &&
        !TreeManager::isNode(MODEL)) {
      $fc = $node->firstChild;
      $keys = array('search');
      if ($this->ctx->getAttribute('is_loaded') == true) {
        $keys[] = 'storeandadd';
        $keys[] = 'prev';
        $keys[] = 'next';
      }
      foreach ($keys as $key) {
        $button = $this->doc->createElement('button');
        $button->setAttribute('name', $key);
        $node->insertBefore($button, $fc);
      }
    }

    $attrs = array('class' => array('launcher', 'buttons'));
    if ($node->hasAttribute('align')) {
      $attrs['style'] = array('text-align: '.$node->getAttribute('align'));
    }
    $html = AH::asTag('span', $attrs);
    $html .= "\n";

    foreach ($node->childNodes as $button) {
      $name = $button->getAttribute('name');
      if (!$this->isBuilt($button)) continue ;
      $attrs = array('class' => array('button'),
                     'style' => array(),
                     'data' => array('name' => $name),
                     'on' => array());

      $icon = $this->getNodeAttribute($button, 'icon');
      if ($name == 'destroy' && ($this->ctx->id < 1 || ($this->item && !$this->item->isDestroyable()))) continue;

      $label = '';
      if ($button->hasAttribute('tooltip')) {
        $label = $this->getNodeAttribute($button, 'tooltip');
      }
      else {
        $label = $this->getNodeAttribute($button, 'label');
      }

      $action = $button->getAttribute('action');
      if (empty($action)) {
        if ($name == 'store') $action = '.onStore(this)';
        elseif ($name == 'storeandadd') $action = '.onStoreAndAdd(this)';
        elseif ($name == 'destroy') {
          $attrs['data']['confirm'] = l('êtes-vous sûr de vouloir supprimer cet élément ?');
          $action = '.onDestroy(this)';
        }
        elseif ($name == 'prev') $action = '.onPrev(this)';
        elseif ($name == 'next') $action = '.onNext(this)';
        elseif ($name == 'search') $action = '.onHome(this)';
        else $action = '.'.$name.'(this)';
      }

      if ($action[0] == '.') {
        $action = 'Kwo.F(\''.MODEL.'\')'.$action;
      }
      elseif (strpos($action, '/') === false && strpos($action, '(') === false) {
        if (!$button->hasAttribute('model')) {
          $attrs['data']['model'] = MODEL;
        }
        if ($button->hasAttribute('confirm')) {
          $attrs['data']['confirm'] = $button->getAttribute('confirm');
        }
        if ($button->hasAttribute('callback')) {
          $attrs['data']['callback'] = $button->getAttribute('callback');
        }
        if ($button->hasAttribute('container')) {
          $attrs['data']['container'] = $button->getAttribute('container');
        }
        $attrs['data']['action'] = $action;
        $action = 'kwo.ux.onExec(this)';
      }

      if (!$this->isVisible($button)) {
        $attrs['style'][] = 'display:none';
      }

      if ($this->isActive($button) === false) {
        $attrs['on']['click'] = 'void(0)';
        $attrs['class'][] = 'notactive';
      }
      else {
        $attrs['on']['click'] = $action;
      }

      if (empty($label)) $label = l(self::$labels[$name]) ?: $name;
      if (empty($icon)) $icon = self::$icons[$name] ?: 'fa-caret-right';
      if ($name === 'store' && $this->ctx->hasAttribute('is_loaded') && $this->ctx->getAttribute('is_loaded') != true) {
        $icon = 'fa-plus-circle';
      }

      $html .= AH::asTag('span', $attrs) . '<i class="fa ' . $icon . '"></i>' . c($label) . '</span>';
//      $html .= H::elt('input', PIX_URL.'/action/'.$icon.'.png', $attrs);
    }
    $html .= '</span>';
    $html .= H::trigger('new kwo.managers.Launcher(this)');
    return $html;
  }

  private function renderForm($node) {
    $attrs = array('autocomplete' => 'off',
                   'enctype' => 'utf-8',
                   'accept-charset' => 'utf-8',
                   'on' => array('submit' => 'return false;'),
                   'data' => array('xml-file' => $this->xml_file));
    if ($node->hasAttribute('id')) {
      $attrs['id'] = $node->getAttribute('id');
    }
    $html  = '<form '.AH::asAttributes($attrs).'>';
    $html .= $this->renderContainer($node);
    $html .= '</form>';
    return $html;
  }

  private function renderFieldset($node) {
    $attr = array();
    if (!$this->isVisible($node)) {
      $attr['style'] = 'display:none;';
    }
    if ($node->hasAttribute('id')) {
      $attr['id'] = $node->getAttribute('id');
    }
    if (!$node->hasAttribute('label') && $this->isEdition()) {
      $node->setAttribute('label', '$fieldset_label');
      $fc = $node->firstChild;
      if ($fc->nodeName == 'notebook') {
      	if (!$fc->firstChild->hasAttribute('label') &&
      	    !$fc->lastChild->hasAttribute('label')) {
      	  $fc->firstChild->setAttribute('label', l('définition'));
      	  $fc->lastChild->setAttribute('label', l('opérations'));
      	  $fc->lastChild->setAttribute('built', '$is_loaded');
      	}
      }
    }
    $label = $this->getNodeAttribute($node, 'label');
    $html  = '<fieldset '.AH::asAttributes($attr).'>';
    $html .= '<legend>'.c($label).'</legend>';
    $html .= $this->renderContainer($node);
    $html .= '</fieldset>';
    return $html;
  }

/*  private function renderWindow($node) {
    $item = $this->item;
    $fc = $node->firstChild;
    if ($fc->nodeName == 'notebook') {
      if (!$fc->firstChild->hasAttribute('label') &&
          !$fc->lastChild->hasAttribute('label')) {
        $fc->firstChild->setAttribute('label', 'définition');
        $fc->lastChild->setAttribute('label', 'opérations');
        $built = $item->isItem() ? 'true' : 'false';
        $fc->lastChild->setAttribute('built', $built);
      }
    }
    $attributes = array();
    if (!$this->isVisible($node)) {
      $attributes['style'] = 'display:none;';
    }
    if ($node->hasAttribute('id')) {
      $attributes['id'] = $node->getAttribute('id');
    }
    $attributes['class'] = array('window');
    $label = '<ul>';
    $label .= '<li class="bold">'.c(Model::label($item)).'</li>';
    $label .= '<li>';
    $label .= ' <img src="'.PIX_URL.'/action/home-search.png"'
      .' onclick="Kwo.F(\''.$item->getModel().'\').onHome(this);"'
      .' onload="if (!$(this).inModal()) { $(this).show(); }"'
      .' style="display:none;"'
      .' title="retour à liste" />';
    $label .= '</li>';
    if ($item->isItem()) {
      $label .= '<li data-model="'.$item->getModel().'" onclick="kwo.ux.onNavigate(this)" class="prev">&lt;</li>';
      $label .= '<li data-model="'.$item->getModel().'" onclick="kwo.ux.onNavigate(this)" class="next">&gt;</li>';
      $label .= '<li>#'.$item->id.'</li>';
      $label .= '<li>'.$item->getName().'</li>';
    }
    else {
      $label .= '<li>Nouvel élément</li>';
    }
    $label .= '</ul>';
    //    $label = $this->getNodeAttribute($node, 'label');
    $html  = '<div '.AH::asAttributes($attributes).'>'."\n";
    $html .= '<div class="window-toolbar">'.$label.'</div>'."\n";
    $html .= '<div class="window-container">';
    $html .= $this->renderContainer($node);
    $html .= '</div>'."\n";
    $html .= '</div>';
    return $html;
  }*/

  private function renderLink($node) {
    $id = $node->getAttribute('id');
    $icon = $node->getAttribute('icon');
    if ($icon === 'belongsto') {
      $exec = $node->getAttribute('exec');
      list ($model, $action) = explode('.', $exec);
      if ($action == 'edit') {
        if ($this->req->getBranchValue($model.'_id') >= 1) {
          return null;
        }
      }
    }
    $action = $node->getAttribute('action');
    $actions = array('details' => array('label' => l('informations'), 'action' => 'details'),
                     'stats' => array('label' => l('statistiques'), 'action' => 'stats'),
                     'tags' => array('label' => l('tags'), 'action' => 'tags'),
                     'note' => array('label' => l('note'), 'action' => 'note'),
                     'seo' => array('label' => l('SEO'), 'action' => 'seo'),
                     'dashboard' => array('label' => l('dashboard'), 'action' => 'dashboard'),
                     'geocode' => array('label' => l('géocode'), 'action' => 'geocode'),
                     'interest' => array('label' => l('thèmes'), 'action' => 'interest'),
                     'letter' => array('label' => l('courrier'), 'action' => 'letter'),
                     'timeline' => array('label' => l('timeline'), 'action' => 'timeline'),
                     'metrics' => array('label' => l('métriques'), 'action' => 'metrics'),
                     'conversions' => array('label' => l('conversions'), 'action' => 'conversions'),
                     'comments' => array('label' => l('commentaires'), 'action' => 'comments', 'id' => 'comments-button'));
    if (empty($action)) {
      if ($actions[$icon]) {
        if ($actions[$icon]['ignored'] === true) return '';
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', $actions[$icon]['label']);
        }
        $node->setAttribute('action', $actions[$icon]['action']);
        if ($actions[$icon]['id']) {
          $node->setAttribute('id', $actions[$icon]['id']);
        }
      }
      elseif ($icon === 'contacts') {
        $node->setAttribute('label', l('contacts'));
        $node->setAttribute('action',
                            'Kwo.F(\'contact\').onHome(this)');
        $deck = $this->doc->getElementsByTagName('deck')->item(0);
        $box = $this->doc->createElement('box');
        $box->setAttribute('id', 'contact-box');
        $box->setAttribute('class', 'contact-box');
        $deck->appendChild($box);
      }
      elseif ($icon === 'announcements') {
        $node->setAttribute('label', l('messages'));
        $node->setAttribute('action', 'Kwo.F(\'announcement\').onHome(this)');
        $deck = $this->doc->getElementsByTagName('deck')->item(0);
        $box = $this->doc->createElement('box');
        $box->setAttribute('id', 'announcement-box');
        $box->setAttribute('class', 'announcement-box');
        $deck->appendChild($box);
      }
      elseif ($icon === 'duplicate') {
        $node->setAttribute('label', l('dupliquer'));
        $node->setAttribute('confirm', l('êtes-vous sûr de vouloir dupliquer cet élément ?'));
        $node->setAttribute('action', '.onDuplicate(this)');
      }
      elseif ($icon === 'preview') {
        $node->setAttribute('label', l('aperçu'));
        $node->setAttribute('action', '.onPreview(this)');
        if ($this->item) {
          $node->setAttribute('data-item', $this->item->getKey());
        }
      }
      elseif ($icon === 'add') {
        $node->setAttribute('label', l('ajouter'));
        $node->setAttribute('action', '.onEdit(this)');
        $node->setAttribute('data-id', '0');
      }
      elseif ($icon === 'hasmany') {
        $model = strtolower($node->getAttribute('model'));
        $relation = Model::parameter(MODEL, 'has_many');
/*        if (isset($relation[$model]['through'])) {
          $node->setAttribute('action',
                              "Kwo.F('".MODEL."').onExec('associations', this, {container:'".$model."-box'})");
          $node->setAttribute('id', $model.'-button');
          $node->setAttribute('values', '{"model":"'.$model.'", "append":1}');
        }
        else {*/
          $node->setAttribute('action', "Kwo.F('".$model."').onHome(this)");
/*        }*/
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', Model::parameter($model, 'label').'(s)');
        }
        $deck = $this->doc->getElementsByTagName('deck')->item(0);
        $box = $this->doc->createElement('box');
        $box->setAttribute('class', $model.'-box');
        $deck->appendChild($box);
      }
      elseif ($icon === 'truncate') {
        $node->setAttribute('label', l('vider'));
        $node->setAttribute('action', '.onExec(\'truncate\', this, {confirm: \''.l('êtes-vous sûr ?').'\', callback: true})');
      }
      else {
        $node->setAttribute('label', l($this->getNodeAttribute($node, 'label')));
      }
    }
    elseif ($node->hasAttribute('label')) {
      $node->setAttribute('label', l($this->getNodeAttribute($node, 'label')));
    }
    $label = $this->getNodeAttribute($node, 'label');
    $action = $node->getAttribute('action');
    if (strpos($action, '/') === false && strpos($action, '(') === false) {
      if (!$node->hasAttribute('model')) {
        $node->setAttribute('model', MODEL);
      }
      $node->setAttribute('data-action', $action);
      $action = 'kwo.ux.onExec(this)';
      if (!$node->hasAttribute('container') && !$node->hasAttribute('callback')) {
        $node->setAttribute('container', 'deck');
      }
    }
    elseif (strpos($action, '/') !== false || strpos($action, '(') === false) {
      if (strpos($action, '/') === false) {
        $action = '/back/' . $this->req->getExtension() . '/' . $action;
      }
      $node->setAttribute('data-action', $action);
      $action = 'Kwo.exec(this)';
    }
    elseif ($action[0] == '.') {
      $action = 'Kwo.F(\''.MODEL.'\')'.$action;
    }
    $icon = $node->hasAttribute('icon') ? $node->getAttribute('icon') : 'run';
    $div_style = $img_style = '';
    if ($node->hasAttribute('align') && $node->getAttribute('align') === 'right') {
      $div_style = ' style="text-align:right;"';
      $img_style = ' style="margin:3px 1px 3px 5px; vertical-align:middle;"';
      $right_flag = true;
    }
    $html  = '<div'.$div_style.' class="link">'."\n";
    $id = $node->hasAttribute('id') ? ' id="'.$node->getAttribute('id').'"' : '';
    $html .= ' <a href="javascript:void(0)"'.$id."\n";
    if ($node->hasAttribute('confirm')) {
      $confirm = $node->getAttribute('confirm');
      if ($confirm == 'true') {
        $confirm = 'êtes-vous sûr ?';
      }
      $html .= '    data-confirm="'.l($confirm).'"';
    }
    if ($node->hasAttribute('values')) {
      $str = $node->getAttribute('values');
      // TODO : A REGLER PLUS PROPREMENT
      $str = str_replace("'", '"', $str);
      $str = htmlspecialchars($str);
      $html .= ' data-values="'.$str.'"';
    }
    if ($node->hasAttribute('data-id')) {
      $html .= ' data-id="'.$node->getAttribute('data-id').'"';
    }
    if ($node->hasAttribute('data-item')) {
      $html .= ' data-item="'.$node->getAttribute('data-item').'"';
    }
    if ($node->hasAttribute('data-action')) {
      $html .= ' data-action="'.$node->getAttribute('data-action').'"';
    }
    if ($node->hasAttribute('model')) {
      $html .= ' data-model="'.$node->getAttribute('model').'"';
    }
    if ($node->hasAttribute('container')) {
      $html .= ' data-container="'.$node->getAttribute('container').'"';
    }
    if ($node->hasAttribute('callback')) {
      $html .= ' data-callback="'.$node->getAttribute('callback').'"';
    }
    $html .= ' class="button-link"';
    $html .= ' onclick="'.$action.'">';
    if ($right_flag != true) $html .= '<img src="'.PIX_URL.'/action/'.$icon.'.png"'.$img_style.' />';
    $html .= c($label);
    if ($right_flag === true) $html .= '<img src="'.PIX_URL.'/action/'.$icon.'.png"'.$img_style.' />';
    $html .= '</a>'."\n";
    $html .= '</div>';
    return $html;
  }

  private function renderHbox($node) {
    return $node->firstChild->nodeName === 'vbox'
         ? $this->renderTable($node)
         : $this->renderContainer($node);
  }

  private function renderTable($hbox) {
    $class = 'hbox';
    if ($hbox->hasAttribute('class')) {
      $class .= ' '.$hbox->getAttribute('class');
    }
    $html = '<div class="hbox"><table class="'.$class.'"><tbody><tr>'."\n";
    $vboxes = $hbox->childNodes;
    foreach ($vboxes as $vbox) {
      $style = $vbox->hasAttribute('width')
             ? ' style="width:'.$vbox->getAttribute('width').';"'
             : '';
      $html .= '<td'.$style.'>';
      if ($vbox->hasAttribute('label')) {
        $html .= '<div class="hbox-label">'.c(l($this->getNodeAttribute($vbox, 'label'))).'</div>'."\n";
      }
      $html .= $this->renderContainer($vbox);
      $html .= '</td>'."\n";
    }
    $html .= '</tr></tbody></table></div>';
    return $html;
  }



  /***********************************************

   ELEMENTS

  ************************************************/


  private function renderElem($node) {
    $this->elt_num++;
    $field = null;
    if ($node->hasAttribute('field')) {
      $field = $node->getAttribute('field');
      if (!$node->hasAttribute('name')) {
        $node->setAttribute('name', 'record['.$field.']');
      }
      if (!$node->hasAttribute('value')) {
        $node->setAttribute('value', '$'.$field);
      }
    }
    if ($node->hasAttribute('relation')) {
      $model = $node->getAttribute('relation');
      if (!$node->hasAttribute('label')) {
        $node->setAttribute('label', Model::label($model));
        $node->setAttribute('precision', l('association'));
      }
      $node->setAttribute('type', 'checkbox');
      $node->setAttribute('cols', ($node->hasAttribute('cols') ? $node->getAttribute('cols') : '3'));
      $node->setAttribute('name', 'relation['.$model.']');
      if (!$node->hasAttribute('options')) {
        $node->setAttribute('options', 'Model::asHash(\''.$model.'\')');
      }
      if (!$node->hasAttribute('value') && $this->ctx->item->isItem()) {
        $node->setAttribute('value', '$item->getItems(\''.$model.'\')');
      }
    }
    if ($node->hasAttribute('type')) {
      $type_evaluated = $this->getNodeAttribute($node, 'type');
      $type = $node->getAttribute('type');
      if ($type != $type_evaluated) {
        $node->setAttribute('type', $type_evaluated);
        $type = $type_evaluated;
      }
      if ($type == 'yesno' && $field == 'status' && !$node->hasAttribute('label')) {
        $node->setAttribute('label', 'en ligne');
      }
      if ($type === 'select' && !$node->hasAttribute('options')) {
        if (substr($field, -3) == '_id' && Model::exists(substr($field, 0, -3))) {
          $model = substr($field, 0, -3);
          $node->setAttribute('options', 'Model::asHash('.$model.')');
        }
      }
      if ($type === 'period') {
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', 'période');
        }
        if ($node->hasAttribute('fields') && !$node->hasAttribute('field')) {
          $parts = explode(',', $node->getAttribute('fields'));
          $node->setAttribute('field', trim($parts[1]));
        }
      }
    }
    else {
      $value = $node->getAttribute('value');
      if ($value === '$locale') {
        $type = 'lang';
      }
      elseif ($value === '$image' || $value === '$thumbnail' || $value === '$picture' || $value === '$icon') {
        $type = 'image';
      }
      elseif ($value === '$about') {
        if (!$node->hasAttribute('rows')) {
          $node->setAttribute('rows', 3);
        }
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', 'à propos');
        }
        $type = 'text';
      }
      elseif ($value === '$country_id') {
        $type = 'select';
        $node->setAttribute('label', 'pays');
        $node->setAttribute('options', 'I18N::getCountries()');
      	if ($this->ctx->getAttribute('country_id') < 1) {
      	  $this->ctx->setAttribute('country_id', Country::FR);
      	}
      }
      elseif ($value === '$app_id') {
        $type = 'select';
        $node->setAttribute('label', 'app associée');
        $node->setAttribute('options', 'App::withId()');
      }
      elseif ($value === '$flags') {
        $type = 'checkbox';
        if (!$node->hasAttribute('cols')) {
          $node->setAttribute('cols', '3');
        }
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', 'flags');
        }
        if (!$node->hasAttribute('options') && property_exists(MODEL, 'flags')) {
          $node->setAttribute('options', c(MODEL).'::$flags');
        }
      }
      elseif ($value === '$author_id') {
        $type = 'select';
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', 'auteur');
        }
        if (!$node->hasAttribute('options')) {
          $node->setAttribute('options', 'Author::asHash()');
        }
      }
      elseif ($value === '$published_on') {
        $type = 'date';
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', 'date de publication');
        }
      }
      elseif ($value === '$locales') {
        $type = 'checkbox';
        if (!$node->hasAttribute('options')) {
          $this->ctx->_locales = self::locales();
          $node->setAttribute('options', '$_locales');
        }
        $n = count($this->ctx->_locales) ?: 4;
        if (!$node->hasAttribute('cols')) {
          $node->setAttribute('cols', $n);
        }
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', 'traductions disponibles');
        }
      }
      elseif ($value === '$status') {
        $has_options = property_exists(MODEL, 'statuses') || $node->hasAttribute('options');
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', $has_options ? 'état' : 'en ligne');
        }
        if ($has_options === true) {
          $type = 'radio';
          if (!$node->hasAttribute('options')) {
            $node->setAttribute('options', c(MODEL).'::$statuses');
          }
        }
        else {
          $type = 'yesno';
        }
      }
      elseif ($value === '$type') {
        $type = 'radio';
        $node->setAttribute('label', 'type');
        $node->setAttribute('options', c(MODEL).'::$types');
      }
      elseif ($field === 'item') {
        $type = 'item';
        if (!$node->hasAttribute('label')) {
          $node->setAttribute('label', 'élément associé');
        }
      }
      elseif (substr($field, -3) == '_id' && Model::exists(substr($field, 0, -3))) {
        if ($this->branch[$field] > 0 || $this->branches[$field] > 0) return '';
        $model = substr($field, 0, -3);
        if (!$node->hasAttribute('label')) {
          $suffix = $field == 'user_id' ? 'associé' : 'associé(e)';
          $node->setAttribute('label', Model::label($model).' '.$suffix);
        }
        if ($node->hasAttribute('options')) {
          $type = 'select';
          //          $node->setAttribute('options', 'Model::asHash('.$model.')');
        }
        else {
          $type = 'item';
          $node->setAttribute('model', $model);
        }
      }
      else {
        $casts = (array) Model::parameter(MODEL, 'casts');
        if ($casts[$field] == 'bits') {
          if (!$node->hasAttribute('cols')) {
            $node->setAttribute('cols', '3');
          }
          if (!$node->hasAttribute('label')) {
            $node->setAttribute('label', $field);
          }
          if (!$node->hasAttribute('options')) {
            $node->setAttribute('options', MODEL.'::$'.$field);
          }
          $type = 'checkbox';
        }
        elseif ($casts[$field] == 'date') {
          $type = 'date';
        }
        elseif ($casts[$field] == 'datetime') {
          $type = 'datetime';
        }
        if (empty($type)) {
          $type = $node->hasAttribute('options') ? 'select' : 'text';
        }
      }
      $node->setAttribute('type', $type);
    }

    if ($node->hasAttribute('options')) {
      $options = $node->getAttribute('options');
      if (strpos($options, '::$') !== false) {
        $node->setAttribute('options', 'array_map("l", '.$options.')');
      }
    }

    if (!$node->hasAttribute('name') && $node->hasAttribute('value')) {
      $value = substr($node->getAttribute('value'), 1);
      if (strpos($value, '[') !== false) {
        $pair = explode('[', $value);
        $node->setAttribute('name',
                            'record['.$pair[0].']['.substr($pair[1], 0, -1).']');
      }
      else {
        $node->setAttribute('name', 'record['.$value.']');
      }
    }

    if ($node->getAttribute('type') === 'relations' && !$node->hasAttribute('label')) {
      $node->setAttribute('label', 'élément(s) lié(s)');
    }

    if ($node->getAttribute('type') === 'acl' && !$node->hasAttribute('label')) {
      $node->setAttribute('label', 'responsable(s)');
    }

    $label = $this->getNodeAttribute($node, 'label');
    if (!$node->hasAttribute('label') &&
        in_array($node->getAttribute('type'),
                 array('file', 'image', 'select', 'text', 'yesno', 'radio',
                       'checkbox', 'date', 'datetime', 'resources'))) {
      if ($node->hasAttribute('value')) {
        $label = ltrim($node->getAttribute('value'), '$');
      }
      elseif ($node->hasAttribute('name')) {
        $label = $node->getAttribute('name');
      }
      $labels = array('name' => 'nom',
                      'content' => 'contenu',
                      'title' => 'titre',
                      'priority' => 'priorité',
                      'value' => 'valeur');
      if ($labels[$label]) {
        $label = l($labels[$label]);
      }
      else {
        if (substr($label, -3) == ' id') {
          $label = substr($label, 0, - 3);
        }
        else {
          $label = str_replace('_', ' ', $label);
        }
      }
      $node->setAttribute('label', $label);
    }
    elseif ($type === 'lang') {
      $label = l('langue');
    }
    else {
      $label = l($label);
    }

    if ($node->hasAttribute('hiddenifnotnull')) {
      $value = $this->getNodeAttribute($node, 'value');
      if (!empty($value)) {
        $type = 'hidden';
        $node->setAttribute('type', $type);
      }
    }
    if ($type != 'hidden' && $type != 'biselect' && $type != 'link' && $type != 'singleoption' &&
        $type != 'button' && $type != 'action' && !empty($label)) {
      $title = $node->getAttribute('tooltip');
      if (!empty($title)) $title = ' title="'.$title.'"';
      $html .= '<label class="elt"'.$title.'>';
      $html .= c($label);
      if ($node->hasAttribute('precision')) {
        $html .= ' &middot; <small>'.l($node->getAttribute('precision')).'</small>';
      }
      if ($node->getAttribute('required') == "true") {
        $html .= '<span title="'.l('champ obligatoire').'">*</span>';
      }
      $html .= '</label>'."\n";
    }

    $method = 'render'.ucfirst($type);
    if ($method == 'renderGrid') {
      $method = 'renderElemGrid';
    }

    $ret = in_array($type, self::$elems)
         ? call_user_func(array($this, $method), $node)
         : '['.$type.'] element not implemented<hr/>';

    if (empty($ret)) return '';

    $ret = $ret === null ? '' : $html.$ret;
    if ($type == 'hidden') return $ret;
    if ($node->hasAttribute('mode') && $node->getAttribute('mode') === 'rich') {
      $type = 'richtext';
    }
    $attributes = array('class' => array('elem', 'elem-'.$type));
    if ($node->hasAttribute('id')) {
      $attributes['id'] = $node->getAttribute('id');
    }
    $content = "\n".$ret."\n";
    return self::Node('div',
                      array('attributes' => $attributes,
                            'content' => $content));
  }

  public static function Node($tag, $opts=null) {
    $elt = $tag;
    $opts = (array) $opts;
    if (!empty($opts['attributes'])) {
      $elt = $tag.' '.AH::asAttributes($opts['attributes']);
    }
    return join("\n", array('<'.$elt.'>',
                            $opts['content'],
                            '</'.$tag.'>'));
  }

  private function renderAcl($node) {
    $html = '';
    $attrs = array('class' => array('item-attach-button'));
    $img = H::elt('img', PIX_URL.'/action/item.add.png', $attrs);
    $html .= '<div class="header">'.$img.'<div class="clear"></div></div>';
    $classes = array('rows');
    $attrs = array('class' => $classes,
                   'style' => array());
    if ($node->hasAttribute('height')) {
      $attrs['style']['height'] = $node->getAttribute('height');
    }
    try {
      $inner = '';
      $inner .= ACL::asPermissionRows($this->item);
      $inner .= H::trigger('new kwo.managers.Acl(this)');
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
    $html .= H::elt('div', $inner, $attrs);
    $attrs = array('class' => 'acl',
                   'data' => array('item' => $this->item->getKey(),
                                   'destroy-confirm' => l('êtes-vous sûr ?')));
    return H::elt('div', $html, $attrs);
  }

  private function renderRelations($node) {
    $models = $node->hasAttribute('model')
            ? $node->getAttribute('model')
            : $this->getNodeAttribute($node, 'models');

    $table = $node->getAttribute('table');
    $model_table = false;
    if (strpos($models, ',') === false) {
      $relation = Model::relation($this->item->getModel(), $models);
      if (empty($table)) {
        if ($relation['through']) {
          $table = $relation['through'];
        }
        else {
          $table = T($models);
          $model_table = true;
        }
      }
    }
    else {
      $table = $table ?: T($this->item).'_item';
    }

    $classes = array('rows');
    if (S('dbo')->getTable($table)->hasField('position')) {
      $classes[] = 'sortable';
    }
    $edition = $node->getAttribute('edition');
    // TODO : supprimer editable
    if ($node->hasAttribute('editable') || $edition == 'row') {
      $classes[] = 'edition-row';
    }
    else {
      $classes[] = 'edition-item';
    }
    if ($model_table === false) {
      $classes[] = 'pickable';
    }
    $html = '';
    $attrs = array('data' => array('selection' => $node->getAttribute('selection'),
                                   'models' => $models),
                   'class' => array('icon-plus-sign-alt', 'item-attach-button'));
    if ($this->item) {
      $attrs['data']['caller-item'] = $this->item->getKey();
    }
    $btn = H::elt('span', '', $attrs);
    $html .= '<div class="header">'.$btn.'<div class="clear"></div></div>';
    if (!$node->hasAttribute('selection')) {
      $node->setAttribute('selection', 'single');
    }
    if ($node->hasAttribute('rows') && !$node->hasAttribute('height')) {
      $node->setAttribute('height', ($node->getAttribute('rows') * 25).'px');
    }
    $style = $node->hasAttribute('height')
           ? ' style="height:'.$node->getAttribute('height').';"'
           : '';
    $html .= '<div class="'.join(' ', $classes).'"'.$style.'>'."\n";
    try {
      $html .= Item::asRelationRows($this->item, $models, $table);
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
    $html .= H::trigger('new kwo.managers.Relation(this)');
    $html .= '</div>'."\n";
    $datas = array('table' => $table,
                   'item' => $this->item->getKey(),
                   'destroy-confirm' => l('êtes-vous sûr ?'));
    if ($node->hasAttribute('callback')) {
      $datas['callback'] = $node->getAttribute('callback');
    }
    $attrs = array('class' => 'relations',
                   'data' => $datas);
    $html = H::elt('div', $html, $attrs);
    return $html;
  }

  private function renderElemGrid($node) {
    $rows = $this->getNodeAttribute($node, 'rows');
    if (!is_array($rows)) {
      $rows = array();
      S('log')->trace('invalid input ['.$this->xml_file.']'.
                      'elem [field='.$node->getAttribute('field').']',
                      __METHOD__);
    }
    $html = '<table class="vbox"><tbody>';
    foreach ($rows as $row) {
      $html .= '<tr>';
      $html .= '<td>'.c($row['label']).'</td>';
      $html .= '<td>';
      $html .= '<select name="'.$row['name'].'">';
      $html .= H::options($row['options'], $row['value']);
      $html .= '</select>';
      $html .= '</td>';
      $html .= '</tr>'."\n";
    }
    $html .= '</tbody></table>';
    return $html;
  }

  private function renderResources($node) {
    $filter = '';
    if ($node->hasAttribute('filter')) {
      $filter = $node->getAttribute('filter');
    }
    $path = '';
    if ($node->hasAttribute('path')) {
      $path = $this->getNodeAttribute($node, 'path');
    }
    $html  = '';
    $html .= '<div class="dropzone" data-directory="" data-filter="'.$filter.'" data-script="/back/core/resource.upload"></div>';
    $html .= '<div class="file-select"><span class="icon-cloud-upload" data-directory="" data-path="'.$path.'" data-filter="'.$filter.'" data-script="/back/core/resource.upload"></span></div>';
    $style = $node->hasAttribute('height')
           ? ' style="height:'.$node->getAttribute('height').';"'
           : '';
    $html .= '<div class="list"'.$style.'>'."\n";
    $html .= Resource::asRows($this->item);
    $html .= H::trigger('new kwo.managers.Resource(this)');
    $html .= '</div>'."\n";
    return '<div class="resources" data-destroy-confirm="'.l('êtes-vous sûr ?').'" data-options="{hidden:true}">'.$html.'</div>';
  }

  private function renderFolder($node) {
    $name = ' name="'.$node->getAttribute('name').'"';
    $value = $this->getNodeAttribute($node, 'value');
    if (strlen($value) < 1 || !preg_match('#^/'.USR_PATH.'#', $value)) {
      $onclick = 'onclick="new kwo.dialogs.FolderManager(this)"';
    }
    $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
    $html  = '<input type="text"'.$name.' value="'.$value.'"'.$disabled."\n";
    $html .= '  '.$onclick.' />';
    return $html;
  }

  private function renderDatetime($node) {
    return $this->renderDate($node);
  }

  private function renderDate($node) {
    $has_time = $node->getAttribute('type') == 'datetime';
    $name = ' name="'.$node->getAttribute('name').'"';
    $value = $this->getNodeAttribute($node, 'value');
    if (strpos($value, '-') !== false) {
      $value = $has_time ? H::datetime($value) : H::date($value);
    }
    $value = ' value="'.$value.'"';
    $class = '';
    $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
    $opts = '';
    if ($has_time) {
      $opts = ', {has_time: true}';
    }

    // TODO : GESTION DU DISABLE ET DU READONLY !!

    if ($has_time === true) {
      $onclick = ' onclick="kwo.ux.onInputDatetimeClick(event)"';
    }
    else {
      $onclick = ' onclick="kwo.ux.onInputDateClick(event)"';
    }

    $html  = '<div class="input-selector"'.$onclick.'>';
    $html .= H::elt('a', self::RESET_CHAR,
                    array('title' => 'vider',
                          'class' => 'reset',
                          'tabindex' => '-1'));
    $html .= '<input type="text"'.$class.$name.$value.$disabled.' />';
    $html .= '<a href="javascript:void(0)" tabindex="-1">'.self::SELECT_CHAR.'</a>';
    $html .= '</div>';
    return $html;
  }

  private function renderDate2($node) {
    $has_time = $node->getAttribute('type') == 'datetime';
    $name = ' name="'.$node->getAttribute('name').'"';
    $value = $this->getNodeAttribute($node, 'value');
    if (strpos($value, '-') !== false) {
      $value = $has_time ? H::datetime($value) : H::date($value);
    }
    $value = ' value="'.$value.'"';
    $class = '';
    $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
    $opts = '';
    if ($has_time) {
      $opts = ', {has_time: true}';
    }
    $onclick = ' onclick="new Kwo.Datepicker($(this).previous()'.$opts.')"';
    $html  = '<div class="input-selector">';
    $html .= H::elt('a', self::RESET_CHAR,
                    array('title' => 'vider',
                          'tabindex' => '-1',
                          'on' => array('click' => 'kwo.ux.onInputReset(this)')));
    $html .= '<input type="text"'.$class.$name.$value.$disabled.' />';
    $html .= '<a href="javascript:void(0)" tabindex="-1"'.$onclick.'>'.self::SELECT_CHAR.'</a>';
    $html .= '</div>';
    return $html;
  }

  private function renderPeriod($node) {
    list ($f, $t) = explode(',', $node->getAttribute('fields'));
    $from = array('field' => $f,
                  'value' => $this->ctx->getAttribute($f),
                  'name' => 'record['.$f.']');
    $from['value'] = DH::isNull($from['value']) ? '' : H::date($from['value']);
    $to = array('field' => $t,
                'value' => $this->ctx->getAttribute($t),
                'name' => 'record['.$t.']');
    $to['value'] = DH::isNull($to['value']) ? '' : H::date($to['value']);
    $parts = array();
    $attrs = array('title' => l('vider'),
                   'class' => 'reset',
                   'tabindex' => '-1');
    $parts[] = H::elt('a', self::RESET_CHAR, $attrs);
    $attrs = array('type' => 'text',
                   'class' => 'from',
                   'name' => $from['name']);
    $parts[] = H::elt('input', $from['value'], $attrs);
    $attrs = array('tabindex' => -1);
    $parts[] = H::elt('span', '&gt;', $attrs);
    $attrs = array('type' => 'text',
                   'class' => 'to',
                   'name' => $to['name']);
    $parts[] = H::elt('input', $to['value'], $attrs);
    $attrs = array('title' => l('sélectionner'),
                   'tabindex' => '-1');
    $parts[] = H::elt('a', self::SELECT_CHAR, $attrs);
    $attrs = array('class' => 'input-selector',
                   'on' => array('click' => 'kwo.ux.onInputPeriodClick(event)'));
    $html = H::elt('div', join("\n", $parts), $attrs);
    return $html;
  }

  private function renderImage($node) {
    $name = ' name="'.$node->getAttribute('name').'"';
    $filepath = $this->getNodeAttribute($node, 'value');

    $html  = '<div class="input-selector">';
    $html .= H::elt('a', self::RESET_CHAR,
                    array('on' => array('click' => 'kwo.ux.onInputReset(this)')));

    $onkeyup = ' onkeyup="kwo.ux.onInputChange(this)"';
    $html .= '<input type="hidden"'.$name.' value="'.$filepath.'" disabled="disabled" />';
    $html .= '<input type="text" value="'.$filepath.'"'.$onkeyup.' />';

    $opts = array();
    if ($node->hasAttribute('path')) {
      $opts[] = "path:'".$this->getNodeAttribute($node, 'path')."'";
    }
    elseif ($node->hasAttribute('folder')) {
      $opts[] = "path:'".$node->getAttribute('folder')."'";
    }

    $html .= H::elt('a', self::SELECT_CHAR,
                    array('on' => array('click' => 'kwo.ux.onInputSelect(this, {'.join(',', $opts).'})')));
    $html .= '</div>';
    $class = ' class="elt-image"';
    $style = $node->hasAttribute('style') ? ' style="'.$node->getAttribute('style').'"' : '';
    $filepath = !empty($filepath) && File::exists($filepath) ? $filepath : PIX_URL.'/unknown.gif';
    if ($filepath[0] != '/') {
      $filepath = '/'.$filepath;
    }
    $src = ' src="'.$filepath.'"';
    $html .= '<img'.$src.$class.$style.' onclick="if (this.src.endsWith(\'unknown.gif\')) return; new kwo.dialogs.BitmapEditor(this)" />';
    return $html;
  }

  private function renderFile($node) {
    $attrs = array('name' => $node->getAttribute('name'),
                   'type' => 'text',
                   'value' => $this->getNodeAttribute($node, 'value'));
    if ($this->isDisabled($node)) {
      $attrs['disabled'] = 'disabled';
    }
    if (in_array($node->getAttribute('name'), array('ico', 'image', 'thumbnail'))) {
      $opts[] = "filter:'image'";
    }
    if (strlen($attrs['value']) < 1 || !preg_match('#^/'.USR_PATH.'#', $attrs['value'])) {
      $opts = array();
      $opts[] = "filter:'".$node->getAttribute('filter')."'";
      if ($node->hasAttribute('path')) {
        $opts[] = "path:'".$this->getNodeAttribute($node, 'path')."'";
      }
      elseif ($node->hasAttribute('folder')) {
        $opts[] = "path:'".$node->getAttribute('folder')."'";
      }
      $opts = join(',', $opts);
      $onclick = 'onclick="new kwo.dialogs.FileManager($(this).previous(), {'.$opts.'})"';
    }
    $html = '<div class="input-selector file-selector">';
    $html .= H::elt('a', self::RESET_CHAR,
                    array('title' => 'vider',
                          'tabindex' => '-1',
                          'on' => array('click' => 'kwo.ux.onInputReset(this)')));
    $html .= '<input '.AH::asAttributes($attrs).'/>';
    $html .= '<a href="javascript:void(0)"'.$onclick.' tabindex="-1">'.self::SELECT_CHAR.'</a>';
    $html .= '</div>';
    return $html;
  }

  private function renderGeo() {
    $html = '<fieldset><legend>'.l('Géolocalisation').'</legend>'."\n";
    $html .= '<table class="layout" style="margin-top:4px;"><tr>'."\n";
    foreach (array('latitude', 'longitude', 'zoom') as $i => $input) {
      $style = $i ? ' style="padding-left:6px;"' : '';
      $value = $this->ctx->getAttribute($input);
      $html .= ' <td'.$style.'><label>'.c($input).'</label><br/>';
      $html .= '  <input type="text" id="record['.$input.']" name="record['.$input.']" value="'.$value.'" class="back '.$input.'"'."\n";
      $html .= '         onclick="new Kwo.Geolocpicker(this)" />'."\n";
    }
    $html .= '</td></tr></table>';
    $html .= '</fieldset>';
    return $html;
  }

  private function renderYesno($node) {
    $name = $node->getAttribute('name');
    $value = $this->getNodeAttribute($node, 'value');
    $value = strlen($value) >= 1 ? (int) $value : $value;
    if ($this->isDisabled($node)) $disabled = ' disabled="disabled"';
    $class = 'options inline';
    if ($value != 1 && $node->hasAttribute('warn')) $class .= ' optionsWarning';
    $html  = '<div class="'.$class.'">'."\n";
    $html .= ' <label>';
    $html .= '<input type="radio" name="'.$name.'" value="1"'.($value === 1 ? ' checked="checked"' : '').$disabled;
    $html .= ' />oui</label>'."\n";
    $html .= ' <label>';
    $html .= '<input type="radio" name="'.$name.'" value="0"'.($value === 0 ? ' checked="checked"' : '').$disabled;
    $html .= ' />non</label>'."\n";
    $html .= '</div>';
    return $html;
  }

  private function renderRadio($node) {
    if (!$node->hasAttribute('options')) return '';
    $html = '';
    $options = (array) $this->getNodeAttribute($node, 'options');
    $value = $this->getNodeAttribute($node, 'value');
    $value = strlen($value) >= 1 ? (int) $value : $value;
    $name = ' name="'.$node->getAttribute('name').'"';
    if ($this->isReadonly($node) === true) {
      $html .= '<input type="hidden"'.$name.' value="'.$value.'" />'."\n";
      $disabled = ' disabled="disabled"';
    }
    elseif ($this->isDisabled($node)) {
      $disabled = ' disabled="disabled"';
    }
    $html .= '<div class="options">'."\n";
    foreach ($options as $key => $val) {
      $checked = ($key == $value || count($options) === 1) && $value !== null ? ' checked="checked"' : '';
      $html .= ' <label class="click">';
      $html .= '<input type="radio"'.$name.' value="'.$key.'"'.$checked.$readonly.$disabled.' />';
      $html .= c($val).'</label>'."\n";
    }
    $html .= '</div>';
    return $html;
  }

  private function renderLocales($node) {
    $values = $this->getNodeAttribute($node, 'value');
    $locales = NH::asSet($values);
    $name = $node->getAttribute('name');
    $cels = array();
    $html = '<div class="locales">'."\n";
    $html .= '<span class="icon-wrench" onclick="new kwo.dialogs.TranslationManager(this)" data-item="'.$this->item->getKey().'"></span>';
    foreach ($this->getNodeAttribute($node, 'options') as $locale => $code) {
      $checked = '';
      if (in_array($locale, $locales)) {
        $checked = ' checked="checked"';
      }
      $cels[] = '<label><input type="checkbox" name="'.$name.'['.$locale.']" value="'.$locale.'"'.$checked.' />'.$code.'</label>';
    }
    $html .= join(' ', $cels);
    $html .= '</div>';
    return $html;
  }

  private function renderCheckbox($node) {
    if ($node->hasChildNodes()) {
      return $this->renderCheckbox2($node);
    }
    $name = $node->getAttribute('name');
    if ($node->getAttribute('field') === 'locales') {
      return $this->renderLocales($node);
    }
    if ($this->isEditable($node) === false) $readonly = ' readonly="readonly"';
    if ($this->isDisabled($node)) $disabled = ' disabled="disabled"';

    $checkall = $this->useAttribute($node, 'checkall');

    $options = $this->getNodeAttribute($node, 'options');
    if ($options instanceof Collection) {
      $options = $options->asHash('id', true);
    }
    elseif (is_string($options) || is_numeric($options)) {
      $options = SH::asSet($options);
    }
    elseif (empty($options)) {
      $options = array();
    }
    $values = $this->getNodeAttribute($node, 'value');
    if ($values instanceof Collection) {
      //      $values = $values->asHash('id', true);
      $values = $values->asSet();
    }
    elseif (is_numeric($values) &&
            $this->hasItem() &&
            Model::isBitmaskField($this->item,
                                  $node->getAttribute('field'))) {
      $bits = array();
      for ($i = 0; $i <= 31; $i++) {
        $n = pow(2, $i);
        if ($n > $values) break ;
        if ($values & $n) $bits[] = $n;
      }
      $values = $bits;
    }
    elseif (is_scalar($values) || empty($values)) {
      $values = SH::asSet($values);
    }

    //    S('log')->debug($values);
    //    $html .= count($options) > 1 ? '<div class="options">'."\n" : '<div class="optionsSimple">'."\n";
    $html .= '<div class="options'.($this->useAttribute($node, 'hightlight')?' options-highlighted':'').'">'."\n";

    if (count($options) >= 1) {
      $html .= ' <input type="hidden" name="'.$name.'" value=""'.$readonly.$disabled.' />'."\n";
    }

    $elts = array();
    foreach ($options as $key => $value) {
      $checked = '';
      $value = is_array($value) ? $value['name'] : $value;
      $value = c($value);
      //      if (count($options) === 1) $value = '<label>'.$value.'</label>';
      if ($checkall === true || in_array($key, $values)) {
        $checked = ' checked="checked"';
      }
      $checkbox = '<input type="checkbox" name="'.$name.'['.$key.']" value="'.$key.'"'.$checked.$readonly.$disabled.' />';
      if (count($options) === 1 && $node->getAttribute('position') === 'right') {
        $elts[] = $value.'&nbsp;'.$checkbox;
      }
      else {
        $elts[] = ' <label>'.$checkbox.$value.'</label>';
      }
    }
    if (count($elts) > 1 && $node->hasAttribute('cols')) {
      $n = $node->getAttribute('cols');
      $max = ceil(count($elts) / $n);
      $max = $n;

      $i = 1;
      $j = 1;
      $cols = array();
      foreach ($elts as $col) {
        $cols[$i++][$j] = $col;
        if ($i > $max) {
          $i = 1;
          $j++;
        }
      }
      $cols = array_pad($cols, $max, array());
      $html .= '<table class="layout"><tr>';
      foreach ($cols as $col) {
        $html .= '<td>'."\n".join("\n", $col).'</td>'."\n";
      }
      $html .= '</tr></table>';
    }
    else {
      $html .= join("\n", $elts);
    }
    $html .= "\n";
    $html .= '</div>';

    return $html;
  }

  private function renderCheckbox2($node) {
    $html = '';
    foreach ($node->childNodes as $option) {
      // FIX
      $name = 'record['.substr($option->getAttribute('value'), 1).']';
      $value = (int) $this->getNodeAttribute($option, 'value');
      $label = $this->getNodeAttribute($option, 'label');
      $checked = $value === 1 ? ' checked="checked"' : '';
      $onclick = ' onclick="var i = $(this).up().next(\'input\'); i.value = $F(i).toggle();';
      if ($option->hasChildNodes()) {
        $onclick .= '$(this).up().next(\'div\').toggle()';
      }
      $onclick .= '"';
      $html .= '<div style="margin-bottom:6px;">'."\n";
      $html .= '<label><input type="checkbox" '.$checked.$onclick.' /> '.$label.'</label><br/>'."\n";
      $html .= '<input type="hidden" name="'.$name.'" value="'.($checked === '' ? 0 : 1).'" /> '."\n";
      if ($option->hasChildNodes()) {
        $style = $checked === '' ? 'display:none;' : '';
        $html .= '<div style="margin:4px 0 4px 20px; border:1px solid #ccc; padding:5px 8px 2px 8px;'.$style.'">';
        $html .= $this->renderContainer($option);
        $html .= '</div>'."\n";
      }
      $html .= '</div>'."\n";
    }
    return $html;
  }

  private function renderSingleOption($node) {
    $name = $node->getAttribute('name');
    $value = $node->hasAttribute('value') ? $this->getNodeAttribute($node, 'value') : 1;
    $label = $this->getNodeAttribute($node, 'label');
    $html .= '<table class="layout singleoption"><tr><td style="width:18px;">'."\n";
    $checked = '';
    if ($this->isChecked($node)) $checked = ' checked="checked"';
    $disabled = '';
    if ($this->isDisabled($node)) $disabled = ' disabled="disabled"';
    $html .= ' <input type="checkbox" name="'.$name.'" value="'.$value.'"'.$checked.$disabled.' /> '."\n";
    $html .= '</td><td>'."\n";
    $html .= ' '.c($label)."\n";
    $html .= '</td></tr></table>';
    return $html;
  }

  private function renderLang($node) {
    $name = $node->hasAttribute('name') ? $node->getAttribute('name') : 'record[locale]';
    if (!$node->hasAttribute('value')) $node->setAttribute('value', '$locale');
    $value = $this->getNodeAttribute($node, 'value');
    $locales = self::locales();
    if ($node->hasAttribute('options')) {
      $locales = array();
      foreach ($this->getNodeAttribute($node, 'options') as $locale) {
        $locales[$locale] = Locale::toCode($locale);
      }
    }
    if (count($locales) === 0) {
      return '<input type="hidden" name="record[locale]" value="'.$value.'" />';
    }
    $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
    $readonly = $this->isReadonly($node) ? ' readonly="readonly"' : '';
    $html = '<div class="options inline">'."\n";
    $html .= ' <input type="hidden" name="record[locale]" value="0"'.$disabled.$readonly.' />'."\n";
    foreach ($locales as $locale => $lang) {
      $checked = $value == $locale ? ' checked="checked"' : '';
      $html .= ' <label>';
      $html .= '<input type="radio" name="'.$name.'" value="'.$locale.'"'.$checked.$disabled.$readonly.' />';
      $html .= '<img src="'.PIX_URL.'/lang/'.Locale::toCode($locale).'.png" />';
      $html .= '</label>'."\n";
    }
    $html .= '</div>';
    return $html;
  }

  private function renderSelect($node) {
    if ($node->getAttribute('type') === 'country') {
      $options = I18N::getCountries(I18N::LOCALE_FR);
      $values = $this->ctx->getAttribute('country_id');
      $values = $values ? $values : Country::FR;
      if (!$node->hasAttribute('name')) {
        $node->setAttribute('name', 'record[country_id]');
      }
    }
    else {
      $values = $this->getNodeAttribute($node, 'value');
      if ($node->hasAttribute('options')) {
        $options = $this->getNodeAttribute($node, 'options');
        if ($options instanceof Collection) {
          $options = $options->asHash();
        }
      }
    }
    $name  = $node->getAttribute('name');

    if ($node->hasAttribute('size') || $node->hasAttribute('height')) {
      if ($values instanceof Collection) {
        $values = $values->asSet();
      }
      elseif (!is_array($values)) {
        $values = SH::asSet($values);
      }
      $style = $size = '';
      if ($node->hasAttribute('height')) {
        $style = ' style="height:'.$node->getAttribute('height').';"';
      }
      else {
        $style = ' size="'.$node->getAttribute('size').'"';
      }
      if ($node->getAttribute('selectall') == 'true') $selectall_flg = true;
      $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
      $html .= '<select name="'.$name.'[]" multiple="multiple"'.$size.$style.$disabled.'>'."\n";
      foreach ($options as $k => $v) {
        $selected = '';
        if (in_array($k, $values) || $selectall_flg===true) $selected = ' selected="selected"';
        $html .= ' <option value="'.$k.'"'.$selected.'>'.$v.'</option>'."\n";
      }
    }
    else {
      $readonly = $this->isReadonly($node) ? ' readonly="true"' : '';
      $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
      $onchange = '';
      if ($node->hasAttribute('enrichwith')) {
        $onchange = ' onchange="Kwo.Select.onOptionPrompt(this, \''.$node->getAttribute('enrichwith').'\', \''.MODEL.'\')"';
      }
      $html .= '<select name="'.$name.'"'.$readonly.$disabled.$onchange.'>'."\n";
      if ($node->hasAttribute('addfirst')) {
        $html .= ' <option value="">'.$node->getAttribute('addfirst').'</option>'."\n";
      }
      if ($options instanceof ActiveRecord) {
        $html .= $options->asOptions($values);
      }
      elseif (empty($options) && !$node->hasAttribute('addfirst')) {
        $html .= ' <option>--</option>'."\n";
      }
      else {
        foreach ($options as $k => $v) {
          if (is_array($v)) {
            $html .= '<optgroup label="' . $k . '">'."\n";
            foreach ($v as $id => $label) {
              $selected = '';
              if ($id == $values) $selected = ' selected="selected"';
              $html .= ' <option value="'.$id.'"'.$selected.'>'.$label.'</option>'."\n";
            }
            $html .= '</optgroup>'."\n";
          }
          else{
            $selected = '';
            if ($k == $values) $selected = ' selected="selected"';
            $html .= ' <option value="'.$k.'"'.$selected.'>'.$v.'</option>'."\n";
          }
        }
      }
      if ($node->hasAttribute('enrichwith')) {
        $html .= ' <option value="-1" style="font-style:italic;">'.l('ajouter un élément').'</option>'."\n";
      }
    }
    $html .= '</select>';
    /*    if ($node->hasAttribute('dynamic')) {
      $elt = $html;
      $html = '<div class="input-selector">';
      $html .= $elt;
      $attributes = array('data' => array('models' => 'country'),
                          //                          'on' => array('click' => 'new Kwo.Class.ItemEditor(this, $(this).up(\'.elem\'))'));
                          'on' => array('click' => 'new kwo.dialogs.ItemPicker(this)'));
      $html .= H::elt('a', self::SELECT_CHAR, $attributes);
      $html .= '</div>';
      }*/
    return $html;
  }

  public function isEdition() {
    return strpos($this->xml_file, '.edit.xml') > 0;
  }

  private function hasItem() {
    return $this->item instanceof ActiveRecord && $this->item->isItem();
  }

  private function renderItem($node) {
    if ($node->hasAttribute('model')) {
      $models = $this->getNodeAttribute($node, 'model');
      $model = $models;
    }
    else {
      $models = $this->getNodeAttribute($node, 'models');
    }
    $readonly = $this->isReadonly($node);
    $disabled = $this->isDisabled($node);
    if (!$node->hasAttribute('field') || $node->getAttribute('field') === 'item') {
      $html = '';
      if ($this->hasItem()) {
        $model_id = $this->item->model_id;
        $record_id = $this->item->record_id;
        if ($model_id > 0 && $record_id > 0) {
          try {
            $item = Item::getInstance($model_id, $record_id);
          }
          catch (Exception $e) {
            $model_id = $record_id = 0;
            S('log')->error($e, __METHOD__);
          }
        }
      }
      $str = $item instanceof ActiveRecord ? $item->getModel().': '.$item->getName() : h($str);
      if ($readonly) {
        $html .= H::elt('input', $model_id,
                        array('type' => 'hidden',
                              'name' => 'record[model_id]',
                              'class' => 'input-model-id'));
        $html .= H::elt('input', $record_id,
                        array('type' => 'hidden',
                              'name' => 'record[record_id]',
                              'class' => 'input-record-id'));
        $html .= H::elt('input', h($str),
                        array('readonly' => 'readonly',
                              'on' => array('focus' => 'this.blur()')));
      }
      else {
        $html = '<div class="input-selector">';
        $html .= H::elt('a', self::RESET_CHAR,
                        array('title' => 'vider',
                              'on' => array('click' => 'kwo.ux.onInputReset(this)')));
        $html .= H::elt('input', $model_id,
                        array('type' => 'hidden',
                              'name' => 'record[model_id]',
                              'class' => 'input-model-id'));
        $html .= H::elt('input', $record_id,
                        array('type' => 'hidden',
                              'name' => 'record[record_id]',
                              'class' => 'input-record-id'));

        $attrs = array('readonly' => 'readonly',
                       'data' => array(),
                       'on' => array());
        if ($item instanceof ActiveRecord) {
          $attrs['data']['item'] = $item->getKey();
          $attrs['on']['click'] = 'kwo.ux.onItemEdit(this)';
        }
        $html .= H::elt('input', $str, $attrs);
        $attrs = array('class' => 'item-picker-button',
                       'on' => array('click' => 'new kwo.dialogs.ItemPicker(this)'),
                       'data' => array('models' => $models,
                                       'value' => $value));
      	if ($this->model) {
      	  $attrs['data']['caller-model'] = $this->model;
      	}
        if ($this->hasItem()) {
          $attrs['data']['caller-item'] = $this->item->getKey();
        }
        $html .= H::elt('a', self::SELECT_CHAR, $attrs);
        $html .= '</div>';
      }
      return $html;
    }

    $name = $node->getAttribute('name');
    $value = $this->getNodeAttribute($node, 'value');
    if ($value > 0) {
      try {
        $item = Item::getInstance($model, $value);
      }
      catch (Exception $e) {
        $value = 0;
        S('log')->error($e, __METHOD__);
      }
    }
    $item_name = '';
    if ($item instanceof ActiveRecord) {
      $h = $item->asPickerHash();
      $item_name = $h['name'];
    }

    if ($readonly) {
      $html .= H::elt('input', $value,
                      array('type' => 'hidden',
                            'name' => $name));
      $html .= H::elt('input', $item_name,
                      array('readonly' => 'readonly',
                            'on' => array('focus' => 'this.blur()')));
      return $html;
    }


    $html = '<div class="input-selector">';

    $attrs = array();
    if ($disabled) {
      $attrs['class'] = array('disabled');
    }
    else {
      $attrs['on'] = array('click' => 'kwo.ux.onInputReset(this)');
      $attrs['title'] = l('vider');
    }
    $html .= H::elt('a', self::RESET_CHAR, $attrs);

    $attrs = array('type' => 'hidden',
                   'name' => $name,
                   'class' => 'input-record-id');
    $html .= H::elt('input', $value, $attrs);

    $attrs = array('readonly' => 'readonly');
    if ($item instanceof ActiveRecord) {
      if ($disabled) {
        $attrs['on'] = array('click' => 'this.blur()');
      }
      else {
        $attrs['data'] = array('item' => $item->getKey());
        $attrs['on'] = array('click' => 'kwo.ux.onItemEdit(this)');
      }
    }
    $html .= H::elt('input', $item_name, $attrs);

    $attrs = array('class' => array('item-picker-button'),
                     'data' => array());
    if ($disabled) {
      $attrs['class'][] = 'disabled';
    }
    else {
      $attrs['on'] = array('click' => 'new kwo.dialogs.ItemPicker(this)');
      $attrs['data']['models'] = $models;
    }
    if ($this->model) {
      $attrs['data']['caller-model'] = $this->model;
    }
    if ($this->hasItem()) {
//        $attrs['data']['item'] = $this->item->getKey();
        $attrs['data']['caller-item'] = $this->item->getKey();
    }
    $html .= H::elt('a', self::SELECT_CHAR, $attrs);

    $html .= '</div>';

    return $html;
  }

  private function renderBiselect($node) {
    $name = $node->getAttribute('name');
    $id = $node->getAttribute('id');
    $label = $this->getNodeAttribute($node, 'label');
    $labelbis = $this->getNodeAttribute($node, 'labelbis');
    $val_arr = $this->getNodeAttribute($node, 'value');
    //    $this->log->debug($val_arr);
    if (!is_array($val_arr)) $val_arr = SH::asSet($val_arr);
    $opt_ori_arr = (array) $this->getNodeAttribute($node, 'options');
    //    $this->log->debug($opt_ori_arr);
    //    $height = '400px';
    //    if ($node->hasAttribute('height')) $height = $node->getAttribute('height');
    $size = 8;
    if ($node->hasAttribute('size')) {
      $size = $node->getAttribute('size');
    }

    //$opt_arr = array_diff_key($opt_ori_arr, array_flip($val_arr));
    $opt_arr = array_diff_key($opt_ori_arr, $val_arr);
    $html  = '';
    $html .= '<table class="layout biselect" id="'.$id.'"><tr>'."\n";
    $html .= ' <td style="width:47%;">'."\n";
    $html .= '  <label>'.c($label).'</label><br/>'."\n";
    $html .= '  <select id="'.$id.'_1" size="'.$size.'" ondblclick="Kwo.Form.select_add(\''.$id.'\')">'."\n";
    foreach ($opt_arr as $k => $v) {
      $html .= '   <option value="'.$k.'">'.$v.'</option>'."\n";
    }
    // size="'.$size.'"
    $html .= '  </select>'."\n";
    $html .= ' </td>'."\n";
    $html .= ' <td style="width:6%; text-align:center; vertical-align:middle;">'."\n";
    $html .= '  <input type="button" onclick="Kwo.Form.select_add(\''.$id.'\')" value=">" title="'.l('Ajouter Ã  votre sélection').'" style="margin:0;" />'."\n";
    $html .= '  <br/><br/>'."\n";
    $html .= '  <input type="button" onclick="Kwo.Form.select_remove(\''.$id.'\')" value="<" title="'.l('Retirer de votre sélection').'" style="margin:0;" />'."\n";
    $html .= ' </td>'."\n";
    $html .= ' <td style="width:47%;">'."\n";
    $html .= '  <label><i>'.l('Votre selection').' '.$labelbis.'</i></label><br/>'."\n";
    $html .= '  <select id="'.$id.'_2" size="'.$size.'" ondblclick="Kwo.Form.select_remove(\''.$id.'\')">'."\n";

    foreach ($val_arr as $k => $v) {
      if (!array_key_exists($k, $opt_ori_arr)) continue;
      $html .= '   <option value="'.$k.'">'.$opt_ori_arr[$k].'</option>'."\n";
    }
    $html .= '  </select>'."\n";
    $html .= '  <select name="'.$name.'[]" id="'.$id.'_3" multiple="multiple" style="display:none;" >'."\n";
    foreach ($val_arr as $k => $v) {
      if (!array_key_exists($k, $opt_ori_arr)) continue;
      $html .= '   <option value="'.$k.'" selected="selected">'.$v.'</option>'."\n";
    }
    $html .= '  </select>'."\n";
    if ($node->hasAttribute('classifiable')) {
      $html .= '  <div style="text-align:right;">'."\n";
      $html .= '   <input type="button" onclick="Kwo.Form.option_move(\''.$id.'\',\'up\')" value="up" style="margin:0;" />';
      $html .= '   <input type="button" onclick="Kwo.Form.option_move(\''.$id.'\',\'down\')" value="down" style="margin:0;" />';
      $html .= '  </div>'."\n";
    }
    $html .= ' </td>'."\n";
    $html .= '</tr></table>';
    return $html;
  }

  private function renderFlowselect($node) {
    /*
$arr['cat1'] = array(1=>'prod 1 cat 1',2=>'prod 2 cat 1');
$arr['cat2'] = array('cat2'=>array(3=>'prod 1 cat 2',4=>'prod 2 cat 2'));
     */
    $id    = $node->getAttribute('id');
    $val   = $this->getNodeAttribute($node,'value');
    $opt_arr = (array) $this->getNodeAttribute($node,'options');
    if (count($opt_arr)<1) return '';
    $opt1_arr = $opt_arr[0];
    $opt2_arr = $opt_arr[1];
// spina 2008-11-06 ajout de < top. > pour ramener la variable a la racine DOM idem dans app/core/js/back.js Kwo.Form.populate()
    $html .= '<script>'."\n".' var top.'.$id.' = new Array();'."\n";
    $i = 0;
    foreach ($opt2_arr as $key => $tmp_arr) {
      $html .= ' top.'.$id.'["'.$key.'"] = {';
      $sep = '';
      if ($i===0) $sel_id = $key;
      foreach ($tmp_arr as $k => $v) {
        if ($k==$val) $sel_id = $key;
        $html .= $sep.'"'.$k.'":"'.$v.'"';
        $sep = ', ';
      }
      $html .= '};'."\n";
      $i++;
    }
    $html .= '</script>'."\n";

    $select1 .= ' <select name="record['.$id.'][0]" id="'.$id.'[0]" onchange="Kwo.Form.populate(\''.$id.'\')">'."\n";
    foreach ($opt1_arr as $k => $v) {
      $selected = '';
      if ($k==$sel_id) $selected = ' selected="selected"';
      $select1 .= '  <option value="'.$k.'"'.$selected.'>'.$v.'</option>'."\n";
    }
    $select1 .= ' </select>'."\n";

    $select2 .= ' <select name="record['.$id.'][1]" id="'.$id.'[1]">'."\n";
    foreach ($opt2_arr[$sel_id] as $k => $v) {
      $selected = '';
      if ($k==$val) $selected = ' selected="selected"';
      $select2 .= '  <option value="'.$k.'"'.$selected.'>'.$v.'</option>'."\n";
    }
    $select2 .= ' </select>'."\n";

    $html .= '<div style="margin:4px 0 4px 0;">'."\n";
    if ($node->hasAttribute('align')) {
      $html .= $select1;
      $html .= $select2;
    }
    else {
      $html .= '<table class="layout"><tr><td>'."\n";
      $html .= $select1;
      $html .= '</td>';
      $html .= '<td>'."\n";
      $html .= $select2;
      $html .= '</td></tr></table>'."\n";
    }
    $html .= '</div>';
    return $html;
  }

  private function renderHidden($node) {
    $name = $node->hasAttribute('name') ? ' name="'.$node->getAttribute('name').'"' : '';
    $id = $node->hasAttribute('id') ? ' id="'.$node->getAttribute('id').'"' : '';
    $value = ' value="'.$this->getNodeAttribute($node, 'value').'"';
    $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
    $html = '<input type="hidden"'.$name.$id.$value.$disabled.' />';
    return $html;
  }

  private function renderPassword($node) {
    $name = $node->hasAttribute('name') ? ' name="'.$node->getAttribute('name').'"' : '';
    $id = $node->hasAttribute('id') ? ' id="'.$node->getAttribute('id').'"' : '';
    $value = ' value="'.$this->getNodeAttribute($node, 'value').'"';
    $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
    $html = '<input type="password"'.$name.$id.$value.$disabled.' />';
    return $html;
  }

  private function renderText($node) {
    $name  = $node->getAttribute('name');
    if (!$node->hasAttribute('value') && substr($name, 0, 6) === 'datas[') {
      $data = substr($name, 6);
      $data = rtrim($data, ']');
      $node->setAttribute('value', '$_datas['.Item::$datas[$data].']');
    }
    if ($node->hasAttribute('rows') || $node->hasAttribute('height') || $node->hasAttribute('mode')) {
      return $this->renderTextarea($node);
    }

    if ($node->hasAttribute('count')) {
      $prefix = null;
      if ($node->hasAttribute('prefix')) {
        $prefix = $this->getNodeAttribute($node, 'prefix');
      }
      return $this->renderMultiText($node, $prefix);
    }
    $class = '';
    $maxlength = '';
    if ($node->hasAttribute('max')) {
      $maxlength = ' maxlength="'.$this->getNodeAttribute($node, 'max').'"';
    }
    if ($this->isEditable($node) ===false ) {
      $readonly = ' readonly="readonly"';
      $class   .= ' readonly';
    }
    $disabled = '';
    if ($this->isDisabled($node)) {
      $disabled = ' disabled="disabled"';
      $class .= ' disabled';
    }
    $onkeyup = '';
    if ($node->hasAttribute('jumpto')) {
      // NEED FIX : $id est indéfini
      $onkeyup = ' onkeyup="if ($F(\''.$id.'\').length >= '.$node->getAttribute('max').') { $(\''.$node->getAttribute('jumpto').'\').focus(); }"';
    }
    if ($node->hasAttribute('execonenter')) {
      $onkeydown = ' onkeydown="Kwo.Event.execOnEnter(\''.$node->getAttribute('execonenter').'\',event)"';
    }
    if ($node->hasAttribute('default') && $this->item && !$this->item->isItem()) {
      $value = $node->getAttribute('default');
    }
    elseif ($node->hasAttribute('value')) {
      $value = $this->getNodeAttribute($node, 'value');
    }
    if (is_array($value)) {
      $value = join(', ', $value);
    }
    else {
      $value = $this->protect($value);
    }

    $spellcheck = ' spellcheck="'.($node->hasAttribute('spellcheck') ? $node->getAttribute('spellcheck') : 'true').'"';
    if ($name === 'code' || $name === 'record[code]' || $name === 'record[website]') $spellcheck = '';
    $class = $this->req->isIe() ? ' class="'.$class.'"' : '';
    $style = $node->hasAttribute('options') ? ' style="margin-bottom:0px;"' : '';
    $lang = '';
    if ($node->hasAttribute('lang')) {
      $lang = ' lang="'.$node->getAttribute('lang').'"';
    }
    elseif ($this->item && $this->item->locale > 0) {
      $lang = ' lang="'.Locale::toCode($this->item->locale).'"';
    }
    $html .= '<input type="text" name="'.$name.'"'.$maxlength.$readonly.$disabled.$lang.$class.$style.$onkeydown.$onkeyup.$spellcheck.' value="'.$value.'" />';

    if ($node->hasAttribute('options')) {
      S('log')->trace('options in elem text is DEPRECATED', __METHOD__);
    }
    if ($node->hasAttribute('onclick')) {
      $input = $html;
      $html  = '<div class="input-selector">';
      $html .= H::elt('a', self::RESET_CHAR,
                      array('title' => 'vider',
                            'class' => 'reset',
                            'tabindex' => '-1',
                            'on' => array('click' => 'kwo.ux.onInputReset(this)')));
      $html .= $input;
      $html .= '<a href="javascript:void(0)" tabindex="-1" onclick="'.$node->getAttribute('onclick').'">'.self::SELECT_CHAR.'</a>';
      $html .= '</div>';
    }
    return $html;
  }

  private function renderMultiText($node, $prefix=null) {
    if ($node->hasAttribute('value')) {
      $values = $this->getNodeAttribute($node, 'value');
    }
    $count = (int) $this->getNodeAttribute($node, 'count');
    $name  = $node->getAttribute('name');
    $inputs = array();
    for ($i = 0; $i < $count; $i++) {
      $value = $this->protect($values[$i]);
      $inputs[] = '<input type="text" name="'.$name.'[]" value="'.$value.'" />';
    }
    if ($prefix === null) {
      $line_count = ceil($count / 3);
      $html = '<table class="layout multitext">'."\n";
      for ($i = 0; $i < $line_count; $i++) {
        $html .= '<tr>'."\n";
        for ($j = 0; $j < 3; $j++) {
          $html .= ' <td>'.$inputs[($i * 3) + $j].'</td>'."\n";
        }
        $html .= '</tr>'."\n";
      }
      $html .= '</table>';
    }
    else {
      $html = '<table class="layout multitext">'."\n";
      for ($i = 0; $i < $count; $i++) {
        $html .= '<tr>'."\n";
        $html .= ' <td class="label">'.$prefix.' '.($i + 1).'</td>'."\n";
        $html .= ' <td>'.$inputs[$i].'</td>'."\n";
        $html .= '</tr>'."\n";
      }
      $html .= '</table>';
    }
    return $html;
  }

  private function renderTextarea($node) {
    if ($node->hasAttribute('mode') && $node->getAttribute('mode') === 'rich') {
      return $this->renderRichtext($node);
    }
    $name = $node->getAttribute('name');
    $style = $node->getAttribute('style');
    $maxlength = '';
    if ($node->hasAttribute('max')) $maxlength = ' maxlength="'.$this->getNodeAttribute($node, 'max').'"';
    $rows = '';
    if ($node->hasAttribute('rows')) {
      $rows = 'rows="'.((int)$node->getAttribute('rows')).'"';
    }
    if ($node->hasAttribute('height')) {
      $style .= 'height:'.$node->getAttribute('height').';';
    }
    $value = $this->getNodeAttribute($node, 'value');
    if (is_array($value)) {
      $value = join(', ', $value);
    }
    else {
      $value = $this->protect($value);
    }
    $disabled = $this->isDisabled($node) ? ' disabled="disabled"' : '';
    $value = str_replace("\n", $this->nl, $value);
    $spellcheck = ' spellcheck="'.($node->hasAttribute('spellcheck') ? $node->getAttribute('spellcheck') : 'true').'"';
    $lang = '';
    if ($node->hasAttribute('lang')) {
      $lang = ' lang="'.$node->getAttribute('lang').'"';
    }
    elseif ($this->item && $this->item->locale > 0) {
      $lang = ' lang="'.Locale::toCode($this->item->locale).'"';
    }
    $html = '<textarea name="'.$name.'" '.$rows.' style="'.$style.'"'.$maxlength.$spellcheck.$lang.$disabled.'>'.$value.'</textarea>';

    $this->nl_replace_flag = true;
    return $html;
  }

  private function renderRichtext($node) {
    $id = $node->getAttribute('name').$this->elt_num.time();
    if (!$node->hasAttribute('rows')) {
      $node->setAttribute('rows', 10);
    }
    $name = ' name="'.$node->getAttribute('name').'"';
    $style = $node->getAttribute('style');
    $h = intval($node->getAttribute('rows')) * 16;
    $style .= 'height:'.$h.'px;';
    if (S('admin')->hasFlag(Admin::FLAG_RICHTEXT)) {
      $style .= 'display:none;';
    }
    $style = ' style="'.$style.'margin:0;"';
    $lang = '';
    if ($node->hasAttribute('lang')) {
      $lang = ' lang="'.$node->getAttribute('lang').'"';
    }
    elseif ($this->item && $this->item->locale > 0) {
      $lang = ' lang="'.Locale::toCode($this->item->locale).'"';
    }
    $value = $this->protect($this->getNodeAttribute($node, 'value'));
    $spellcheck = ' spellcheck="'.($node->hasAttribute('spellcheck') ? $node->getAttribute('spellcheck') : 'false').'"';
    $html  = '';
    $html .= '<div class="kwo-toolbar" style="display:none;"><img src="'.SH::dataImage().'"'."\n";
    $html .= '     id="'.$id.'" class="source" /></div>';
    $html .= '<textarea'.$name.$style.$spellcheck.'>'.$value.'</textarea>'."\n";
    $content = '<html><body></body></html>';
    if ($this->req->isChrome()) {
      // repasser à srcdoc="" en cas de pb
      $html .= '<iframe class="richtext" srcdoc="'.$content.'" name="editor" style="height:'.$h.'px;"'.$lang.'></iframe>';
    }
    elseif (true) {
//      $base64 = base64_encode($content);
//      $html .= '<iframe class="richtext" src="data:text/html;charset=utf-8;base64,'.$base64.'" name="editor" style="height:'.$h.'px;"'.$lang.'></iframe>';
      $html .= '<iframe class="richtext" srcdoc="'.$content.'" name="editor" style="height:'.$h.'px;"'.$lang.'></iframe>';
    }
    else {
      $lines = array('<html><head>',
                     '<style>',
                     'BODY { background:; cursor:text; height:100%; ',
                     '       line-height:1.4em; ',
                     '       border:none; color:#777; font-family:monospace;',
                     '       font-size:12px; margin:0px 2px 0 4px; }',
                     'TABLE.fixed { table-layout:fixed; width:100%;  }',
                     'TABLE TD, TABLE TH { border:1px solid #aaa; }',
                     'BIG { font-size:1.4em; }',
                     'SMALL { font-size:0.8em; }',
                     'OBJECT { background:#ffc; cursor:pointer;',
                     '         border:1px dotted #c00; display:inline-block; }',
                     'IFRAME { border:1px dotted #c00; padding:2px; }',
                     '.underline { text-decoration:underline; }',
                     '</style>',
                     '<link href="'.Stylesheet::url().'"',
                     '      rel="stylesheet" type="text/css" async="async" />',
                     '</head>',
                     '<body>'.$value.'</body>',
                     '</html>');
      $base64 = base64_encode(join("\n", $lines));
      $html .= '<iframe contenteditable="true" class="richtext has-content"'.
               ' src="data:text/html;charset=utf-8;base64,'.$base64.'"'.
               ' name="editor" style="height:'.$h.'px;"'.$lang.'></iframe>';
    }
    $html .= H::trigger('new Kwo.Richtext(this)');
    $this->nl_replace_flag = true;
    return $html;
  }

  public function renderHeading($node) {
    $html = '<h1>'.$node->getAttribute('label').'</h1>';
    return $html;
  }

  public static function grid() {}

  public static function getInstance($file) {
    return new UiManager($file);
  }

  private static function locales() {
    $locale = BackManager::getlocale();
    $locales = array($locale => Locale::toCode($locale));
    foreach (S('app')->getBackLocales() as $_locale) {
      if ($_locale == $locale) continue;
      $locales[$_locale] = Locale::toCode($_locale);
    }
    return $locales;
  }

}