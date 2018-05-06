<?php

  /***
http://cweiske.de/tagebuch/Importing%20huge%20XML%20files%20using%20PHP5%20-%20efficiently%20and%20conveniently.htm
  ***/

class XmlIterator implements Iterator
{
  protected $strFile = null;

  protected $reader = null;
 
  protected $program = null;

  protected $nKey = null;
  
  protected $strObjectTagname = 'program';
    
  public function __construct($strFile, $tag) {
    $this->strFile = $strFile;
    $this->strObjectTagname = $tag;
  }
  
  public function current() {
    return $this->program;
  }
  
  public function key() {
    return $this->nKey;
  }
  
  public function next() {
    $this->program = null;
  }
  
  public function rewind() {
    $this->reader = new XMLReader();
    $this->reader->open($this->strFile);
    $this->program = null;
    $this->nKey    = null;
  }
  
  public function valid() {
    if ($this->program === null) {
      $this->loadNext();
    }
    
    return $this->program !== null;
  }
  
  protected function loadNext() {
    $strElementName = null;
    $bCaptureValues = false;
    $arValues       = array();
    $arNesting      = array();
    
    while ($this->reader->read()) {
      switch ($this->reader->nodeType) {
      case XMLReader::ELEMENT:
        $strElementName = $this->reader->name;
        if ($bCaptureValues) {
          if ($this->reader->isEmptyElement) {
            $arValues[$strElementName] = null;
          } 
          else {
            $arNesting[] = $strElementName;
            $arValues[implode('-', $arNesting)] = null;
          }
        }
        if ($strElementName == $this->strObjectTagname) {
          $bCaptureValues = true;
        }
        break;
        
      case XMLReader::TEXT:
        if ($bCaptureValues) {
          $arValues[implode('-', $arNesting)] = $this->reader->value;
        }
        break;
        
      case XMLReader::END_ELEMENT:
        if ($this->reader->name == $this->strObjectTagname) {
          $this->program = $arValues;
          ++$this->nKey;
          break 2;
        }
        if ($bCaptureValues) {
          array_pop($arNesting);
        }
        break;
      }
    }
  }
}
