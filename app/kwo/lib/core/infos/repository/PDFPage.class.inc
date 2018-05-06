<?php

class PDFPage {
  var $pdf_doc;
  var $name;
  var $page_size;
  var $margins;
  var $current_pos;
  var $font;
  var $font_name;
  var $font_size;
  var $font_color;
  var $check_overflow;
  var $overfl_pages;
  var $max_x;
  var $min_y;
  var $cellspacingV;
  var $cellspacingH;
  var $prec_x;
  var $check_nl;
  var $control_np;
  var $min_overfl;
  var $nb_elt;
  var $num_elt;
  var $prec;
  var $start_hbox;
  var $start_vbox;
  var $nb_pages;
  
  function PDFPage($pdf_doc, $name, $nb_pages, $font, $fontcolor, $margins) {
    $this->pdf_doc = $pdf_doc;
    /*
     * taille de la page pour format A4 : (595,842)
     * la position (0,0) se trouve en bas à gauche
     * on commence donc une page en haut à gauche à la position : (0,842)
     */
    $this->page_size['width'] = 595;
    $this->page_size['height'] = 842;
    // taille des marges de la page :
    $this->margins = array();
    if (!isset($margins[0]))
      $this->margins['left'] = 30;
    else
      $this->margins['left'] = $margins[0];
    if (!isset($margins[1]))
      $this->margins['right'] = 30;
    else
      $this->margins['right'] = $margins[1];
    if (!isset($margins[2]))
      $this->margins['top'] = 50;
    else
      $this->margins['top'] = $margins[2];
    if (!isset($margins[3]))
      $this->margins['bottom'] = 50;
    else
      $this->margins['bottom'] = $margins[3];

    // position de x et y selon où on se trouve dans la page
    $this->current_pos = array($this->margins['left'], $this->page_size['height'] - $this->margins['top']);
    //    $this->name = utf8_decode($name);
    $this->name = $name;
    if (empty($font))
      $this->font_name = "Vera"; // police par defaut
    else
      $this->font_name = $font;
    if (is_array($fontcolor))
      $this->font_color = $fontcolor;
    else
      $this->font_color = array(0, 0, 0);
    $this->font_size = "10";
    $this->check_overflow = 0;
    $this->overfl_pages = 0;
    $this->check_nl = 0;
    $this->control_np = 0;
    $this->cellspacingV = 8;
    $this->cellspacingH = 10;
    $this->max_x = $this->page_size['width'];
    $this->min_y = $this->page_size['height'] - $this->margins['top'];
    $this->start_hbox = array($this->margins['left'], $this->page_size['height'] - $this->margins['top']);
    $this->nb_pages = $nb_pages;
    
    PDF_begin_page($this->pdf_doc, $this->page_size['width'], $this->page_size['height']);
    $this->nb_pages++;
    $handl = PDF_create_bookmark($this->pdf_doc, $this->name, "");
    $this->load_new_font($this->font_name);
  }	

  function get_nb_pages() {
    return $this->nb_pages;
  }
  
  // Chargement d'une police
  function load_new_font($font) {
    PDF_set_parameter($this->pdf_doc, "SearchPath", "media/font");
    if (false == ($this->font = PDF_load_font($this->pdf_doc, $font, "host", "errorpolicy=return")))
      die("Erreur : " . PDF_get_errmsg($this->pdf_doc));
    PDF_setfont($this->pdf_doc, $this->font, $this->font_size);
  }
  
  // mise à jour des espacements
  function set_cellspacing($vert, $hor) {
    $this->cellspacingH = $hor;
    $this->cellspacingV = $vert;
  }
  
  /**
   * Creation d'une zone de texte
   *
   * $txt      : tableau contenant les infos du texte
   * $pos      : position de début de la zone
   * $num_vbox : numéro de la Vbox courante au sein de la Hbox
   * $nb_vbox  : nombre de Vbox dans la Hbox courante
   * $vbox     : $vbox['width'] et $vbox['height'] largeur et hauteur de la vbox courante
   */
  function create_txt_area($txt, $pos, $num_vbox, $nb_vbox, $vbox) {
    $tmp_x = $pos[0];
    $tmp_y = $pos[1];
    $right_x = $this->margins['left'];
    // options du textflow (selon les parametres)
    // leading = hauteur d'une ligne (texte + espacement interligne) 100% = hauteur du texte (interligne = 0)
    $opt_list = "encoding=winansi leading=120%"; 
    // Récupération et vérification des paramètres
    $check_font = '';
    $check_size = '';
    $overfl_vert = "true"; // overflow sur le côté : passer à la ligne (par défaut) ou couper le texte
    $overfl_hor = "true"; // overflow en bas : continuer sur une nouvelle page (par défaut) ou couper le texte
    $nb_col = 1;
    $fontcolor = '';
    $border = 'false';
    foreach ($txt as $k => $attr) {
      switch ($k) {
      case 'VALUE' :
	$data = $attr;
	break;
      case 'ALIGN' :
	$opt_list .= " alignment=".$attr;
	break;
      case 'FONT' :
	$check_font = $attr;
	break;
      case 'FONTSIZE' :
	$check_size = $attr;
	break;
      case 'FONTCOLOR' :
	$tab = explode(',', $attr);
	$fontcolor = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	break;
      case 'FONTSTYLE' :
	$opt_list .= " fontstyle=" . $attr;
	break;
      case 'OVERFLOWVERT' :
	$overfl_vert = $attr;
	break;
      case 'OVERFLOWHOR' :
	$overfl_hor = $attr;
	break;
      case 'NBCOL' :
	$nb_col = $attr;
	break;
      case 'UNDERLINE' :
	$underline = $attr;
	break;
      }
    }
    if (!empty($check_font)) {
      $font = $check_font;
      $opt_list .= " fontname=" . $check_font;
    } else {
      $font = $this->font_name;
      $opt_list .= " fontname=". $this->font_name;
    }
    if (!empty($check_size)) {
      $size = $check_size;
      $opt_list .= " fontsize=". $check_size;
    } else {
      $size = $this->font_size;
      $opt_list .= " fontsize=". $this->font_size;
    }
    if (!strcmp($underline, 'true'))
      $opt_list .= " underline=true";
    if (!strcmp($border, 'true'))
      $bord = "showborder=true";
    else
      $bord = "";
    
    // si une cellule se trouve trop près de la marge, on passe à la ligne
    if ($tmp_x >= ($this->page_size['width'] - $this->margins['right'] - 5)) {
      if ($this->check_overflow != 0) {
	$scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	if (!strcmp($scope, "page"))
	  PDF_suspend_page($this->pdf_doc, "");
	PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages));
	$this->check_nl = 1;
      } else
	$this->check_nl = 2;
      $tmp_x = $this->margins['left'];
      $tmp_y = $this->min_y - ($this->cellspacingV * 2);
      $right_x = $tmp_x;
      $this->current_pos = array($tmp_x, $tmp_y);
      $this->prec_x = $tmp_x;
      $this->start_hbox[0] = $this->margins['left'];
      $this->prec = 0;
    }
    
    // si une cellule se trouve trop près du bas de page, on créé une nouvelle page
    if ($tmp_y <= $this->margins['bottom']) {
      $this->overfl_pages++;
      $this->check_overflow++;
      $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
      if (!strcmp($scope, "page"))
	PDF_suspend_page($this->pdf_doc, "");
      if ($this->overfl_pages > $this->prec) {
	PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	$this->nb_pages++;
      } else
	PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($this->prec - $this->overfl_pages)));
      $tmp_y = $this->page_size['height'] - $this->margins['top'];
    }
    
    // traitement des données
    if (!empty($data)) {
      $data = str_replace('\n', "\n", $data);
      $check_over = false;
      $f = PDF_load_font($this->pdf_doc, $font, "winansi", "");
      $color = "";
      if (is_array($fontcolor)) {
	$r = round($fontcolor[0] * 100 / 255) / 100;
	$g = round($fontcolor[1] * 100 / 255) / 100;
	$b = round($fontcolor[2] * 100 / 255) / 100;
      } else {
	$r = round($this->font_color[0] * 100 / 255) / 100;
	$g = round($this->font_color[1] * 100 / 255) / 100;
	$b = round($this->font_color[2] * 100 / 255) / 100;
      }
      $opt_list .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";
      $opt_list .= " strokecolor={rgb ". $r ." ". $g ." ". $b ."}";
      // calcul de la longueur du texte			
      $txtlen = PDF_stringwidth($this->pdf_doc, $data, $f, $size);
      $txtflow = PDF_create_textflow($this->pdf_doc, $data, $opt_list);
      $val_x = ceil($txtlen);
      
      // on vérifie si le texte s'étend sur plusieurs lignes et adaptation de la taille en conséquence
      $tab_str = explode("\n", $data);	
      if (count($tab_str) > 1) {
	$val_y = $size * count($tab_str);
	if ($tmp_y - $val_y < $this->margins['bottom'])
	  $end_y = $this->margins['bottom'];
	else
	  $end_y = $tmp_y - $val_y;
      } else {
	$val_y = $size;
	$end_y = $tmp_y - $val_y;
      }
      $end_x = $tmp_x + $val_x;
      
      // Si la taille de la cellule est fixée
      if ($vbox['width'] != 0)
	$end_x = $tmp_x + $vbox['width'];
      if ($vbox['height'] != 0)
	$end_y = $tmp_y - $vbox['height'];
      
      // si on arrive au bord droit de la page (ou à la valeur max en largeur), la zone de texte s'agrandie verticalement et le texte passe à la ligne
      if ($end_x > $this->page_size['width'] - $this->margins['right']) {
	$end_x = $this->page_size['width'] - $this->margins['right'];
	if (!strcmp($overfl_hor, "true"))
	  $end_y = $this->margins['bottom'];	
      }
      $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
      if (strcmp($scope, "page")) {
	// si on a déjà du changer de page
	if ($this->check_overflow != 0) {
	  $p = $this->nb_pages - ($this->nb_pages - $this->overfl_pages - 1);
	  if ($p > $this->nb_pages) {
	    PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	    $this->nb_pages++;
	    $this->check_overflow++;
	    if ($this->overfl_pages > $this->prec)
	      $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	  } else if ($this->prec != 1) {
	    if ($this->prec > $this->overfl_pages)
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($this->prec - $this->overfl_pages)));
	    else
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($p));
	  } else
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages));
	} else {
	  PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	  $this->nb_pages++;
	  $this->check_overflow++;
	  if ($this->overfl_pages > $this->prec || ($this->overfl_pages == 0 && $this->prec == 0))
	    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	}
      }
      
      if ($this->overfl_pages > $this->prec)
	$this->min_overfl = $this->page_size['height'] - $this->margins['top'];
      
      // si on arrive au bas de la page, passage à la suivante (si overflow à 'true')
      if (!strcmp($overfl_vert, "true")) {
	$min_y = $this->page_size['height'] - $this->margins['top'];
	$col_width = floor(($end_x - $tmp_x - $this->cellspacingH * $nb_col) / $nb_col);
	
	// mise en page du texte sur plusieurs colonnes (pour les textes longs)
	if ($nb_col > 1) {
	  for ($i = 0; $i < $nb_col; $i++) {
	    if ($i == 0) {
	      $left = $tmp_x;
	      $right = $left + $col_width;
	    } else {
	      $left = $right + $this->cellspacingH;
	      $right = $left + $col_width;
	    }
	    $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $left, $tmp_y, $right, $end_y, $bord);
	    $end_y = PDF_info_textflow($this->pdf_doc, $txtflow, "textendy");
	    if ($end_y < $min_y)
	      $min_y = $end_y;
	  }
	} else {
	  if ($end_x == $this->page_size['width'] - $this->margins['right'])
	    $end_x -= $this->cellspacingH;
	  $end_y = $this->margins['bottom'];
	  $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $tmp_x, $tmp_y, $end_x, $end_y, $bord);
	  $end_y = PDF_info_textflow($this->pdf_doc, $txtflow, "textendy");
	}
	
	if ($end_y < ($this->margins['bottom'] + $size + $this->cellspacingV)) {
	  $this->check_overflow++;
	  $this->overfl_pages++;
	  $end_y = $this->page_size['height'] - $this->margins['top'];
	  if (!strcmp($res, "_stop")) {
	    PDF_suspend_page($this->pdf_doc, "");
	    if ($this->overfl_pages > $this->prec && ($this->num_elt < $this->nb_elt || $num_vbox < $nb_vbox)) {
	      PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	      $this->nb_pages++;
	      $this->min_y = $this->page_size['height'] - $this->margins['top']; 
	      $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	    } else if ($this->overfl_pages != 0 && $num_vbox < $nb_vbox)						
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($this->prec - $this->overfl_pages)));
	    else if ($this->overfl_pages == $this->prec && $this->prec != 0)
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages));
	    else
	      $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	  } else
	    $check_over = true;
	}
	
	if ($end_y < $min_y)
	  $min_y = $end_y;
	if ($this->overfl_pages != 0) {
	  if (($this->overfl_pages >= $this->prec) && ($this->min_overfl > $min_y))
	    $this->min_overfl = $min_y;
	}
	
	if ($nb_vbox > 1 && $num_vbox < $nb_vbox) {
	  $tab_tmp = explode("\n", $data);
	  if (count($tab_tmp) > 1) {
	    $max_length = 0;
	    foreach ($tab_tmp as $str) if ($max_length < PDF_stringwidth($this->pdf_doc, $str, $f, $size))
	      $max_length = PDF_stringwidth($this->pdf_doc, $str, $f, $size);
	    $end_x = $tmp_x + $max_length;
	  }
	  
	  $max_x = PDF_info_textflow($this->pdf_doc, $txtflow, "textendx");
	  if ($max_x > $end_x)
	    $end_x = $max_x;
	}
	
	// passage à une nouvelle page
	if (strcmp($res, "_stop") && strcmp($res, "_boxempty")) {
	  if ($end_y < $this->page_size['height'] - $this->margins['top'] && $end_y > $this->margins['bottom'] + $size) {
	    $tmp_y = $end_y;
	  } else {
	    $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	    if (!strcmp($scope, "page"))
	      PDF_suspend_page($this->pdf_doc, "");
	    $tmp_y = $this->page_size['height'] - $this->margins['top'];
	    $min_y = $tmp_y;
	  }
	  while (strcmp($res, "_stop")) {
	    $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	    if (strcmp($scope, "page")) {
	      if ($this->overfl_pages > $this->prec) {
		PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
		$this->nb_pages++;
		$this->min_overfl = $this->page_size['height'] - $this->margins['top'];
		$this->min_y = $this->page_size['height'] - $this->margins['top'];
		$this->check_overflow++;
		if ($end_x >= $this->page_size['width'] - $this->margins['right'])
		  $this->control_np++;
	      } else
		PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages));
	    }
	    if ($nb_vbox == 1) {
	      $tmp_x = $this->margins['left'];
	      $end_x = $this->page_size['width'] - $this->margins['right'];
	      if ($vbox['width'] != 0)
		$end_x = $tmp_x + $vbox['width'];
	    }
	    if ($end_x > $this->page_size['width'] - $this->margins['right'])
	      $end_x = $this->page_size['width'] - $this->margins['right'];
	    if ($vbox['width'] != 0 && $nb_col == 1)
	      $col_width = $end_x - $tmp_x;
	    else
	      $col_width = ($end_x - $tmp_x - $this->cellspacingH * $nb_col) / $nb_col;
	    $this->check_overflow++;
	    $this->overfl_pages++;
	    $end_y = $this->margins['bottom'];
	    $i = 0;
	    while ($i < $nb_col && strcmp($res, "_stop")) {
	      $prec_y = $end_y;
	      if ($i == 0) {
		$left = $tmp_x;
		$right = $left + $col_width;
	      } else {
		$left = $right + $this->cellspacingH;
		$right = $left + $col_width;
		$end_y = PDF_info_textflow($this->pdf_doc, $txtflow, "textendy");
	      }
	      $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $left, $tmp_y, $right, $end_y, $bord);
	      $end_y = PDF_info_textflow($this->pdf_doc, $txtflow, "textendy");
	      $i++;
	    }
	    if ($i == 1)
	      $min_y = $end_y;
	    else
	      $min_y = $prec_y;
	    if ($this->min_overfl > $min_y && $this->overfl_pages >= $this->prec)
	      $this->min_overfl = $min_y;
	    if (!strcmp($res, "_stop") && ($min_y > ($this->margins['bottom'] + $size)))
	      $tmp_y = $min_y;
	    else {
	      PDF_suspend_page($this->pdf_doc, "");
	      $tmp_y = $this->page_size['height'] - $this->margins['top'];
	    }
	  } // end while res != stop
	  
	  if ($this->overfl_pages >= $this->prec && strcmp($res, "_stop")) { 
	    if ($nb_vbox == 1)
	      $this->min_overfl = $tmp_y;
	    else if ($num_vbox < $nb_vbox) {
	      if ($min_y < $this->margins['bottom'] + $size) {
		$scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
		if (!strcmp($scope, "page"))
		  PDF_suspend_page($this->pdf_doc, "");
		PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
		$this->nb_pages++;
		$this->min_overfl = $this->page_size['height'] - $this->margins['top'];
		$this->min_y = $this->page_size['height'] - $this->margins['top'];
		$tmp_y = $this->page_size['height'] - $this->margins['top'];
		$this->current_pos = array($this->current_pos[0], $tmp_y);
	      } else if ($end_x > $this->page_size['width'] - $this->margins['right'])
		$this->current_pos = array($this->current_pos[0], $tmp_y - $this->cellspacingV);
	      
	      if ($end_x > $this->page_size['width'] - $this->margins['right']) {
		$this->prec = 0;
		$this->check_overflow = 0;
		$end_x = $this->page_size['width'] - $this->margins['right'];
		if ($end_x > $this->max_x)
		  $this->max_x = $end_x;
		if ($end_x > $right_x)
		  $right_x = $end_x;
	      } 
	    }
	  }
	  if ($check_over == true)
	    $this->overfl_pages--;
	  $end_y = $tmp_y;
	}
      } else
	$res = PDF_fit_textflow($this->pdf_doc, $txtflow, $tmp_x, $tmp_y, $end_x, $end_y, $bord);
      
      if ($nb_vbox > 1) {
	$max_x = PDF_info_textflow($this->pdf_doc, $txtflow, "textendx");
	if ($max_x > $end_x)
	  $end_x = $max_x;
      }
      
      if ($this->overfl_pages > 0 && $this->overfl_pages >= $this->prec && $this->min_overfl > $end_y)
	$this->min_overfl = $end_y;
      if ($right_x > $this->max_x)
	$this->max_x = $right_x;
      if ($min_y < $end_y)
	$end_y = $min_y;
      PDF_delete_textflow($this->pdf_doc, $txtflow);
      $pos_x = $end_x + $this->cellspacingH;
      $pos_y = $end_y - ($this->cellspacingV / 2);
      
    } else {
      $pos_x = $tmp_x;
      $pos_y = $tmp_y;
    }		
    // on décale le curseur à la suite du texte avec les nouvelles coordonnées
    return array($pos_x, $pos_y);
  }
  
  /**
   * Création d'un tableau
   *
   * $table : tableau contenant les infos du tableau
   * $pos   : position de départ du tableau
   * $num_vbox : numéro de la Vbox courante au sein de la Hbox
   * $nb_vbox  : nombre de Vbox dans la Hbox courante
   *
   */
  function create_table($table, $pos, $num_vbox, $nb_vbox) {
    // récupération des options du tableau
    $opt_list = "encoding=winansi";
    $t_list = "encoding=winansi";

    $check_font = '';
    $check_size = '';
    $overfl_vert = "true";
    $overfl_hor = "true";
    $fontcolor = '';
    $bordercolor = '';
    $title_style = '';
    $title_font = '';
    $title_size = '';
    $title_color = '';
    $title_align = '';
    $t_valign = '';
    $style = '';
    $align = '';
    $valign = "verticalalign=top";
    $showborder = "false";
    foreach ($table as $k => $attr) {
      switch ($k) {
      case 'VALUE' :
	$data = $attr;
	break;
      case 'ALIGN' :
	$align = $attr;
	$opt_list .= " alignment=" . $attr;
	break;
      case 'VALIGN' :
	$valign = "verticalalign=" . $attr;
	break;
      case 'FONT' :
	$check_font = $attr;
	break;
      case 'FONTSIZE' :
	$check_size = $attr;
	break;
      case 'FONTSTYLE' :
	$style = " fontstyle=" . $attr;
	$opt_list .= " fontstyle=" . $attr;
	break;
      case 'FONTCOLOR' :
	$tab = explode(',', $attr);
	$fontcolor = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	break;
      case 'TITLESTYLE' :
	$title_style = $attr;
	break;
      case 'TITLEFONT' :
	$title_font = $attr;
	break;
      case 'TITLESIZE' :
	$title_size = $attr;
	break;
      case 'TITLECOLOR' :
	$tab = explode(',', $attr);
	$title_color = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	break;
      case 'TITLEALIGN' :
	$title_align = $attr;
	break;
      case 'TITLEVALIGN' :
	$t_valign = "verticalalign=" . $attr;
	break;
      case 'OVERFLOWVERT' :
	$overfl_vert = $attr;
	break;
      case 'BORDERCOLOR' :
	$tab = explode(',', $attr);
	$bordercolor = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	break;
      case 'SHOWBORDER' :
	$showborder = $attr;
	break;
      }
    }
    if (!empty($check_font)) {
      $font = $check_font;
      $opt_list .= " fontname=" . $check_font;
    } else {
      $font = $this->font_name;
      $opt_list .= " fontname=". $this->font_name;
    }
    if (!empty($check_size)) {
      $size = $check_size;
      $opt_list .= " fontsize=". $check_size;
    } else {
      $size = $this->font_size;
      $opt_list .= " fontsize=". $this->font_size;
    }
    
    // si la position donnée en y est trop près du bas de page, création d'une nouvelle page
    $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
    if ($pos[1] <= $this->margins['bottom'] + $size) {
      if (!strcmp($scope, "page"))
	PDF_suspend_page($this->pdf_doc, "");
      PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
      $this->nb_pages++;
      $this->max_x = $this->margins['left'];
      $this->min_y = $this->page_size['height'] - $this->margins['top'];
      $pos[1] = $this->page_size['height'] - $this->margins['top'];
      
    } else if (strcmp($scope, "page")) {
      PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
      $this->nb_pages++;
      $this->max_x = $this->margins['left'];
      $this->min_y = $this->page_size['height'] - $this->margins['top'];
    }
    
    // définition du nombre de colonnes du tableau		
    if (!empty($data))
      $nb_col = count($data[0]);
    
    // récupération des éléments Colonne s'il y en a
    if (is_array($table['COL'])) {
      $cols_number = count($table['COL']);
      $col_size = '';
      $tmp_width = '';
      $max_col_size = 0;
      for ($i = 0; $i < $cols_number; $i++) {
	$tmp_opt[$i] = 0; 
	$col_align = '';
	$col_valign = '';
	$col_font = '';
	$col_fontsize = '';
	$col_fontcolor = '';
	$col_style = '';
	$place = array();
	foreach ($table['COL'][$i] as $k => $attr) {
	  switch ($k) {
	  case 'ALIGN' :
	    $col_align = $attr;
	    break;
	  case 'VALIGN' :
	    $col_valign = "verticalalign=" . $attr;
	    break;
	  case 'FONT' :
	    $col_font = $attr;
	    break;
	  case 'FONTSIZE' :
	    $col_fontsize = $attr;
	    break;
	  case 'FONTSTYLE' :
	    $col_style = " fontstyle=" . $attr;
	    break;
	  case 'FONTCOLOR' :
	    $tab = explode(',', $attr);
	    $col_fontcolor = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	    break;
	  case 'WIDTH' :
	    $tmp_width[$i] = $attr;
	    break;
	  case 'LABEL' :
	    $label = $attr;
	    break;
	  }
	}
	if (isset($label))
	  $place[$i] = ($label - 1);
	else
	  $place[$i] = $i;
	if (!empty($col_align))
	  $tmp_align = $col_align;
	else
	  $tmp_align = $align;
	if (!empty($col_font))
	  $tmp_font = $col_font;
	else
	  $tmp_font = $font;
	if (!empty($col_fontsize))
	  $tmp_size = $col_fontsize;
	else
	  $tmp_size = $size;
	if (!empty($col_style))
	  $tmp_style = $col_style;
	else
	  $tmp_style = $style;
	if (!empty($col_valign))
	  $tmp_valign[$i] = $col_valign;
	else
	  $tmp_valign[$i] = $valign;
	$tmp_opt[$i] = "encoding=winansi";
	$tmp_opt[$i] .= " alignment=" . $tmp_align;
	$tmp_opt[$i] .= " fontname=". $tmp_font;
	$tmp_opt[$i] .= " fontsize=". $tmp_size;
	$tmp_opt[$i] .= $tmp_style;
	if (is_array($col_fontcolor)) {
	  $r = round($col_fontcolor[0] * 100 / 255) / 100;
	  $g = round($col_fontcolor[1] * 100 / 255) / 100;
	  $b = round($col_fontcolor[2] * 100 / 255) / 100;
	} elseif (is_array($fontcolor)) {
	  $r = round($fontcolor[0] * 100 / 255) / 100;
	  $g = round($fontcolor[1] * 100 / 255) / 100;
	  $b = round($fontcolor[2] * 100 / 255) / 100;
	} else {
	  $r = round($this->font_color[0] * 100 / 255) / 100;
	  $g = round($this->font_color[1] * 100 / 255) / 100;
	  $b = round($this->font_color[2] * 100 / 255) / 100;
	}
	$tmp_opt[$i] .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";
	$col_opt[$place[$i]] = $tmp_opt[$i];
	$valign_col[$place[$i]] = $tmp_valign[$i];
	$col_size[$place[$i]] = $tmp_width[$i];					
	
	$title_list[$place[$i]] = "encoding=winansi";
	if (!empty($title_style))
	  $title_list[$place[$i]] .= " fontstyle=" . $title_style;
	else
	  $title_list[$place[$i]] .= $tmp_style;
	if (!empty($title_font)) {
	  $title_list[$place[$i]] .= " fontname=" . $title_font;
	  $t_font = $title_font;
	} else {
	  $title_list[$place[$i]] .= " fontname=" . $tmp_font;
	  $t_font = $tmp_font;
	}
	if (!empty($title_size)) {
	  $title_list[$place[$i]] .= " fontsize=" . $title_size;
	  $t_size = $title_size;
	} else {
	  $title_list[$place[$i]] .= " fontsize=" . $tmp_size;
	  $t_size = $tmp_size;
	}
	if (!empty($title_align)) 
	  $title_list[$place[$i]] .= " alignment=" . $title_align;
	else 
	  $title_list[$place[$i]] .= " alignment=" . $tmp_align;
	if (!empty($t_valign))
	  $title_valign[$place[$i]] = $t_valign;
	else
	  $title_valign[$place[$i]] = $valign_col[$place[$i]];
	if (is_array($title_color)) {
	  $r = round($title_color[0] * 100 / 255) / 100;
	  $g = round($title_color[1] * 100 / 255) / 100;
	  $b = round($title_color[2] * 100 / 255) / 100;
	}
	$title_list[$place[$i]] .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";
	if ($max_col_size < $tmp_size)
	  $max_col_size = $tmp_size;
      }// end for cols_number
    } // end if table childNodes
    if ($cols_number == 0) {
      $max_col_size = $size;
      if (!empty($title_style))
	$t_list .= " fontstyle=" . $title_style;
      elseif (!empty($style))
	$t_list .= $style;
      if (!empty($title_font)) {
	$t_list .= " fontname=" . $title_font;
	$t_font = $title_font;
      } else {
	$t_list .= " fontname=" . $font;
	$t_font = $font;
      }
      if (!empty($title_size)) {
	$t_list .= " fontsize=" . $title_size;
	$t_size = $title_size;
      } else {
	$t_list .= " fontsize=" . $max_col_size;
	$t_size = $max_col_size;
      }
      if (!empty($title_align))
	$t_list .= " alignment=" . $title_align;
      elseif (!empty($align))
	$t_list .= " alignment=" . $align;
    }
    
    // traitement des données
    if (!empty($data)) {
      $nb_row = count($data);
      if (is_array($title_color)) {
	$r = round($title_color[0] * 100 / 255) / 100;
	$g = round($title_color[1] * 100 / 255) / 100;
	$b = round($title_color[2] * 100 / 255) / 100;
      } elseif (is_array($fontcolor)) {
	$r = round($fontcolor[0] * 100 / 255) / 100;
	$g = round($fontcolor[1] * 100 / 255) / 100;
	$b = round($fontcolor[2] * 100 / 255) / 100;
      } else {
	$r = round($this->font_color[0] * 100 / 255) / 100;
	$g = round($this->font_color[1] * 100 / 255) / 100;
	$b = round($this->font_color[2] * 100 / 255) / 100;
      }
      $t_list .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";
      /* 
       * calcul des coordonnées et tailles du tableau
       * si le tableau s'étend sur plusieurs pages, calcul du nombre de pages,
       * du nombre de lignes par page
       */
      $coords = array($pos[0], $pos[1]); //coordonnées d'origine du tableau
      // calcul du nombre de colonnes et de la largeur de chaque colonne (=> largeur du tableau)
      if (is_array($col_size)) {
	$def_col = count($col_size);
	if ($def_col < $nb_col) {
	  $tmp_width = 0;
	  for ($i = 0; $i < $nb_col; $i++)
	    $tmp_width += $col_size[$i];
	  $diff_width = floor(($this->page_size['width'] - ($this->margins['left'] + $this->margins['right']) - $tmp_width) / ($nb_col - $def_col));
	  for ($i = 0; $i < $nb_col; $i++) if ($col_size[$i] == 0)
	    $col_size[$i] = $diff_width;
	}
	$tab_width = 0;
	for ($i = 0; $i < count($col_size); $i++)
	  $tab_width += $col_size[$i];
	if ($tab_width > $this->page_size['width'] - ($this->margins['left'] + $this->margins['right'])) {
	  $diff = $tab_width - ($this->page_size['width'] - ($this->margins['left'] + $this->margins['right']));
	  $minus = ceil($diff / $nb_col);
	  for ($i = 0; $i < count($col_size); $i++)
	    $col_size[$i] -= $minus;
	  $tab_width = $this->page_size['width'] - ($this->margins['left'] + $this->margins['right']);
	}
      } else
	$tab_width = $this->page_size['width'] - ($this->margins['left'] + $this->margins['right']);
      if ($max_col_size == 0)
	$height_row = 10;
      else
	$height_row = $max_col_size * 1.5; //hauteur des cellules
      $title_height = $t_size * 2; //hauteur des cellules de titres
      $tab_height = ($nb_row - 1) * $height_row + $title_height; // hauteur du tableau
      $width_col = round($tab_width / $nb_col); //largeur d'une colonne
      
      // si le tableau ne rentre pas dans la page (en hauteur)
      if ($coords[1] - $tab_height < $this->margins['bottom'] && $overfl_vert != 'false') {
	$size_y = $coords[1] - $this->margins['bottom'];
	if ($size_y < $title_height) {
	  $rows = 0;
	  $rest = $tab_height;
	  $rows_rest = $nb_row;
	} else {
	  $rows = 1 + ceil(($size_y - $title_height) / $height_row);
	  $rest = $tab_height - $size_y;
	  $rows_rest = $nb_row - $rows;
	}
	$max_rows_page = floor(($this->page_size['height'] - ($this->margins['top'] + $this->margins['bottom'])) / $height_row);
	if ($max_rows_page == 0)
	  $max_rows_page = 1;
	$tab_pages = 1 + ceil($rows_rest / $max_rows_page);
	$this->overfl_pages = $tab_pages - 1;
	$this->check_overflow = $tab_pages - 1;
      } else {
	$size_y = $tab_height;
	$rows = $nb_row;
	$rest = 0;
	$tab_pages = 1;
	$max_rows_page = floor(($this->page_size['height'] - ($this->margins['top'] + $this->margins['bottom'])) / $height_row);
      }
      
      $t_f = PDF_load_font($this->pdf_doc, $t_font, "winansi", "");
      $f = PDF_load_font($this->pdf_doc, $font, "winansi", "");
      
      for ($p = 0; $p < $tab_pages; $p++) {	
	if ($p == 0) {
	  $pos_x = $coords[0];
	  $pos_y = $coords[1];
	  $start = 0;
	  $nb_rows = $rows;
	  $end = $rows;
	} else {
	  $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	  if (!strcmp($scope, "page"))
	    PDF_suspend_page($this->pdf_doc, "");
	  PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	  $this->nb_pages++;
	  $this->max_x = $this->margins['left'];
	  $this->min_y = $this->page_size['height'] - $this->margins['top'];
	  $pos_x = $coords[0];
	  $pos_y = $this->page_size['height'] - $this->margins['top'];
	  $start = $end;
	  $rows_rest = $nb_row - $start;
	  if ($rows_rest > $max_rows_page && $overfl_vert != 'false') {
	    if ($p == ($tab_pages - 1)) {
	      $tab_pages++;
	      $this->overfl_pages++;
	      $this->check_overflow++;
	    }
	    $nb_rows = $max_rows_page;
	    $rows_rest -= $nb_rows;
	  } else
	    $nb_rows = $rows_rest;
	  
	  $end = $start + $nb_rows;
	  $rows = $nb_rows;
	}
	
	// couleurs du tableau
	if (is_array($bordercolor)) {
	  $r = round($bordercolor[0] * 100 / 255) / 100;
	  $g = round($bordercolor[1] * 100 / 255) / 100;
	  $b = round($bordercolor[2] * 100 / 255) / 100;
	  PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	}
	if (is_array($fontcolor)) {
	  $r = round($fontcolor[0] * 100 / 255) / 100;
	  $g = round($fontcolor[1] * 100 / 255) / 100;
	  $b = round($fontcolor[2] * 100 / 255) / 100;
	} else {
	  $r = round($this->font_color[0] * 100 / 255) / 100;
	  $g = round($this->font_color[1] * 100 / 255) / 100;
	  $b = round($this->font_color[2] * 100 / 255) / 100;
	}
	$opt_list .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";					
	
	// affichage des données du tableau pour chaque ligne
	$cpt = 0;		
	for ($i = $start; $i < $end; $i++, $cpt++) {
	  $pos_x = $coords[0];
	  reset($data);					
	  if ($i > $start)
	    $this->prec = $tmp_height;
	  $prec_oversize = 1;
	  for ($j = 0; $j < count($data[$i]); $j++) {
	    $str = str_replace('\n', "\n", $data[$i][$j]);
	    $tab_tmp = explode("\n", $str);
	    $nb_line = count($tab_tmp);	
	    
	    if ($j == 0 || $nb_line > $tmp_height)
	      $tmp_height = $nb_line;
	    if (is_array($col_size))
	      $width_col = $col_size[$j];
	    for ($k = 0; $k < $nb_line; $k++) {
	      if ($i == 0)
		$txtlen = ceil(PDF_stringwidth($this->pdf_doc, $tab_tmp[$k], $t_f, $t_size));
	      else
		$txtlen = ceil(PDF_stringwidth($this->pdf_doc, $tab_tmp[$k], $f, $max_col_size));
	      if ($txtlen > ($width_col) && $width_col != 0)
		$oversize = ceil($txtlen / $width_col);
	      else
		$oversize = 1;
	      if ($oversize > $prec_oversize)
		$prec_oversize = $oversize;
	    }
	  }
	  $tmp_height += ($prec_oversize - 1);					
	  if ($i == 0)
	    $h_row = $title_height;
	  else
	    $h_row = $height_row;
	  
	  if (($pos_y - $h_row * $tmp_height) < $this->margins['bottom']) {
	    if ($i == $start) {
	      $rows_rest += $nb_rows;
	      $end = $start;
	    } else {	
	      if ($p == ($tab_pages - 1))
		$rows_rest = ($end - $i);
	      else
		$rows_rest += ($end - $i);
	      if ($p == ($tab_pages - 1) && $rows_rest != 0 && $overfl_vert != 'false')
		$tab_pages++;
	      $end = $i;
	    }
	  } else {
	    // trace une ligne au-dessus du tableau
	    if ($i == $start) {
	      PDF_moveto($this->pdf_doc, $this->margins['left'], $pos_y + ($this->cellspacingV - 2));
	      PDF_lineto($this->pdf_doc, ($this->page_size['width'] - $this->margins['right']), $pos_y + ($this->cellspacingV - 2));
	      PDF_stroke($this->pdf_doc);
	    }

	    // couleur pour une ligne sur 2  -- rgb : 255, 244, 238 
	    // =================================
	    if ($i != 0 && ($i % 2) == 0) {
	      PDF_setcolor($this->pdf_doc, 'fill', 'rgb', round((255 * 100 / 255)) / 100, round((244 * 100 / 255)) / 100, round((238 * 100 / 255)) / 100, 0.0);
	      PDF_rect($this->pdf_doc, $this->margins['left'], ($pos_y - $h_row * $tmp_height), ($this->page_size['width'] - $this->margins['left'] - $this->margins['right']), $h_row * $tmp_height);
	      PDF_fill($this->pdf_doc);
	    }

	    $start_x = $pos_x;						
	    // remplissage avec les données pour chaque cellule
	    for ($j = 0; $j < count($data[$i]); $j++) {
	      if (is_array($col_size))
		$width_col = $col_size[$j];
	      $boxsize = array($width_col, ($h_row * $tmp_height));
	      if (isset($cols_number) && $cols_number != 0 && isset($col_opt[$j])) {
		if ($i == 0) {
		  $txtflow = PDF_create_textflow($this->pdf_doc, str_replace('\n', "\n", $data[$i][$j]), $title_list[$j]);
		  $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $pos_x + 2, $pos_y - $boxsize[1] + 4, $pos_x + $boxsize[0] - 2, $pos_y, $title_valign[$j]);
		} else {
		  $txtflow = PDF_create_textflow($this->pdf_doc, str_replace('\n', "\n", $data[$i][$j]), $col_opt[$j]);
		  $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $pos_x + 2, $pos_y - $boxsize[1] + 4, $pos_x + $boxsize[0] - 2, $pos_y, $valign_col[$j]);
		}
	      } else {
		if ($i == 0) {
		  $txtflow = PDF_create_textflow($this->pdf_doc, str_replace('\n', "\n", $data[$i][$j]), $t_list);
		  $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $pos_x + 2, $pos_y - $boxsize[1] + 4, $pos_x + $boxsize[0] - 2, $pos_y, $t_valign);
		} else {
		  $txtflow = PDF_create_textflow($this->pdf_doc, str_replace('\n', "\n", $data[$i][$j]), $opt_list);
		  $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $pos_x + 2, $pos_y - $boxsize[1] + 4, $pos_x + $boxsize[0] - 2, $pos_y, $valign);
		}
	      }
	      $pos_x += $width_col;
	    }
	    $pos_y -= $h_row * $tmp_height;
	    // trace une ligne sous les titres
	    if ($i == $start && $p == 0) {
	      PDF_moveto($this->pdf_doc, $this->margins['left'], $pos_y);
	      PDF_lineto($this->pdf_doc, ($this->page_size['width'] - $this->margins['right']), $pos_y);
	      PDF_stroke($this->pdf_doc);
	    }
	  } // end else
	} // end for start to end
	// trace une ligne sous le tableau
	PDF_moveto($this->pdf_doc, $this->margins['left'], $pos_y);
	PDF_lineto($this->pdf_doc, ($this->page_size['width'] - $this->margins['right']), $pos_y);
	PDF_stroke($this->pdf_doc);
      } // end for 0 to tab_pages
      
    }
    if ($this->overfl_pages > 0)
      $this->min_overfl = $pos_y;
    if ($this->min_y > $pos_y)
      $this->min_y = $pos_y;
    
    if ((($pos_y - $this->cellspacingV) <= $this->margins['bottom'] + $this->cellspacingV * 2) && $num_vbox < $nb_vbox) {
      $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
      if (!strcmp($scope, "page"))
	PDF_suspend_page($this->pdf_doc, "");
      PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
      $this->nb_pages++;
      $this->max_x = $this->margins['left'];
      $this->min_y = $this->page_size['height'] - $this->margins['top'];
      $this->control_np++;
      $pos_x = $coords[0];
      $pos_y = $this->page_size['height'] - $this->margins['top'];
      $this->current_pos = array($pos_x, $pos_y);
    }
    return array($pos_x + $this->cellspacingH, $pos_y - $this->cellspacingV);
  }
  
  /**
   * Positionne une image
   *
   * $img  : tableau contenant les infos de l'image
   * $pos  : position de placement de l'image
   * $vbox : $vbox['width'] et $vbox['height'] largeur et hauteur de la vbox courante
   *
   */
  function create_image($img, $pos, $vbox) {
    $check_size = '';
    foreach ($img as $k => $attr) {
      switch ($k) {
      case 'VALUE' :
	$image = $attr;
	break;
      case 'SIZE' :
	$s = explode(",", $attr);
	$check_size = array(trim($s[0]), trim($s[1])); // s[0] -> width, s[1] -> height
	break;
      }
    }
    if ($pos[0] >= $this->page_size['width'] - $this->margins['right']) {
      $pos[0] = $this->margins['left'];
      $pos[1] = $this->min_y;
      $this->check_nl = 1;
    }
    // place une image
    if ($stream = fopen($image, 'r')) {
      $im = fread($stream, filesize($image)); 
      fclose($stream);
    } else
      echo "Erreur : impossible d'ouvrir le fichier image : " . $image . "<br>";
    $ext = substr($image, strpos($image, '.') + 1);
    $pvf_filename = substr($image, 0, strpos($image, '.')) ."_tmp.".$ext;
    PDF_create_pvf($this->pdf_doc, $pvf_filename, $im, "");
    if ($ext == "jpg") $type = "jpeg";
    else $type = $ext;
    if (($i = PDF_load_image($this->pdf_doc, $ext, $pvf_filename, "")) == -1)
      echo "Erreur : impossible de charger l'image : " . $image . "<br>";
    $ratio = 1;
    $ratio2 = 0;
    $height = PDF_get_value($this->pdf_doc, "imageheight", $i);
    $width = PDF_get_value($this->pdf_doc, "imagewidth", $i);
    if (is_array($check_size)) {
      if ($check_size[1] != 0)
	$hght = $check_size[1];
      else if ($vbox['height'] != 0)
	$hght = $vbox['height'];
      else
	$hght = $height;
      if ($check_size[0] != 0)
	$wdth = $check_size[0];
      else if ($vbox['width'] != 0)
	$wdth = $vbox['width'];
      else
	$wdth = $width;
      if ($width > $wdth)
	$ratio = $width / $wdth;
      if ($height > $hght)
	$ratio2 = $height / $hght;
    }
    if ($width > ($this->page_size['width'] - ($this->margins['left'] + $this->margins['right']))) {
      $tmp = round($this->page_size['width'] - ($this->margins['left'] + $this->margins['right']));
      $ratio = $width / $tmp;
    }
    if ($height > ($this->page_size['height'] - ($this->margins['top'] + $this->margins['bottom']))) {
      $tmp = round($this->page_size['height'] - ($this->margins['top'] + $this->margins['bottom']));
      $ratio2 = $height / $tmp;
    }
    if ($ratio2 > $ratio)
      $ratio = $ratio2;
    
    if (($pos[1] - ($height / $ratio)) < $this->margins['bottom']) {
      $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
      if (!strcmp($scope, "page"))
	PDF_suspend_page($this->pdf_doc, "");
      PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
      $this->nb_pages++;
      $this->max_x = $this->margins['left'];
      $this->min_y = $this->page_size['height'] - $this->margins['top'];
      $pos[0] = $this->margins['left'];
      $pos[1] = $this->page_size['height'] - $this->margins['top'];
      $this->current_pos = array($pos[0], $pos[1]);
    }
    PDF_fit_image($this->pdf_doc, $i, $pos[0], $pos[1] - ($height / $ratio), "boxsize {". ($width / $ratio) ." ". ($height / $ratio) ."} scale ". (1 / $ratio));
    PDF_delete_pvf($this->pdf_doc, $pvf_filename);
    if ($this->max_x < ($pos[0] + ($width / $ratio) + $this->cellspacingH))
      $this->max_x = $pos[0] + ($width / $ratio) + $this->cellspacingH;
    if ($this->min_y > ($pos[1] - ($height / $ratio)))
      $this->min_y = round($pos[1] - ($height / $ratio));
    if ($this->min_overfl > $this->min_y)
      $this->min_overfl = $this->min_y;
    
    return array(round($pos[0] + ($width / $ratio) + $this->cellspacingH), round($pos[1] - ($height / $ratio)));
  }	
  
  /**
   * Création d'un élément grid
   *
   * $grd  : tableau contenant les infos du grid
   * $pos  : position de placement du grid
   * $vbox : $vbox['width'] et $vbox['height'] largeur et hauteur de la vbox courante
   *
   */
  function create_grid($grd, $pos, $vbox) {
    $tmp_x = $pos[0];
    $tmp_y = $pos[1];
    $min_y = $tmp_y;
    $opt_list = "encoding=winansi";
    $label_list = "encoding=winansi";
    $key_list = "encoding=winansi";

    $check_font = '';
    $check_size = '';
    $check_style = '';
    $fontcolor = '';
    $align = '';
    $label = '';
    $label_font = '';
    $label_color = '';
    $label_size = '';
    $label_style = '';
    $label_align = '';
    $key_font = '';
    $key_color = '';
    $key_size = '';
    $key_style = '';
    $key_align = '';
    $hidden = '';
    foreach ($grd as $k => $attr) {
      switch ($k) {
      case 'VALUE' :
	$data = $attr;
	break;
      case 'LABEL' :
	$label = $attr;
	break;
      case 'FONT' :
	$check_font = $attr;
	break;
      case 'FONTSIZE' :
	$check_size = $attr;
	break;
      case 'FONTCOLOR' :
	$tab = explode(',', $attr);
	$fontcolor = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	  break;
      case 'FONTSTYLE' :
	$check_style = $attr;
	break;
      case 'ALIGN' :
	$align = $attr;
	break;
      case 'LABELFONT' :
	$label_font = $attr;
	break;
      case 'LABELSIZE' :
	$label_size = $attr;
	break;
      case 'LABELSTYLE' :
	$label_style = $attr;
	break;
      case 'LABELCOLOR' :
	$tab = explode(',', $attr);
	$label_color = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	break;
      case 'LABELALIGN' :
	$label_align = $attr;
	break;
      case 'KEYFONT' :
	$key_font = $attr;
	break;
      case 'KEYSIZE' :
	$key_size = $attr;
	break;
      case 'KEYSTYLE' :
	$key_style = $attr;
	break;
      case 'KEYCOLOR' :
	$tab = explode(',', $attr);
	$key_color = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	break;
      case 'KEYALIGN' :
	$key_align = $attr;
	break;
      case 'HIDDEN' :
	$hidden = $attr;
	break;
      }
    }
    if (!empty($check_font)) {
      $font = $check_font;
      $opt_list .= " fontname=" . $check_font;
    } else {
      $font = $this->font_name;
      $opt_list .= " fontname=". $this->font_name;
    }
    if (!empty($check_size)) {
      $size = $check_size;
      $opt_list .= " fontsize=". $check_size;
    } else {
      $size = $this->font_size;
      $opt_list .= " fontsize=". $this->font_size;
    }
    if (!empty($check_style))
      $opt_list .= " fontstyle=" . $check_style;
    if (!empty($align))
      $opt_list .= " alignment=" . $align;
    if (!empty($label_font))
      $label_list .= " fontname=" . $label_font;
    else
      $label_list .= " fontname=" . $font;
    if (!empty($label_style))
      $label_list .= " fontstyle=" . $label_style;
    else if (!empty($check_style))
      $label_list .= " fontstyle=" . $check_style;
    if (!empty($label_size))
      $label_list .= " fontsize=" . $label_size;
    else
      $label_list .= " fontsize=" . $size;
    if (!empty($label_align))
      $label_list .= " alignment=" . $label_align;
    else if (!empty($align))
      $label_list .= " alignment=" . $align;
    if (!empty($key_font))
      $key_list .= " fontname=" . $key_font;
    else
      $key_list .= " fontname=" . $font;
    if (!empty($key_style))
      $key_list .= " fontstyle=" . $key_style;
    else if (!empty($check_style))
      $key_list .= " fontstyle=" . $check_style;
    if (!empty($key_size))
      $key_list .= " fontsize=" . $key_size;
    else
      $key_list .= " fontsize=" . $size;
    if (!empty($key_align))
      $key_list .= " alignment=" . $key_align;
    else if (!empty($align))
      $key_list .= " alignment=" . $align;
    
    // si une cellule se trouve trop près de la marge, on passe à la ligne
    if ($tmp_x > ($this->page_size['width'] - $this->margins['right'] - 5)) {
      if ($this->check_overflow != 0) {
	$scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	if (!strcmp($scope, "page"))
	  PDF_suspend_page($this->pdf_doc, "");
	PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages));
	$this->check_nl = 1;
	$this->prec = 0;
	$min_y = $this->page_size['height'] - $this->margins['top'];
      }
      else
	$this->check_nl = 2;
      $tmp_x = $this->margins['left'];
      $tmp_y = $this->min_y - ($this->cellspacingV);
      $this->current_pos = array($tmp_x, $tmp_y);
      $this->prec_x = $tmp_x;
      $this->start_hbox[0] = $this->margins['left'];
      $this->start_vbox = array($this->margins['left'], $tmp_y);
    }
    if ($tmp_y < $this->margins['bottom'] + $size + $this->cellspacingV) {
      $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
      if (!strcmp($scope, "page"))
	PDF_suspend_page($this->pdf_doc, "");
      PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
      $this->nb_pages++;
      $this->min_y = $this->page_size['height'] - $this->margins['top'];
      $tmp_y = $this->page_size['height'] - $this->margins['top'];
      $min_y = $this->page_size['height'] - $this->margins['top'];
      $this->start_hbox[1] = $tmp_y;
      $this->start_vbox[1] = $tmp_y;
    }
    
    if (is_array($data)) {
      $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
      if (strcmp($scope, "page")) 	{
	PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	$this->nb_pages++;
	$this->min_y = $this->page_size['height'] - $this->margins['top'];
      }
      $f = PDF_load_font($this->pdf_doc, $font, "winansi", "");
      if (is_array($fontcolor)) {
	$r = round($fontcolor[0] * 100 / 255) / 100;
	$g = round($fontcolor[1] * 100 / 255) / 100;
	$b = round($fontcolor[2] * 100 / 255) / 100;
      } else {
	$r = round($this->font_color[0] * 100 / 255) / 100;
	$g = round($this->font_color[1] * 100 / 255) / 100;
	$b = round($this->font_color[2] * 100 / 255) / 100;
      }
      $opt_list .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";
      
      if (is_array($label_color)) {
	$r_l = round($label_color[0] * 100 / 255) / 100;
	$g_l = round($label_color[1] * 100 / 255) / 100;
	$b_l = round($label_color[2] * 100 / 255) / 100;
	$label_list .= " fillcolor={rgb ". $r_l ." ". $g_l ." ". $b_l ."}";
      } else
	$label_list .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";
      if (is_array($key_color)) {
	$r_k = round($key_color[0] * 100 / 255) / 100;
	$g_k = round($key_color[1] * 100 / 255) / 100;
	$b_k = round($key_color[2] * 100 / 255) / 100;
	$key_list .= " fillcolor={rgb ". $r_k ." ". $g_k ." ". $b_k ."}";
      } else
	$key_list .= " fillcolor={rgb ". $r ." ". $g ." ". $b ."}";
      
      $max_key = 0;
      $max_str = 0;
      $the_max = 0;
      $min_y = $tmp_y;
      foreach ($data as $key => $value) {
	$string = str_replace('\n', "\n", $value);
	$key = str_replace('\n', "\n", $key);
	$tab_key = explode("\n", $key);	
	if (count($tab_key) > 1) {
	  $len = 0;
	  for ($j = 0; $j < count($tab_key); $j++) {
	    $tmp_len = ceil(PDF_stringwidth($this->pdf_doc, $tab_key[$j], $f, $size));
	    if ($tmp_len > $len)
	      $len = $tmp_len;
	  }
	} else
	  $len = ceil(PDF_stringwidth($this->pdf_doc, $key, $f, $size));
	if ($len > $max_key)
	  $max_key = $len;
	$tab_str = explode("\n", $string);	
	if (count($tab_str) > 1) {
	  $len_str = 0;
	  for ($j = 0; $j < count($tab_str); $j++) {
	    $tmp_str = ceil(PDF_stringwidth($this->pdf_doc, $tab_str[$j], $f, $size));
	    if ($tmp_str > $len_str)
	      $len_str = $tmp_str;
	  }
	} else
	  $len_str = ceil(PDF_stringwidth($this->pdf_doc, $string, $f, $size));
	if ($len_str > $max_str)
	  $max_str = $len_str;
	
	if ($max_str > $the_max)
	  $the_max = $max_str;
	if ($max_key > $the_max)
	  $the_max = $max_key;
      }
      $check = false;
      if ($tmp_x + $max_key + round($max_str / 2) > $this->page_size['width'] - $this->margins['right']) {
	$this->prec_x = $this->max_x;
	$tmp_x = $this->margins['left'];
	$tmp_y = $this->min_y - 2;
	$min_y = $this->min_y - 2;
	$this->current_pos = array($tmp_x, $tmp_y);
	$this->max_x = $tmp_x + $max_key;
	
	if ($this->prec != 0) {
	  $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	  if (!strcmp($scope, "page"))
	    PDF_suspend_page($this->pdf_doc, "");
	  PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages));
	}
	$check = true;
	$this->prec = 0;
	$this->check_nl = 2;
      }
      $max_x = 0;
      if (!empty($label)) {
	$label = str_replace('\n', "\n", $label);				
	$txtlen = PDF_stringwidth($this->pdf_doc, $label, $f, $size);
	$txtflow = PDF_create_textflow($this->pdf_doc, $label, $label_list);
	$val_x = ceil($txtlen);
	if ($tmp_x + $val_x > $this->page_size['width'] - $this->margins['right'] && $check == false) {
	  $tmp_x = $this->margins['left'];
	  $tmp_y = $this->min_y - 2;
	  $this->current_pos = array($tmp_x, $tmp_y);
	  $this->max_x = $tmp_x + $val_x;
	  if ($this->prec != 0) {
	    $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	    if (!strcmp($scope, "page"))
	      PDF_suspend_page($this->pdf_doc, "");
	    PDF_resume_page($this->pdf_doc, "pagenumber " . $this->nb_pages);
	  }
	  $this->check_overflow = 0;
	}
	// on vérifie si le texte s'étend sur plusieurs lignes
	$tab_str = explode("\n", $label);	
	if (count($tab_str) > 1) {
	  $val_y = $size * count($tab_str);
	  if ($tmp_y - $val_y < $this->margins['bottom'])
	    $end_y = $this->margins['bottom'];
	  else
	    $end_y = $tmp_y - $val_y;
	} else {
	  $val_y = $size;
	  $end_y = $tmp_y - $val_y;
	}
	$end_x = $tmp_x + $max_key + $max_str + $this->cellspacingH;
	if ($vbox['width'] != 0 && $vbox['width'] >= $val_x) {					
	  $end_x = $tmp_x + $vbox['width'];
	  if ($end_x > $this->page_size['width'] - $this->margins['right'])
	    $end_x = $this->page_size['width'] - $this->margins['right'];
	}
	
	if ($end_y < $this->margins['bottom']) {
	  $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	  if (!strcmp($scope, "page"))
	    PDF_suspend_page($this->pdf_doc, "");
	  $this->check_overflow++;
	  $this->overfl_pages++;
	  $min_y = $this->page_size['height'] - $this->margins['top'];
	  if ($this->overfl_pages > $this->prec) {
	    PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	    $this->nb_pages++;
	    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	    $this->min_y = $this->page_size['height'] - $this->margins['top'];
	  } else 
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($this->prec - $this->overfl_pages)));
	  $tmp_y = $min_y;
	  $end_y = $tmp_y - $val_y;
	  if ($this->overfl_pages >= $this->prec && $this->min_overfl < $end_y)
	    $this->min_overfl = $end_y;
	}
	$res = PDF_fit_textflow($this->pdf_doc, $txtflow, $tmp_x, $tmp_y, $end_x, $end_y, "");
	$end_y = PDF_info_textflow($this->pdf_doc, $txtflow, "textendy");
	$val_x = PDF_info_textflow($this->pdf_doc, $txtflow, "textendx");
	if ($end_y < $min_y)
	  $min_y = $end_y;
	$max_label = $val_x;
      } else {
	$min_y = $tmp_y;
	$max_label = 0;
      }
      $min_y -= ($this->cellspacingV / 3);
      foreach ($data as $key => $value) {
	$string = str_replace('\n', "\n", $value);
	$key_str = str_replace('\n', "\n", $key);
	$tab_key = explode("\n", $key_str);
	$tab_str = explode("\n", $string);
	if (count($tab_key) > 1) {
	  $val_key = $size * count($tab_key);
	  if ($min_y - $val_key < $this->margins['bottom'])
	    $end_key = $this->margins['bottom'];
	  else
	    $end_key = $min_y - $val_key;
	} else {
	  $val_key = $size;
	  $end_key = $min_y - $val_key;
	}
	if (count($tab_str) > 1) {
	  $val_str = $size * count($tab_str);
	  if ($min_y - $val_str < $this->margins['bottom'])
	    $end_str = $this->margins['bottom'];
	  else
	    $end_str = $min_y - $val_str;
	} else {
	  $val_str = $size;
	  $end_str = $min_y - $val_str;
	}
	if ($val_str > $val_key) {
	  $val_y = $val_str;
	  $end_y = $end_str;
	} else {
	  $val_y = $val_key;
	  $end_y = $end_key;
	}
	
	if ($end_y < $this->margins['bottom']) {
	  $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	  if (!strcmp($scope, "page"))
	    PDF_suspend_page($this->pdf_doc, "");
	  $this->check_overflow++;
	  $this->overfl_pages++;
	  $min_y = $this->page_size['height'] - $this->margins['top'];
	  if ($this->overfl_pages > $this->prec || ($check == true)) {
	    PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	    $this->nb_pages++;
	    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	    $this->min_y = $this->page_size['height'] - $this->margins['top'];
	  } else
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($this->prec - $this->overfl_pages)));
	  $tmp_y = $min_y;
	  $end_y = $tmp_y - $val_y;
	  if ($this->overfl_pages >= $this->prec && $this->min_overfl < $end_y)
	    $this->min_overfl = $end_y;
	}
	$txtflow_key = PDF_create_textflow($this->pdf_doc, $key_str, $key_list);
	$res_key = PDF_fit_textflow($this->pdf_doc, $txtflow_key, $tmp_x, $min_y, $tmp_x + $max_key, $end_y, "");
	$txtflow_str = PDF_create_textflow($this->pdf_doc, $string, $opt_list);
	$pos_x = $tmp_x + $max_key + $this->cellspacingH;
	if ($pos_x + $max_str > $this->page_size['width'] - $this->margins['right']) {
	  $max_str = $this->page_size['width'] - $this->margins['right'] - $pos_x;
	  $end_y -= $size;
	}
	$res_str = PDF_fit_textflow($this->pdf_doc, $txtflow_str, $pos_x, $min_y, $pos_x + $max_str, $end_y, "");
	$end_key = PDF_info_textflow($this->pdf_doc, $txtflow_key, "textendy");
	$end_str = PDF_info_textflow($this->pdf_doc, $txtflow_str, "textendy");
	$end_y = min($end_key, $end_str);
	if ($end_y < $min_y)
	  $min_y = $end_y;
	$max_x = $max_str;
	$min_y -= ($this->cellspacingV / 3);
	if (strcmp($res_key, "_stop")) {
	  $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	  $this->check_overflow++;
	  $this->overfl_pages++;
	  PDF_suspend_page($this->pdf_doc, "");
	  if ($this->overfl_pages > $this->prec) {
	    PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	    $this->nb_pages++;
	    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	    $this->min_y = $this->page_size['height'] - $this->margins['top'];
	    $min_y = $this->page_size['height'] - $this->margins['top'];
	  } else
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($this->prec - $this->overfl_pages)));
	  $val = max($val_key, $val_str);
	  $end_y = $this->min_overfl - $val;
	  $res_key = PDF_fit_textflow($this->pdf_doc, $txtflow_key, $tmp_x, $this->min_overfl, $tmp_x + $max_key, $end_y, "");
	  $end_key = PDF_info_textflow($this->pdf_doc, $txtflow_key, "textendy");
	  if (strcmp($res_str, "_stop")) {
	    $res_str = PDF_fit_textflow($this->pdf_doc, $txtflow_str, $tmp_x + $max_key + $this->cellspacingH, $this->min_overfl, $tmp_x + $max_key + $this->cellspacingH + $max_str, $end_y, ""); 
	    $end_str = PDF_info_textflow($this->pdf_doc, $txtflow_str, "textendy");
	    $end_y = min($end_key, $end_str);
	  } else
	    $end_y = $end_key;
	  $min_y = $end_y - ($this->cellspacingV / 3);
	  if ($this->min_overfl > $min_y && $this->overfl_pages >= $this->prec)
	    $this->min_overfl = $min_y;
	} else if (strcmp($res_str, "_stop")) {
	  $this->check_overflow++;
	  $this->overfl_pages++;
	  if ($this->overfl_pages > $this->prec && $end_y <= ($this->margins['bottom'] + $size)) {
	    PDF_suspend_page($this->pdf_doc, "");
	    PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	    $this->nb_pages++;
	    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	    $this->min_y = $this->page_size['height'] - $this->margins['top'];
	    $min_y = $this->page_size['height'] - $this->margins['top'];
	    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	  } else if ($end_y <= ($this->margins['bottom'] + $size)) {
	    PDF_suspend_page($this->pdf_doc, "");
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($this->prec - $this->overfl_pages)));
	    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	    $min_y = $this->min_overfl;
	  } else {
	    $this->check_overflow--;
	    $this->overfl_pages--;
	    $this->min_overfl = $end_y;
	    $end_y -= $size * 2;
	  }
	  $res_str = PDF_fit_textflow($this->pdf_doc, $txtflow_str, $tmp_x + $max_key + $this->cellspacingH, $min_y, $tmp_x + $max_key + $this->cellspacingH + $max_str, $end_y, "");
	  $end_y = PDF_info_textflow($this->pdf_doc, $txtflow_str, "textendy"); 
	  $min_y = $end_y - ($this->cellspacingV / 3);
	  if ($this->min_overfl > $min_y && $this->overfl_pages >= $this->prec)
	    $this->min_overfl = $min_y;
	}
      }
      if ($vbox['width'] != 0 && $vbox['width'] >= ($max_key + $max_str)) {
	$end_x = $tmp_x + $vbox['width'];
	if ($end_x > $this->page_size['width'] - $this->margins['right'])
	  $end_x = $this->page_size['width'] - $this->margins['right'];
      }
      if ($vbox['height'] != 0 && $vbox['height'] > ($pos[1] - max($end_str, $end_key)) && $this->overfl_pages == 0) {
	if ($tmp_y - $vbox['height'] < $this->margins['bottom'])
	  $vbox['height'] = $tmp_y - $this->margins['bottom'];
	$min_y = $tmp_y - $vbox['height'];
      }
      if ($max_label > $pos_x + $max_x)
	$pos_x = $max_label + $this->cellspacingH;
      else
	$pos_x += $max_x + $this->cellspacingH;
      $pos_y = $min_y - ($this->cellspacingV / 3);
      if ($this->overfl_pages >= $this->prec) {
	if ($this->min_y > $pos_y)
	  $this->min_y = $pos_y;
	if ($this->min_overfl > $pos_y)
	  $this->min_overfl = $pos_y;
      }

      // trace une ligne sous la box
      if (!empty($hidden) && $hidden == 'grid_total') {
	PDF_moveto($this->pdf_doc, $tmp_x, $pos_y);
	PDF_lineto($this->pdf_doc, ($this->page_size['width'] - $this->margins['right']), $pos_y);
	PDF_stroke($this->pdf_doc);
	$pos_y -= $this->cellspacingV;
      }
    } else {
      $pos_x = $tmp_x;
      $pos_y = $tmp_y;
    }
    return array($pos_x, $pos_y);
  }
  
  /**
   * Parse l'élément Page de l'XML et construit la page
   *
   * $arr_fact : tableau contenant la structure de la page
   *
   */
  function parse_page($arr_fact) {

    $this->min_y = $this->current_pos[1]; // la position minimal (en vertical, y) du curseur de position sur une hbox
    $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
    // parcours de toutes les hbox d'une page
    if (is_array($arr_fact['HBOX']) && !empty($arr_fact['HBOX'])) foreach ($arr_fact['HBOX'] as $hbox) {
      // on replace le curseur de position sur la gauche de la page 
      // à chaque nouvelle hbox (si aucune position n'est donnée)
      $this->min_y -= $this->cellspacingV;
      $this->current_pos = array($this->margins['left'], $this->min_y);
      $this->check_overflow = 0;
      $this->overfl_pages = 0;
      $this->prec = 0;
      $hbox_overfl = 0;
      $prec_overfl = 0;
      $border_row = 'false';
      $stroke = '';
      $pos_hbox = '';
      $hbox_size = array(0, 0);
      $control_prec = 0;
      // recuperation des attrributs de la Hbox
      if (is_array($hbox['ATTRIBUTS'])) {
	foreach ($hbox['ATTRIBUTS'] as $k => $attr) {
	  switch ($k) {
	  case 'POSITION' :
	    $tab = explode(',', $attr);
	    $pos_hbox = array(trim($tab[0]), trim($tab[1])); 
	    break;
	  case 'BORDER' :
	    $border_row = $attr;
	    break;
	  case 'BORDERCOLOR' :
	    $tab = explode(',', $attr);
	    $stroke = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	    break;
	  case 'WIDTH' :
	    $hbox_size['width'] = $attr;
	    break;
	  case 'HEIGHT' :
	    $hbox_size['height'] = $attr;
	    break;
	  }
	} // end foreach attribut
      } // end if is_array
      
      // parcours des fils d'une Hbox
      $nb_vbox = count($hbox['VBOX']);
      $num_vbox = 1;
      $this->control_np = 0;
      $this->max_x = $this->current_pos[0];
      if (is_array($pos_hbox)) {
	if ($pos_hbox[0] != 0 && $pos_hbox[0] > $this->current_pos[0])
	  $this->start_hbox[0] = $pos_hbox[0];
	else
	  $this->start_hbox[0] = $this->current_pos[0];
	if ($pos_hbox[1] != 0 && $pos_hbox[1] < $this->current_pos[1])
	  $this->start_hbox[1] = $pos_hbox[1];
	else
	  $this->start_hbox[1] = $this->current_pos[1];
      } else
	$this->start_hbox = $this->current_pos;
      // parcours de toutes les vbox contenues dans une Hbox
      foreach ($hbox['VBOX'] as $k => $vbox) {
	// a chaque nouvelle vbox, on déplace le curseur de position en horizontal (x)
	// (si aucune position n'est donnée)
	$this->current_pos = array($this->max_x, $this->current_pos[1]);
	$pos = '';
	$border = 'false';
	$bordercolor = '';
	$this->check_nl = 0;
	$vbox_size = array(0, 0);
	// recuperation des attrributs de la Vbox
	if (is_array($vbox['ATTRIBUTS'])) {
	  foreach ($vbox['ATTRIBUTS'] as $k => $attr) {
	    switch ($k) {
	    case 'POSITION' :
	      $tab = explode(',', $attr);
	      $pos = array(trim($tab[0]), trim($tab[1]));
	      break;
	    case 'BORDER' :
	      $border = $attr;
	      break;
	    case 'BORDERCOLOR' :
	      $tab = explode(',', $attr);
	      $bordercolor = array(trim($tab[0]), trim($tab[1]), trim($tab[2]));
	      break;
	    case 'WIDTH' :
	      $vbox_size['width'] = $attr;
	      break;
	    case 'HEIGHT' :
	      $vbox_size['height'] = $attr;
	      break;
	    }
	  } // end foreach attribut
	} // end if is_array
	
	// parcours des fils d'une Vbox
	if ($this->check_overflow != 0 && $this->control_np == 0) {
	  $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	  if (!strcmp($scope, "page"))
	    PDF_suspend_page($this->pdf_doc, "");
	  $p = $this->nb_pages - ($this->nb_pages - $this->overfl_pages - 1);
	  if ($p > $this->nb_pages) {
	    PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	    $this->nb_pages++;
	    $this->check_overflow++;
	    $this->overfl_pages++;
	  } else {
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - $this->prec));
	    if ($this->overfl_pages != 0)
	      $this->min_y = $this->min_overfl - $this->cellspacingV;
	  }
	} else if ($this->prec == 0) {
	  $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	  if (!strcmp($scope, "page")) {
	    PDF_suspend_page($this->pdf_doc, "");
	    PDF_resume_page($this->pdf_doc, "pagenumber " . $this->nb_pages);
	  }
	}
	if ($this->control_np != 0)
	  $control_prec = $this->control_np;

	$this->nb_elt = count($vbox);
	$this->num_elt = 1;
	$tmp = array($this->current_pos[0], $this->current_pos[1]);
	if ($control_prec != 0)
	  $tmp = array($this->current_pos[0], $this->min_y);
	if (is_array($pos_hbox) && $num_vbox == 1) {
	  if ($pos_hbox[0] != 0 && $tmp[0] < $pos_hbox[0])
	    $tmp[0] = $pos_hbox[0];
	  if ($pos_hbox[1] != 0 && $tmp[1] > $pos_hbox[1])
	    $tmp[1] = $pos_hbox[1];
	  $this->current_pos = $tmp;
	}
	if (is_array($pos)) {
	  if ($pos[0] != 0 && $tmp[0] < $pos[0])
	    $tmp[0] = $pos[0];
	  if ($pos[1] != 0 && $tmp[1] > $pos[1])
	    $tmp[1] = $pos[1];
	} else
	  $pos = $tmp;
	$this->start_vbox = $tmp;
	// Parcours des elements
	foreach ($vbox as $elt) {
	  $this->prec_x = $tmp[0];
	  switch ($elt['TYPE']) {
	  case 'Text' :
	    $tmp = $this->create_txt_area($elt, $tmp, $num_vbox, $nb_vbox, $vbox_size);
	    break;
	  case 'Table' :
	    $tmp = $this->create_table($elt, $tmp, $num_vbox, $nb_vbox);
	    break;
	  case 'Image' :
	    $tmp = $this->create_image($elt, $tmp, $vbox_size);
	    break;
	  case 'Grid' :
	    $tmp = $this->create_grid($elt, $tmp, $vbox_size);
	    break;
	  default :
	    break;
	  }
	  $this->num_elt++;
	  if ($this->overfl_pages > $this->prec)
	    $this->min_y = $this->min_overfl - $this->cellspacingV;
	  else if ($this->overfl_pages == 0 && ($this->prec == 0 || $this->control_np != 0) && $tmp[1] < $this->min_y)
	    $this->min_y = $tmp[1];
	  if (($this->overfl_pages >= $this->prec) && ($this->min_overfl < $this->min_y) && $this->overfl_pages != 0)
	    $this->min_y = $this->min_overfl;
	  if ($tmp[0] > $this->max_x || $this->max_x > $this->page_size['width'] - $this->margins['right'])
	    $this->max_x = $tmp[0];
	  $end_tmp_x = $tmp[0];
	  $tmp[0] = $this->prec_x;
	  
	} // end foreach elt
	if ($this->check_nl == 1 && $tmp[1] < $this->min_y)
	  $this->min_y = $tmp[1];
	
	// si une taille a été définie pour la Vbox
	if ($vbox_size['width'] != 0) {
	  if ($this->start_vbox[0] + $vbox_size['width'] > $this->page_size['width'] - $this->margins['right'])
	    $vbox_size['width'] = $this->page_size['width'] - $this->margins['right'] - $this->start_vbox[0];
	  $tmp[0] = $this->start_vbox[0] + $vbox_size['width'];
	  if ($tmp[0] > $this->max_x)
	    $this->max_x = $tmp[0];
	}
	if ($vbox_size['height'] != 0 && $this->start_vbox[1] - $vbox_size['height'] < $tmp[1] && $this->overfl_pages == 0) {
	  if ($this->start_vbox[1] - $vbox_size['height'] < $this->margins['bottom'])
	    $vbox_size['height'] = $this->start_vbox[1] - $this->margins['bottom'];
	  $tmp[1] = $this->start_vbox[1] - $vbox_size['height'];
	  $this->min_y = $tmp[1];
	}
	
	// tracé d'un cadre de Vbox
	if (!strcmp($border, 'true')) {
	  // couleur du tracé
	  if (is_array($bordercolor)) {
	    $r = round($bordercolor[0] * 100 / 255) / 100;
	    $g = round($bordercolor[1] * 100 / 255) / 100;
	    $b = round($bordercolor[2] * 100 / 255) / 100;
	  } else {
	    $r = round($this->font_color[0] * 100 / 255) / 100;
	    $g = round($this->font_color[1] * 100 / 255) / 100;
	    $b = round($this->font_color[2] * 100 / 255) / 100;
	  }
	  PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	  if ($this->check_nl == 2) {
	    $start_x = $this->current_pos[0] - 2;
	    $start_y = $this->current_pos[1] + 2;
	  } else {
	    $start_x = $this->start_vbox[0] - 2;
	    $start_y = $this->start_vbox[1] + 2;
	  }
	  if ($this->check_nl != 0)
	    $end_x = $end_tmp_x - 2;
	  else
	    $end_x = $this->max_x - 2;
	  $end_y = $tmp[1] - 2;
	  if ($this->overfl_pages != 0) {
	    $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	    if (!strcmp($scope, "page"))
	      PDF_suspend_page($this->pdf_doc, "");
	    if ($this->prec < $this->overfl_pages)
	      $this->prec = $this->overfl_pages;
	    $prec_overfl = $this->overfl_pages;
	    if ($this->check_overflow == 1)
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - $prec_overfl));
	    else
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - $this->prec));
	    PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	    $end_y = $this->margins['bottom'];
	    if ($start_y <= ($this->margins['bottom']) && $this->overfl_pages == 1) {
	      PDF_suspend_page($this->pdf_doc, "");
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages));
	      $start_y = $this->page_size['height'] - $this->margins['top'];
	      $end_y = $tmp[1];
	    }
	    // line top
	    PDF_moveto($this->pdf_doc, $start_x, $start_y);
	    PDF_lineto($this->pdf_doc, $end_x, $start_y);
	    // line left
	    //PDF_moveto($this->pdf_doc, $start_x, $start_y);
	    //PDF_lineto($this->pdf_doc, $start_x, $end_y);
	    // line right
	    //PDF_moveto($this->pdf_doc, $end_x, $start_y);
	    //PDF_lineto($this->pdf_doc, $end_x, $end_y);
	    PDF_stroke($this->pdf_doc);
	    if ($this->check_overflow == 1)
	      $p = $this->nb_pages - $prec_overfl + 1;
	    else
	      $p = $this->nb_pages - $this->prec + 1;
	    for ($i = 0; $i < $this->overfl_pages - 1; $i++) {
	      PDF_suspend_page($this->pdf_doc, "");
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($i + $p));
	      PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	      $end_y = $this->margins['bottom'];
	      $start_y = $this->page_size['height'] - $this->margins['top'];
	      // line right
	      //PDF_moveto($this->pdf_doc, $end_x, $start_y);
	      //PDF_lineto($this->pdf_doc, $end_x, $end_y);
	      // line left
	      //PDF_moveto($this->pdf_doc, $start_x, $start_y);
	      //PDF_lineto($this->pdf_doc, $start_x, $end_y);
	      PDF_stroke($this->pdf_doc);
	    }
	    PDF_suspend_page($this->pdf_doc, "");
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($i + $p));
	    PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	    if ($tmp[1] > ($this->page_size['height'] - $this->margins['top'] - $this->font_size)) {
	      if ($this->overfl_pages >= 1) {
		PDF_suspend_page($this->pdf_doc, "");
		PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - 1));
	      } else
		$start_y = $this->page_size['height'] - $this->margins['top'];
	      $end_y = $this->margins['bottom'] - 2;
	    } else {
	      $end_y = $tmp[1];
	      $start_y = $this->page_size['height'] - $this->margins['top'];
	    }
	    // line left
	    //PDF_moveto($this->pdf_doc, $start_x, $start_y);
	    //PDF_lineto($this->pdf_doc, $start_x, $end_y);
	    // line right
	    //PDF_moveto($this->pdf_doc, $end_x, $start_y);
	    //PDF_lineto($this->pdf_doc, $end_x, $end_y);
	    // line bottom
	    PDF_moveto($this->pdf_doc, $start_x, $end_y);
	    PDF_lineto($this->pdf_doc, $end_x, $end_y);
	    PDF_stroke($this->pdf_doc);
	    $end_y = $tmp[1];
	    if (($i + $p) < $this->nb_pages) {
	      PDF_suspend_page($this->pdf_doc, "");
	      PDF_resume_page($this->pdf_doc, "pagenumber " . $this->nb_pages);
	    }
	    $this->min_y -= $this->cellspacingV;
	  } else {
	    // line top
	    PDF_moveto($this->pdf_doc, $start_x, $start_y);
	    PDF_lineto($this->pdf_doc, $end_x, $start_y);
	    // line left
	    //PDF_moveto($this->pdf_doc, $start_x, $start_y);
	    //PDF_lineto($this->pdf_doc, $start_x, $end_y);
	    // line right
	    //PDF_moveto($this->pdf_doc, $end_x, $start_y);
	    //PDF_lineto($this->pdf_doc, $end_x, $end_y);
	    // line bottom
	    PDF_moveto($this->pdf_doc, $start_x, $end_y);
	    PDF_lineto($this->pdf_doc, $end_x, $end_y);
	    PDF_stroke($this->pdf_doc);
	  }
	}
	if ($this->prec < $this->overfl_pages)
	  $this->prec = $this->overfl_pages;
	if ($hbox_overfl < $this->overfl_pages)
	  $hbox_overfl = $this->overfl_pages;
	$prec_overfl = $this->overfl_pages;
	$this->overfl_pages = 0;
	$num_vbox++;
	if ($this->max_x > $this->page_size['width'] - $this->margins['right'])
	  $this->max_x = $this->page_size['width'] - $this->margins['right'];
      } // end foreach vboxes
      
      // si une taille a été définie pour la Hbox
      if ($hbox_size['width'] != 0) {
	if ($this->start_hbox[0] + $hbox_size['width'] > $this->page_size['width'] - $this->margins['right'])
	  $hbox_size['width'] = $this->page_size['width'] - $this->margins['right'] - $this->start_hbox[0];
	$tmp[0] = $this->start_hbox[0] + $hbox_size['width'];
	if ($tmp[0] > $this->max_x)
	  $this->max_x = $tmp[0];
      }
      if ($hbox_size['height'] != 0 && $this->start_hbox[1] - $hbox_size['height'] < $tmp[1] && $hbox_overfl == 0) {
	if (($this->start_hbox[1] - $hbox_size['height']) < ($this->margins['bottom']))
	  $hbox_size['height'] = $this->start_hbox[1] - $this->margins['bottom'];
	$tmp[1] = $this->start_hbox[1] - $hbox_size['height'];
	$this->min_y = $tmp[1];
      }
      
      // tracé d'un cadre de Hbox
      if (!strcmp($border_row, 'true')) {
	// couleur du tracé
	if (is_array($stroke)) {
	  $r = round($stroke[0] * 100 / 255) / 100;
	  $g = round($stroke[1] * 100 / 255) / 100;
	  $b = round($stroke[2] * 100 / 255) / 100;
	} else {
	  $r = round($this->font_color[0] * 100 / 255) / 100;
	  $g = round($this->font_color[1] * 100 / 255) / 100;
	  $b = round($this->font_color[2] * 100 / 255) / 100;
	}
	PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	if (is_array($pos_hbox) && $this->check_nl != 2) {
	  if ($pos_hbox[0] != 0 && $this->start_hbox[0] < $pos_hbox[0])
	    $start_x = $pos_hbox[0] - 2;
	  else
	    $start_x = $this->start_hbox[0] - 2;
	  if ($pos_hbox[1] != 0 && $this->start_hbox[1] > $pos_hbox[1])
	    $start_y = $pos_hbox[1] + 2;
	  else
	    $start_y = $this->start_hbox[1] + 2;
	} else {
	  $start_x = $this->start_hbox[0] - 2;
	  $start_y = $this->start_hbox[1] + 2;
	}
	if ($control_prec != 0)
	  $end_x = $this->page_size['width'] - $this->margins['right'];
	else
	  $end_x = $this->max_x - 2;
	$end_y = $this->min_y - 2;
	if ($hbox_overfl != 0) {
	  $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	  if (!strcmp($scope, "page"))
	    PDF_suspend_page($this->pdf_doc, "");
	  if ($this->check_nl == 2) {
	    $end_x = $this->prec_x;
	    $hbox_overfl += $this->prec;
	  }
	  PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - $hbox_overfl));
	  PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	  $end_y = $this->margins['bottom'];
	  if ($start_y <= ($this->margins['bottom'] + $this->cellspacingH) && $hbox_overfl >= 1) {
	    PDF_suspend_page($this->pdf_doc, "");
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - ($hbox_overfl - 1)));
	    $start_y = $this->page_size['height'] - $this->margins['top'];
	    $end_y = $this->min_y;
	  }
	  // line top
	  PDF_moveto($this->pdf_doc, $start_x, $start_y);
	  PDF_lineto($this->pdf_doc, $end_x, $start_y);
	  // line left
	  PDF_moveto($this->pdf_doc, $start_x, $start_y);
	  PDF_lineto($this->pdf_doc, $start_x, $end_y);
	  // line right
	  PDF_moveto($this->pdf_doc, $end_x, $start_y);
	  PDF_lineto($this->pdf_doc, $end_x, $end_y);
	  PDF_stroke($this->pdf_doc);
	  $p = $this->nb_pages - $hbox_overfl + 1;
	  for ($i = 0; $i < $hbox_overfl - 1; $i++) {
	    PDF_suspend_page($this->pdf_doc, "");
	    PDF_resume_page($this->pdf_doc, "pagenumber " . ($i + $p));
	    PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	    $end_y = $this->margins['bottom'];
	    $start_y = $this->page_size['height'] - $this->margins['top'];
	    // line right
	    PDF_moveto($this->pdf_doc, $end_x, $start_y);
	    PDF_lineto($this->pdf_doc, $end_x, $end_y);
	    // line left
	    PDF_moveto($this->pdf_doc, $start_x, $start_y);
	    PDF_lineto($this->pdf_doc, $start_x, $end_y);
	    PDF_stroke($this->pdf_doc);
	  }
	  PDF_suspend_page($this->pdf_doc, "");
	  PDF_resume_page($this->pdf_doc, "pagenumber " . ($i + $p));
	  PDF_setcolor($this->pdf_doc, "stroke", "rgb", $r, $g, $b, 0);
	  if ($this->min_y > ($this->page_size['height'] - $this->margins['top'] - $this->font_size)) {
	    if ($hbox_overfl == 1) {
	      PDF_suspend_page($this->pdf_doc, "");
	      PDF_resume_page($this->pdf_doc, "pagenumber " . ($this->nb_pages - $hbox_overfl));
	    } else
	      $start_y = $this->page_size['height'] - $this->margins['top'];
	    $end_y = $this->margins['bottom'] - 2;
	  } else {
	    $end_y = $this->min_y;
	    $start_y = $this->page_size['height'] - $this->margins['top'];
	  }
	  
	  // line left
	  PDF_moveto($this->pdf_doc, $start_x, $start_y);
	  PDF_lineto($this->pdf_doc, $start_x, $end_y);
	  // line right
	  PDF_moveto($this->pdf_doc, $end_x, $start_y);
	  PDF_lineto($this->pdf_doc, $end_x, $end_y);
	  // line bottom
	  PDF_moveto($this->pdf_doc, $start_x, $end_y);
	  PDF_lineto($this->pdf_doc, $end_x, $end_y);
	  PDF_stroke($this->pdf_doc);
	  
	  PDF_suspend_page($this->pdf_doc, "");
	  PDF_resume_page($this->pdf_doc, "pagenumber " . $this->nb_pages);
	} else {
	  // line top
	  PDF_moveto($this->pdf_doc, $start_x, $start_y);
	  PDF_lineto($this->pdf_doc, $end_x, $start_y);
	  // line left
	  PDF_moveto($this->pdf_doc, $start_x, $start_y);
	  PDF_lineto($this->pdf_doc, $start_x, $end_y);
	  // line right
	  PDF_moveto($this->pdf_doc, $end_x, $start_y);
	  PDF_lineto($this->pdf_doc, $end_x, $end_y);
	  // line bottom
	  PDF_moveto($this->pdf_doc, $start_x, $end_y);
	  PDF_lineto($this->pdf_doc, $end_x, $end_y);
	  PDF_stroke($this->pdf_doc);
	}
      }
      //$this->min_y -= $this->cellspacingV;
      if ($this->min_y < ($this->margins['bottom'] + $this->font_size + $this->cellspacingV)) {
	$scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
	if (!strcmp($scope, "page"))
	  PDF_suspend_page($this->pdf_doc, "");
	$this->min_y = $this->page_size['height'] - $this->margins['top'];
	if ($this->overfl_pages > $this->prec) {
	  PDF_begin_page_ext($this->pdf_doc, $this->page_size['width'], $this->page_size['height'], "");
	  $this->nb_pages++;
	  $this->min_overfl = $this->page_size['height'] - $this->margins['top'];
	  $this->check_overflow++;
	  $this->overfl_pages++;
	}
      }
      $end_y = $this->min_y;				
    } // end foreach hboxes
    $scope = PDF_get_parameter($this->pdf_doc, "scope", 0);
    if (!strcmp($scope, "page"))
      PDF_suspend_page($this->pdf_doc, "");
  }// end parse_page function
}

?>