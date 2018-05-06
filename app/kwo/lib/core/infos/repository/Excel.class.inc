<?php

class Excel {
  private $author;
  private $date;
  private $company;
  private $sheets;

  function __construct () {
    $this->author    = "Kernix Web Office";
    $this->date      = date("Y-m-d")."T".date("H:i:s")."Z";
    $this->company   = "Kernix Software";
    $this->sheets    = array();
  }

  function addWorksheet ($name, $arr) {
    $this->sheets[$name] = $arr;
  }

  function getContent () {
    $ret  = '';
    $ret .= "<?xml version=\"1.0\"?>\n";
    $ret .=  "<?mso-application progid=\"Excel.Sheet\"?>\n";

    $ret .= "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
    $ret .= "xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n";
    $ret .= "xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n";
    $ret .= "xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
    $ret .= "xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n";
    $ret .= "  <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n";
    $ret .= "    <Author>".$this->author."</Author>\n";
    $ret .= "    <LastAuthor>".$this->author."</LastAuthor>\n";
    $ret .= "    <Created>".$this->date."</Created>\n";
    $ret .= "    <LastSaved>".$this->date."</LastSaved>\n";
    $ret .= "    <Company>".$this->company."</Company>\n";
    $ret .= "  </DocumentProperties>\n";
    $ret .= "  <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n";
    $ret .= "    <WindowHeight>10000</WindowHeight>\n";
    $ret .= "    <WindowWidth>10000</WindowWidth>\n";
    $ret .= "    <WindowTopX>0</WindowTopX>\n";
    $ret .= "    <WindowTopY>0</WindowTopY>\n";
    $ret .= "    <ProtectStructure>False</ProtectStructure>\n";
    $ret .= "    <ProtectWindows>False</ProtectWindows>\n";
    $ret .= "  </ExcelWorkbook>\n";
    $ret .= "  <Styles>\n";
    $ret .= "    <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n";
    $ret .= "      <Alignment ss:Vertical=\"Bottom\"/>\n";
    $ret .= "      <Borders/>\n";
    $ret .= "      <Font/>\n";
    $ret .= "      <Interior/>\n";
    $ret .= "      <NumberFormat/>\n";
    $ret .= "      <Protection/>\n";
    $ret .= "    </Style>\n";
    $ret .= "    <Style ss:ID=\"Grey\" ss:Name=\"Grey\">\n";
    $ret .= "      <Interior ss:Color=\"#C0C0C0\" ss:Pattern=\"Solid\"/>\n";
    $ret .= "    </Style>\n";
    $ret .= "  </Styles>\n";
    foreach ($this->sheets as $name => $sheet) {
      $ret .= "  <Worksheet ss:Name=\"".$name."\">\n";
      $ret .= "    <Table ss:ExpandedColumnCount=\"".count($sheet[0])."\"\n";
      $ret .= "    ss:ExpandedRowCount=\"".count($sheet)."\"\n";
      $ret .= "    x:FullColumns=\"1\"\n";
      $ret .= "    x:FullRows=\"1\">\n";
      foreach ($sheet as $row) {
	$ret .= "      <Row>\n";
	$style = '';
	foreach ($row as $cell) {
	  if (is_int($cell) || is_float($cell)) $type = 'Number';
	  else {
	    $type = 'String';
	    $cell = htmlspecialchars($cell);
            //	    $cell = utf8_encode($cell);
	  }
	  //	  elseif (Valid::isDate($cell)) $type = 'DateTime';
	  if (substr($cell,0,2)=='__') {
	    $style = ' ss:StyleID="Grey"';
	    $cell = substr($cell,2);
	  }
	  $ret .= '        <Cell'.$style.'><Data ss:Type="'.$type.'">'.$cell.'</Data></Cell>'."\n";
	}
	
	$ret .= "      </Row>\n";
      }
      $ret .= "    </Table>\n";
      $ret .= "    <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n";
      $ret .= "      <ProtectObjects>False</ProtectObjects>\n";
      $ret .= "      <ProtectScenarios>False</ProtectScenarios>\n";
      $ret .= "    </WorksheetOptions>\n";
      $ret .= "  </Worksheet>\n";
    }
    $ret .= "</Workbook>\n";
    return $ret;
  }  

}
