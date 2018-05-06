<?php

class PDFDocument
{
  var $name;
  var $creator;
  var $author;
  var $title;
  var $pdf_doc;
  var $chapter_list;
  var $pages_list;
  var $contxt;
  var $nb_pages;

  function PDFDocument($name, $author, $creator, $title) {
    dl("libpdf_php.so");
    $this->pdf_doc = PDF_new();
    PDF_set_parameter($this->pdf_doc, "license", "L700102-010500-730492-4TKQ22-9KK2E2");

    //  Ouvre un nouveau fichier PDF
    if (PDF_begin_document($this->pdf_doc, "", "") == 0)
      die("Erreur : " . PDF_get_errmsg($this->pdf_doc));
    $this->name = $name;
    $this->creator = $creator;
    $this->author = $author;
    $this->title = $title;
    $this->chapter_list = array();
    $this->contxt = $ctxt;
    $this->nb_pages = 0;

    PDF_set_info($this->pdf_doc, "Creator", $this->creator);
    PDF_set_info($this->pdf_doc, "Author", $this->author);
    PDF_set_info($this->pdf_doc, "Title", $this->title);

  }

  // fermeture du document pdf
  function end_doc() {
    PDF_end_document($this->pdf_doc, "");

    if (!($buf = PDF_get_buffer($this->pdf_doc)))
      die("Erreur buffer : " . PDF_get_errmsg($this->pdf_doc));

    ob_end_clean();

    header('Content-type: application/pdf');
    header('Content-Disposition: attachment; filename="facture.pdf"');
    header("Content-Transfer-Encoding: binary");
    header("Accept-Ranges: bytes");
    header("Pragma: public");

    //header("Content-Type: application/pdf");
    //header("Content-Disposition: inline; filename='".$this->name."'");
    //header("Content-Transfer-Encoding: binary");
    //header("Accept-Ranges: bytes");
    //header("Pragma: public");

    PDF_delete($this->pdf_doc);

    die($buf);
  }

  function save_doc($filename){
    PDF_end_document($this->pdf_doc, "");

    if (!($buf = PDF_get_buffer($this->pdf_doc)))
      die("Erreur buffer : " . PDF_get_errmsg($this->pdf_doc));
    ob_end_clean();
    $h = fopen($filename,"w");
    if(!fputs($h,$buf)) die('Can\'t write on file : '.$filename);
    fclose($h);
    PDF_delete($this->pdf_doc);
  }
  function add_page($page) {
    $this->chapter_list[] = $page;
  }

  function add_chapter($arr_fact) {
    $pages_list = array();

    foreach($arr_fact['PAGE'] as $page) {

      // init vars (attributes)
      $name = '';
      $font = '';
      $fontcolor = '';
      $cellsp = '';
      // recuperation des attrributs de la page
      if (!empty($page['NAME'])) $name = $page['NAME'];
      if (!empty($page['FONT'])) $font = $page['FONT'];
      if (!empty($page['MARGINS'])) {
	$tab = sscanf($page['MARGINS'], "left:%d right:%d top:%d bottom:%d");
	$margins = array(ltrim($tab[0]), ltrim($tab[1]), ltrim($tab[2]), ltrim($tab[3]));
      }
      if (!empty($page['FONTCOLOR'])) {
	$tab = explode(',', $page['FONTCOLOR']);
	$fontcolor = array(ltrim($tab[0]), ltrim($tab[1]), ltrim($tab[2]));
      }
      if (!empty($page['CELLSPACING'])) {
	$tab = explode(',', $page['CELLSPACING']);
	$cellsp = array(ltrim($tab[0]), ltrim($tab[1]));
      }
      
      // creation d'une nouvelle page
      $pages_list[] = new PDFPage($this->pdf_doc, $name, $this->nb_pages, $font, $fontcolor, $margins);
      if (is_array($cellsp)) $pages_list[(count($pages_list) - 1)]->set_cellspacing(ltrim($cellsp[0]), ltrim($cellsp[1]));
      $pages_list[(count($pages_list) - 1)]->parse_page($page);
      $prec = $this->nb_pages;
      $this->nb_pages = $pages_list[(count($pages_list) - 1)]->get_nb_pages();
      
      for ($i = $prec + 1; $i <= $this->nb_pages; $i++) {
	PDF_resume_page($this->pdf_doc, "pagenumber ". $i);
	
	// Creation d'un footer
	// ========================
	if (is_array($page['FOOTER']) && !empty($page['FOOTER'])) {
	  $opt_list = "encoding=winansi leading=120%";
	  
	  if (!empty($page['FOOTER']['FONTSIZE'])) $size = $page['FOOTER']['FONTSIZE'];
	  else $size = "7";
	  $opt_list .= " fontsize=".$size;
	  
	  if (!empty($page['FOOTER']['FONTNAME'])) $font = $page['FOOTER']['FONTNAME'];
	  else $font = "vera";
	  $opt_list .= " fontname=".$font;
	  
	  PDF_set_parameter($this->pdf_doc, "SearchPath", "media/font");
	  if (false == ($f = PDF_load_font($this->pdf_doc, $font, "host", "errorpolicy=return")))
	    die("Erreur : " . PDF_get_errmsg($this->pdf_doc));
	  PDF_setfont($this->pdf_doc, $f, $size);
	  
	  if (!empty($page['FOOTER']['ALIGN'])) $opt_list .= " alignment=".$page['FOOTER']['ALIGN'];
	  if (!empty($page['FOOTER']['FONTSTYLE'])) $opt_list .= " fontstyle=".$page['FOOTER']['FONTSTYLE'];
	  
	  if (!empty($page['FOOTER']['POSITION'])) {
	    $tab = explode(',', $page['FOOTER']['POSITION']);
	    $pos = array(trim($tab[0]), trim($tab[1]));
	  } else $pos = array(60, 60);
	  
	  $txtlen = PDF_stringwidth($this->pdf_doc, $page['FOOTER']['VALUE'], $f, $size);
	  $txtflow = PDF_create_textflow($this->pdf_doc, $page['FOOTER']['VALUE'], $opt_list);
	  $txtlen = ceil($txtlen);
	  $res = PDF_fit_textflow($this->pdf_doc, $txtflow, $pos[0], $pos[1], ($pos[0] + $txtlen), '10', "");
	  PDF_delete_textflow($this->pdf_doc, $txtflow);
	}
	
	PDF_end_page_ext($this->pdf_doc, "");
      }
    }
    // rattache les pages au chapitre dans la liste de tous les chapitres du document
    $this->add_page($pages_list);
    
  } // end add_chapter function
}

?>
