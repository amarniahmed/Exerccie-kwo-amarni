<?php

// Content-Type: text/csv; charset=x-UTF-16LE-BOM
// BOM UTF-16LE "\xff\xfe"
// $res->write("\xff\xfe".mb_convert_encoding($string, 'UTF-16LE', 'UTF-8'));
// BOM UTF-8
// $res->write("\xef\xbb\xbf".$string);

class CsvWriter 
{
  private $columns = null;
  private $delimiter = null;
  private $eol = null;
  private $encoding = null;
  private $index = 1;
  private $fd = null;
  private $separator = null;

  public function __construct($opts) {
    $this->delimiter = $opts['delimiter'] ?: '"';
    $this->eol = $opts['eol'] ?: "\r\n";
    $this->fd = $opts['fd'] ?: S('res');
    $this->encoding = $opts['encoding']
                    ? strtoupper($opts['encoding'])
                    : null;
    $this->separator = $opts['separator'] ?: ';';
    if ($opts['filename']) {
      $this->header($opts['filename']);
    }
    if ($this->encoding === 'UTF-8') {
      $this->fd->write("\xef\xbb\xbf");
    }
    elseif ($this->encoding === 'UTF-16LE') {
      //$res->write("\xff\xfe"); Hugues correction
      $this->fd->write("\xff\xfe");
    }
    if (isset($opts['columns'])) {
      $this->columns = $opts['columns'];
      if (is_array($opts['columns'])) {
        $this->append($opts['columns']);
      }
    }
  }
  
  public function append($line) { 
    if ($this->index++ == 1 && $this->columns === true) {
      $this->append(array_keys($line));
    }
    foreach ($line as $key => $value) {
      $is_quoted = false;
      if (strpos($value, $this->delimiter) !== false) {
        $value = str_replace($this->delimiter, 
                             $this->delimiter.$this->delimiter, 
                             $value);
        $is_quoted = true;
      }
      if (strpos($value, $this->separator) !== false || 
          strpos($value, chr(10)) !== false) {
        $is_quoted = true;
      }
      if ($is_quoted) {
        $value = $this->delimiter.$value.$this->delimiter;
      }
      elseif (is_numeric($value) && strlen($value) > 11) {
        $value = '='.$this->delimiter.$value.$this->delimiter;
      }
      $line[$key] = $value;
    }
    $line = implode($this->separator, $line);
    if ($this->encoding === 'UTF-16LE') {
      $line = mb_convert_encoding($line, 'UTF-16LE', 'UTF-8');
    }
    return $this->fd->write($line.$this->eol);
  }

  private function header($filename) {
    $res = S('res');
    $res->close();
    $filename  = empty($filename) ? 'export' : $filename;
    $filename .= stripos($filename, '.csv') === false ? '.csv' : '';
    $res->setHeader('Content-Disposition', 'attachment; '.
                    'filename="'.$filename.'"');
    $res->setHeader('Content-Transfer-Encoding', 'binary');
    if ($this->encoding === 'UTF-16LE') {
      $res->setHeader('Content-Type', 'text/csv; charset=x-UTF-16LE-BOM');
    }
    else {
      $res->setContentType('csv');
    }
    $res->setHeader('Accept-Ranges', 'bytes');
    $res->setHeader('Pragma', 'public');
  }

  public static function getInstance($opts) {
    return new CsvWriter($opts);
  }

}


// http://en.wikipedia.org/wiki/Byte_order_mark