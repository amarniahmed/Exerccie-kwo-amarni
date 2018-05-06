<?php

class Chart {

  private $IMG;
  private $BOX;

  private $COL_BG;
  private $COL_BORDER;
  private $COL_AXES;
  private $COL_VALUES;
  private $COL_BAR;
  private $COL_BAR_BORDER;
  private $COL_LINES;
  private $COL_BAR_SHADOW;
  private $COL_TITLE;

  private $VALUES		= array();
  private $NB_BARS		= 0;
  private $MAX_VAL		= 0;
  private $COMPVALUES           = array();
  private $COMPARISON           = FALSE;

  private $WIDTH		= 450;
  private $HEIGHT		= 150;
  //  private $PADDING		= 15;
  private $PADDING              = 1;
  private $PADDING_KEYS         = 15;
  private $PADDING_BAR          = 3;
  private $TITLE                = "";
  private $TITLE_SIZE           = 0;
  private $FONT_FILE            = '';
  private $FONT_SIZE            = 8;
  private $FONT_SIZE_KEYS	= 7;
  private $SHADOW_SIZE          = 0;

  private $ALTERNATE_VAL	= FALSE;
  private $ALTERNATE_KEY	= TRUE;

  private $ALTERNATE_VAL_START  = 0;
  private $ALTERNATE_KEY_START  = 0;

  private $VISIBLE_VAL          = TRUE;
  private $VISIBLE_KEY          = TRUE;

  private $SHOW_ONE_VAL         = FALSE;

  private $font_support         = false;

  public function __construct($width = 0,$height = 0,$padding = 1) {
    $this->FONT_FILE = WEB_PATH.'/core/fonts/DroidSans.ttf';
    if ($padding > 1) $this->PADDING = $padding;
    if ($width > 0)   $this->WIDTH = $width-($this->PADDING*2);
    if ($height > 0)  $this->HEIGHT = $height-($this->PADDING*2);

    $this->font_support = function_exists('imagettftext');

    // Image
    $this->IMG = imagecreatetruecolor($this->WIDTH,$this->HEIGHT);
    $this->BOX = imagecreatetruecolor($this->WIDTH+($this->PADDING*2),$this->HEIGHT+($this->PADDING*2));

    // Colors
    $this->COL_BG         = imagecolorallocate($this->IMG,255,255,255);
    $this->COL_BORDER     = imagecolorallocate($this->IMG,255,255,255);
    $this->COL_AXES       = imagecolorallocate($this->IMG,139,137,137);
    $this->COL_KEYS       = imagecolorallocate($this->IMG,175,175,175);
    $this->COL_VALUES     = imagecolorallocate($this->IMG,189,165,024);
    $this->COL_BAR        = imagecolorallocate($this->IMG,235,235,235);
    $this->COL_BAR_BORDER = imagecolorallocate($this->IMG,210,210,210);
    $this->COL_LINES      = imagecolorallocate($this->IMG,235,235,235);
    $this->COL_BAR_SHADOW = imagecolorallocate($this->IMG,210,210,210);
    $this->COL_TITLE	  = imagecolorallocate($this->IMG,175,175,175);
    $this->COL_COMP	  = imagecolorallocate($this->IMG,210,210,210);
    $this->COL_PADDING    = imagecolorallocate($this->BOX,255,255,255);
  }
  public function SetColorBg ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_BG = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorBorder ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_BORDER = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorAxes ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_AXES = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorKeys ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_KEYS = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorValues ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_VALUES = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorBar ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_BAR = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorBarBorder ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_BAR_BORDER = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorLines ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_LINES = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorBarShadow ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_BAR_SHADOW = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorTitle ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_TITLE = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorComp ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_COMP = imagecolorallocate($this->IMG,$myR,$myG,$myB);
  }
  public function SetColorPadding ($myCol) {
    sscanf($myCol, "%2x%2x%2x", $myR, $myG, $myB);
    $this->COL_PADDING = imagecolorallocate($this->BOX,$myR,$myG,$myB);
  }
  public function SetValues($myVALUES) {
    $this->VALUES = $myVALUES;
    $this->NB_BARS = count($this->VALUES);
    $this->MAX_VAL = $this->ArrayMax($this->VALUES);
  }
  public function SetCompValues($myCOMPVALUES) {
    $this->COMPVALUES = $myCOMPVALUES;
    $this->MAX_VAL = $this->ArrayMax($this->COMPVALUES);
    $this->COMPARISON = TRUE;
  }
  public function SetAlternateValues ($myBool, $myStart = 0) {
    $this->ALTERNATE_VAL = $myBool;
    $this->ALTERNATE_VAL_START = $myStart;
  }
  public function SetAlternateKeys ($myBool, $myStart = 0) {
    $this->ALTERNATE_KEY = $myBool; 
    $this->ALTERNATE_KEY_START = $myStart;
  }
  public function SetVisibleValues($myBool) {
    $this->VISIBLE_VAL = $myBool;
  }
  public function SetVisibleKeys($myBool) {
    $this->VISIBLE_KEY = $myBool;
  }
  public function SetPadding($myPADDING) {
    $this->PADDING = ($myPADDING+1);
  }
  public function SetPaddingKeys($myPADDING_KEYS) {
    $this->PADDING_KEYS = $myPADDING_KEYS;
  }
  public function SetTitle($myTITLE) {
    $this->TITLE = $myTITLE;
    $this->TITLE_SIZE = 7;
    $this->TITLE_HEIGHT = 13;
  }
  public function SetTitleSize($myTITLE_SIZE) {
    $this->TITLE_SIZE = $myTITLE_SIZE;
    $this->TITLE_HEIGHT = $this->TITLE_SIZE+$this->TITLE_SIZE/2+1;
  }
  public function SetFontFile($myFONT_FILE) {
    $this->FONT_FILE = $myFONT_FILE;
  }
  public function SetFontSize($myFONT_SIZE) {
    $this->FONT_SIZE = $myFONT_SIZE;
  }
  public function SetShadowSize($mySHADOW_SIZE) {
    $this->SHADOW_SIZE = $mySHADOW_SIZE;
  }
  public function SetShowOneValue($myBool) {
    $this->SHOW_ONE_VAL = $myBool;
  }
  public function GenImg() {
    if (!$this->MAX_VAL) $this->MAX_VAL = 1;

    imageantialias($this->IMG, TRUE);

    // Background
    imagefill($this->IMG,0,0,$this->COL_BG);
    imagefill($this->BOX,0,0,$this->COL_PADDING);

    // Header
    if ($this->font_support) {
      $size = ImageFtBbox($this->TITLE_SIZE, -90, $this->FONT_FILE, $this->TITLE, array("linespacing" => 1));
      imagettftext($this->IMG, $this->TITLE_SIZE, -90, $this->WIDTH-$this->FONT_SIZE-2, 
                   ($this->HEIGHT/2)-($size[3]/2), $this->COL_TITLE, $this->FONT_FILE, $this->TITLE);
    }

    // Chart Sizes
    $this->PADDING_KEYS = $this->FONT_SIZE+$this->FONT_SIZE/2+3;
    $this->PADDING_BAR = (($this->WIDTH)/$this->NB_BARS)/6;
    $this->MAX_BAR_WIDTH = ($this->WIDTH-$this->TITLE_HEIGHT)/$this->NB_BARS;
    $this->MAX_BAR_HEIGHT = $this->HEIGHT-$this->PADDING_KEYS-3;
    
    $x1 = $this->PADDING_BAR;
    $x2 = $this->MAX_BAR_WIDTH-$this->PADDING_BAR;
    $y2 = $this->HEIGHT-$this->PADDING_KEYS;

    $max_size = 0;
    if ($this->SHOW_ONE_VAL == FALSE) {
      foreach ($this->VALUES as $key => $val) {
        if ($this->font_support) {
          $f_size = ImageFtBbox($this->FONT_SIZE, 0, $this->FONT_FILE, $val, array("linespacing" => 1));
        }
	$b_size = $x2 - $x1;
	if ($f_size[4] > $b_size) {
	  if ($f_size[4] + $this->MAX_BAR_HEIGHT*($val/$this->MAX_VAL) + 4 > $max_size) {
	    $max_size = $f_size[4] + $this->MAX_BAR_HEIGHT*($val/$this->MAX_VAL) + 4;
	  }
	} else {
	  if ($this->FONT_SIZE + $this->MAX_BAR_HEIGHT*($val/$this->MAX_VAL) + 4 > $max_size) {
	    $max_size = $this->FONT_SIZE + $this->MAX_BAR_HEIGHT*($val/$this->MAX_VAL) + 4;
	  }
	}
      }
      if ($max_size > $this->MAX_BAR_HEIGHT) {
	$this->MAX_BAR_HEIGHT -= $max_size-$this->MAX_BAR_HEIGHT;
      }
    }
    else {
      $this->MAX_BAR_HEIGHT+3;
    }
    
    // Lines
    imageline($this->IMG,0,$this->HEIGHT-$this->PADDING_KEYS-(1/4)*$this->MAX_BAR_HEIGHT,$this->WIDTH-$this->TITLE_HEIGHT,
	      $this->HEIGHT-$this->PADDING_KEYS-(1/4)*$this->MAX_BAR_HEIGHT,$this->COL_LINES);
    imageline($this->IMG,0,$this->HEIGHT-$this->PADDING_KEYS-(2/4)*$this->MAX_BAR_HEIGHT,$this->WIDTH-$this->TITLE_HEIGHT,
	      $this->HEIGHT-$this->PADDING_KEYS-(2/4)*$this->MAX_BAR_HEIGHT,$this->COL_LINES);
    imageline($this->IMG,0,$this->HEIGHT-$this->PADDING_KEYS-(3/4)*$this->MAX_BAR_HEIGHT,$this->WIDTH-$this->TITLE_HEIGHT,
	      $this->HEIGHT-$this->PADDING_KEYS-(3/4)*$this->MAX_BAR_HEIGHT,$this->COL_LINES);
    imageline($this->IMG,0,$this->HEIGHT-$this->PADDING_KEYS-(4/4)*$this->MAX_BAR_HEIGHT,$this->WIDTH-$this->TITLE_HEIGHT,
	      $this->HEIGHT-$this->PADDING_KEYS-(4/4)*$this->MAX_BAR_HEIGHT,$this->COL_LINES);
    
    // Chart Bars
    $i = $this->ALTERNATE_VAL_START;
    $j = $this->ALTERNATE_KEY_START;
    foreach ($this->VALUES as $key => $val) {
      
      // Bars
      $y1 = $y2-($this->MAX_BAR_HEIGHT*($val/$this->MAX_VAL));
      if ($y2 > $y1) {
	imagefilledrectangle($this->IMG, $x1+$this->SHADOW_SIZE, $y1+$this->SHADOW_SIZE, 
			     $x2+$this->SHADOW_SIZE, $y2, $this->COL_BAR_SHADOW);
	imagefilledrectangle($this->IMG, $x1, $y1, $x2, $y2, $this->COL_BAR);
	imagerectangle($this->IMG, $x1, $y1, $x2, $y2, $this->COL_BAR_BORDER);
      }
      
      // Comparison
      if ($this->COMPARISON == TRUE) {
	if ($i != 0) {
	  $comp_old_x = $comp_x;
	  $comp_old_y = $comp_y;
	}
	$comp_x = $x1+($x2-$x1)/2;
	$comp_y = $y2-($this->MAX_BAR_HEIGHT*($this->COMPVALUES[$key]/$this->MAX_VAL));
	if ($i != 0) {
	  imageline($this->IMG,$comp_old_x,$comp_old_y,$comp_x,$comp_y,$this->COL_COMP);
	}
	imagefilledellipse($this->IMG, $comp_x, $comp_y, 5, 5, $this->COL_COMP);
      }

      // Axes Lines
      imageline($this->IMG,$x1+($x2-$x1)/2,$this->HEIGHT-$this->PADDING_KEYS,$x1+($x2-$x1)/2,$this->HEIGHT-$this->PADDING_KEYS-2,$this->COL_AXES);

      // Values
      if ($this->font_support && $this->VISIBLE_VAL == TRUE && $val != 0 && $this->SHOW_ONE_VAL == FALSE) {
	if ($this->ALTERNATE_VAL == FALSE || $i%2 == 0) {
          if ($this->font_support) {
            $f_size = ImageFtBbox($this->FONT_SIZE, 0, $this->FONT_FILE, $val, array("linespacing" => 1));
          }
	  if ($f_size[4] < $b_size) {
            $x3 = $x1 + ($b_size-$f_size[4])/2;
	    $y3 = $y1-2;
	    imagettftext($this->IMG, $this->FONT_SIZE, 0, $x3, $y3, $this->COL_VALUES, $this->FONT_FILE, $val);
	  } else {
	    $f_size = ImageFtBbox($this->FONT_SIZE, 90, $this->FONT_FILE, $val, array("linespacing" => 1));
            $x3 = $x1 + ($b_size-$f_size[4])/2 + 1;
	    $y3 = $y1-2;
	    imagettftext($this->IMG, $this->FONT_SIZE, 90, $x3, $y3, $this->COL_VALUES, $this->FONT_FILE, $val);
	  }
	}
      }

      // Keys
      if ($this->font_support && $this->VISIBLE_KEY == TRUE) {
	if ($this->ALTERNATE_KEY == FALSE || $j%2 == 0) {
	  $b_size = $x2 - $x1;
	  $f_size = ImageFtBbox($this->FONT_SIZE_KEYS, 0, $this->FONT_FILE, $key, array("linespacing" => 1));
	  $x3 = $x1 + ($b_size-$f_size[4])/2;
	  $y3 = $this->HEIGHT-($this->PADDING_KEYS-($this->FONT_SIZE_KEYS+4));
	  imagettftext($this->IMG, $this->FONT_SIZE_KEYS, 0, $x3, $y3, $this->COL_KEYS, $this->FONT_FILE, $key);
	}
      }
      $x1 += $this->MAX_BAR_WIDTH;
      $x2 += $this->MAX_BAR_WIDTH;
      $i++;
      $j++;
    }
    if ($this->font_support && $this->SHOW_ONE_VAL == TRUE) {
      $f_size = ImageFtBbox($this->FONT_SIZE, 0, $this->FONT_FILE, $this->MAX_VAL, array("linespacing" => 1));
      imagettftext($this->IMG, $this->FONT_SIZE, 0, 2,$this->HEIGHT-($this->MAX_BAR_HEIGHT+$this->PADDING_KEYS)+$this->FONT_SIZE+2,
		   $this->COL_VALUES, $this->FONT_FILE, $this->MAX_VAL);
    }
    // Axes
    imageline($this->IMG,0,$this->HEIGHT-$this->PADDING_KEYS,$this->WIDTH-$this->TITLE_HEIGHT,$this->HEIGHT-$this->PADDING_KEYS,$this->COL_AXES);

    // Img Border
    imagerectangle($this->BOX,0,0,$this->WIDTH+($this->PADDING*2)-1,$this->HEIGHT+($this->PADDING*2)-1,$this->COL_BORDER);

    // Insert IMG in BOX
    imagecopy ($this->BOX,$this->IMG, $this->PADDING, $this->PADDING, 0, 0, $this->WIDTH, $this->HEIGHT);
  }
  public function DisplayPNG() {
    $this->GenImg();
    header("Content-type: image/png");
    imagepng($this->BOX);
  }
  //  public function DisplayGIF() {
  //    $this->GenImg();
  //    header("Content-type: image/gif");
  //    imagegif($this->BOX);
  //  }
  public function DisplayJPG() {
    $this->GenImg();
    header("Content-type: image/jpg");
    imagejpeg($this->BOX);
  }
  public function Distroy() {
    imagedestroy($this->IMG);
    imagedestroy($this->BOX);
  }

  public function ArrayMax($tab = NULL) {
    if ((!is_array($tab)) || (!count($tab) > 0))  return 0;
    unset($tab_tmp);
    foreach($tab as $key => $value) {
      if ((is_numeric($value)) || (is_int($value))) {
        $tab_tmp[$key] = $value;
      }
    }
    if (is_array($tab_tmp)) {
      sort($tab_tmp, SORT_NUMERIC);
      return array_pop($tab_tmp);
    } 
    else {
      return 0; //die("Error in array_max.\n");
    }
  }


}
