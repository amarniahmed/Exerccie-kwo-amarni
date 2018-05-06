<?php

class Editor {

  public $mode = 'std';
  public $name = 'ed';
  public $width  = "100%";
  public $height = "200";

  public $actions = array();

  public function __construct($name='html', $content='') {
    $this->name    = $name;
    $this->content = $content;
  }

  public function loadToolbar() {
    $actions[] = array('t'=>'Supprimer les styles', 'i'=>'filters-smooth', 'a'=>"setContent('removeFormat')", 's'=>"style='margin:0 6px 0 4px '");
    $actions[] = array('t'=>'Typo', 'i'=>'font', 'a'=>"openDialog('font')");
    $actions[] = array('t'=>'Couleur', 'i'=>'text_color_foreground', 'a'=>"openDialog('fgcolor')");
    $actions[] = array('t'=>'Fond', 'i'=>'text_color_background', 'a'=>"openDialog('bgcolor')");
    $actions[] = array('t'=>'Gras', 'i'=>'text_bold', 'a'=>"setContent('bold')");
    $actions[] = array('t'=>'Italic', 'i'=>'text_italic', 'a'=>"setContent('italic')");
    $actions[] = array('t'=>'Souligner', 'i'=>'text_underlined', 'a'=>"setContent('underline')");
    $actions[] = array('t'=>'Titre 1', 'i'=>'h1', 'a'=>"setContent('formatBlock', '<h1>')");
    $actions[] = array('t'=>'Titre 2', 'i'=>'h2', 'a'=>"setContent('formatBlock', '<h2>')");
    $actions[] = array('t'=>'Titre 3', 'i'=>'h3', 'a'=>"setContent('formatBlock', '<h3>')");
    $actions[] = array('t'=>'Aligner à gauche', 'i'=>'text_left', 'a'=>"setContent('justifyleft')");
    $actions[] = array('t'=>'Centrer', 'i'=>'text_center', 'a'=>"setContent('justifycenter')");
    $actions[] = array('t'=>'Aligner à droite', 'i'=>'text_right', 'a'=>"setContent('justifyright')");
    $actions[] = array('t'=>'Justifier', 'i'=>'text_justify', 'a'=>"setContent('justifyfull')");
    $actions[] = array('t'=>'Indenter', 'i'=>'indent', 'a'=>"setContent('indent')");
    $actions[] = array('t'=>'Desindenter', 'i'=>'unindent', 'a'=>"setContent('outdent')");
    $actions[] = array('t'=>'Liste à puces', 'i'=>'list_bullet', 'a'=>"setContent('insertunorderedlist')");
    $actions[] = array('t'=>'Liste ordonnée', 'i'=>'list_enum', 'a'=>"setContent('insertorderedlist')");
    $actions[] = array('t'=>'Ligne horizontale', 'i'=>'insert-footer', 'a'=>"setContent('inserthorizontalrule')");
    if ($this->mode != "push") {
      //      $actions[] = array('t'=>'Style', 'i'=>'text_color_hilight', 'a'=>"showDialog(event,'style')");
    }
    $actions[] = array('t'=>'Hyperlien', 'i'=>'insert_bookmark', 'a'=>"openDialog('link')");
    $actions[] = array('t'=>'Insertion d\'image', 'i'=>'help-chat', 'a'=>"openDialog('img')");
    $actions[] = array('t'=>'Insertion de Table', 'i'=>'insert-cells', 'a'=>"openDialog('table')");
    //    $actions[] = array('t'=>'Propriétés des tables', 'i'=>'insert-cells-right', 'a'=>"openDialog('cell')");
    $actions[] = array('t'=>'Caractères Spéciaux', 'i'=>'format-character', 'a'=>"openDialog('char')");
    //    if ($this->mode!="push") $actions[] = array('t'=>'Elements graphiques', 'i'=>'element', 'a'=>"showDialog(event,'editor.uielement')");
    //      $actions[] = array('t'=>'Insertion de formulaire', 'i'=>'form-combobox', 'a'=>"kwoLayer(event,'extension','name=$s')");
    $actions[] = array('t'=>'Source HTML', 'i'=>'macro-stop-after-command', 'a'=>"toggleSource()", 's'=>"style='margin:0 4px 0 6px '");
    $this->actions = $actions;
  }

  public function setSize($width, $height) {
    if ($width >= 1) $this->width = $width.'px';
    else $this->width = '100%';
    $this->height = $height.'px';
  }

  public function setMode($mode) { $this->mode = $mode; }

  public function display() {
    $this->loadToolbar();
    $ret .= '<div style="width:'.$this->width.'; border:1px solid #ccc;" class="richtext">'."\n";
    $ret .= '<div class="toolbar">'."\n";
    foreach ($this->actions as $v) {
      $ret .= ' <img src="/'.APP_PATH.'/editor/pix/'.$v['i'].'.png" title="'.$v['t'].'"'."\n";
      $ret .= '      onclick="Kwo.registry[\''.$this->name.'\'].'.$v['a'].'" '.$v['s'].' />'."\n"; 
    }
    $ret .= '</div>'."\n";
    $ret .= '<iframe designmode="on" frameborder="no" id="'.$this->name.'_frame" name="'.$this->name.'_frame"'."\n"; 
    $ret .= '        style="height:'.$this->height.';" class="designMode"'."\n"; 
    $ret .= '        onmouseout="Kwo.registry[\''.$this->name.'\'].store();"></iframe>'."\n";
    $ret .= '<textarea id="'.$this->name.'" name="'.$this->name.'" spellcheck="false" class="sourceMode"'."\n"; 
    $this->content = str_replace("\n",'~~~~',$this->content);
    $ret .= '          style="display:none; height:'.$this->height.';">'.$this->content.'</textarea>'."\n";
    $ret .= '<script src="/'.APP_PATH.'/editor/controller.js"></script>'."\n";
    $ret .= '<script>Kwo.registry["'.$this->name.'"] = new Editor("'.$this->name.'","'.$this->mode.'");</script>'."\n";
    $ret .= '</div>';
    return $ret;
  }
  
  public function init() {
    $ret = '<script>Kwo.registry["'.$this->name.'"].init();</script>'."\n";
    return $ret;
  }

}
