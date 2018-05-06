<?php

class Video {
  
  protected $source = null;
  protected $destination = null;
  protected $metas = array();
  
  public function __construct($source) {
    $this->source = $source;
    $file = new File($this->source);
    $lines = explode("\n",shell_exec('ffmpeg -i "'.$this->source.'" 2>&1'));
    foreach ($lines as $line) {
      if (strpos($line,"nput #0")) {
        $input = explode(',',$line);
        $this->metas['container'] = trim($input[1]);
      } else if (strpos($line,"Stream #0")) {
        $stream = explode(': ',$line);
        if ($stream[1] == 'Video') {
          $tmp = explode(',',trim($stream[2]));
          $codec = trim($tmp[0]);
          $type = $tmp[1];
          list($dimensions,$aspect) = explode(' [',trim($tmp[2]));
          list($width,$height) = explode('x',$dimensions);
          list(,,,$aspect) = explode(' ',str_replace(']','',$aspect));
          $this->metas['video'] = array('codec'=>trim($codec),
                                        'type'=>trim($type),
                                        'width'=>trim($width),
                                        'height'=>trim($height),
                                        'anamorphic' => false);
          if (strpos($aspect,':')) {
            $this->metas['video']['aspect'] = trim($aspect);
            $ratio = trim($width) / trim($height);
            if (trim($aspect) == '16:9' && 
                $ratio < 1.555555556) {
              $this->metas['video']['anamorphic'] = true;
            }
          }       
        } else if ($stream[1] == 'Audio') {
          $tmp = explode(',',trim($stream[2]));
          $this->metas['audio'] = array('codec'=>trim($tmp[0]),
                                        'bitrate'=>(int)trim($tmp[4]),
                                        'type'=>trim($tmp[2]));
        }
      }
    }
  }
  
  public function getMetas () {
    return $this->metas;
  }
  
  public function getDimensions () {
    return array('width' => $this->metas['video']['width'],
                 'height' => $this->metas['video']['height']);
  }
  
  public function encode ($destination, $opts=array()) {
    $container = '';
    if ($opts['container']) {
      $container = $opts['container'];
    } else {
      $info = pathinfo($destination);
      $container = $info['extension'];
    }
    $vb = '';
    $size = '';
    
    $width = $this->metas['video']['width'];
    $height = $this->metas['video']['height'];
    
    if ($opts['video']) {
      if ($opts['video']['bitrate']) {
        $vb = $opts['video']['bitrate'];
        $vb = '-b '.$vb.' -bt '.$vb.' -minrate '.$vb.' -maxrate '.$vb;
      }
      
      if ($this->metas['video']['anamorphic'] == true) {
        list($wa,$ha) = explode(':',trim($this->metas['video']['aspect']));
        $ratio = $wa / $ha;
        $width = round($height * $ratio);
      }
      
      if ($opts['video']['maxwidth'] && $opts['video']['maxwidth'] < $width) {
        $height = round($opts['video']['maxwidth'] / (( $width )/$height));
        $width = $opts['video']['maxwidth'];
        if ($height == (int)$height && $height%2 != 0) {
          $height = (int)$height - 1;
        } else if (($height > (int)$height && (int)$height%2 != 0)) {
          $height = (int)$height + 1;
        } else if ($height < (int)$height && (int)$height%2 != 0) {
          $height = (int)$height - 1;
        }
        $size = '-s '.$width.'x'.$height; //.' -vf "scale='.$width.':'.$height.'"';
        if ($this->metas['video']['aspect'] != '') {
          $size .= ' -aspect '.$this->metas['video']['aspect'];
        }
      }
    }
    $ab = '';
    if ($opts['audio']) {
      if ($opts['audio']['bitrate']) {
        if ((int)$opts['audio']['bitrate'] <= $this->metas['audio']['bitrate']) {
          $ab = $opts['audio']['bitrate'];
          $ab = '-ab '.$ab;
        } else {
          $ab = '-ab '.($this->metas['audio']['bitrate']/2).'k';
        }
      }
    }
    error_log('ffmpeg -loglevel 0 -y -i "'.escapeshellcmd($this->source).'" -vcodec libx264 -vpre normal -vpre main '.$vb.' '.$size.' -ilme -acodec libfaac '.$ab.' -threads 0 -r 25 "'.escapeshellcmd($destination).'" 1>/dev/null 2>/dev/null');
    shell_exec('ffmpeg -loglevel 0 -y -i "'.escapeshellcmd($this->source).'" -vcodec libx264 -vpre normal -vpre main '.$vb.' '.$size.' -acodec libfaac '.$ab.' -threads 0 -r 25 "'.escapeshellcmd($destination).'" 1>/dev/null 2>/dev/null');
    if ($opts['container'] == 'mp4') {
      error_log('qt-faststart "'.escapeshellcmd($destination).'" "'.escapeshellcmd($destination).'.tmp" && mv "'.escapeshellcmd($destination).'.tmp" "'.escapeshellcmd($destination).'"');
      shell_exec('qt-faststart "'.escapeshellcmd($destination).'" "'.escapeshellcmd($destination).'.tmp" && mv "'.escapeshellcmd($destination).'.tmp" "'.escapeshellcmd($destination).'"');
    }
  }
  
  /*** STATIC ***/
  
  public static function getInstance($filename) {
    return new Video($filename);
  }
  
}
