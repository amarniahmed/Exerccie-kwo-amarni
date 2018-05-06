<?php

class Stylesheet extends ActiveRecord {

  const TAG_TEXT = 0;
  const TAG_LINK = 1;
  const TAG_TR = 2;
  const TAG_TD = 3;
  const TAG_TABLE = 4;

  public static $fonts = array('sans-serif' => 'sans-serif',
                               'serif' => 'serif',
                               'monospace' => 'monospace',
                               'cursive' => 'cursive',
                               'fantasy' => 'fantasy');

  public static $transforms = array('capitalize' => 'capitalize',
                                    'uppercase' => 'uppercase',
                                    'lowercase' => 'lowercase');

  public static $aligns = array('left' => 'gauche',
                                'right' => 'droite',
                                'center' => 'centré',
                                'justify' => 'justifié');

  public static $decorations = array('underline' => 'underline',
                                     'overline' => 'overline',
                                     'line-through' => 'line-through',
                                     'blink' => 'blink');

  public static $weights = array('normal' => 'normal',
                                 'bold' => 'bold',
                                 'bolder' => 'bolder',
                                 'lighter' => 'lighter');

  public static $styles = array('normal' => 'normal',
                                'italic' => 'italic',
                                'oblique' => 'oblique');

  public static $tag_types = array(self::TAG_TEXT => 'texte',
                                   self::TAG_LINK => 'lien',
                                   self::TAG_TR => 'ligne de tableau',
                                   self::TAG_TD => 'cellule de tableau',
                                   self::TAG_TABLE => 'tableau');

  protected function onBeforeCommit(&$hash) {
    if (isset($hash['code']) && empty($hash['code'])) {
      throw new Exception('code required');
    }
  }

  protected function onAfterCommit($hash) {
    $this->writeFile();
  }

  public function css() {
    $css = '';
    if (!empty($this->background)) {
      $css .= ' background:'.$this->background.';';
    }
    if (!empty($this->color)) {
      $css .= ' color:'.$this->color.';';
    }
    if (!empty($this->font)) {
      $css .= ' font-family:'.$this->font.';';
    }
    if (!empty($this->style)) {
      $css .= ' font-style:'.$this->style.';';
    }
    if (!empty($this->weight)) {
      $css .= ' font-weight:'.$this->weight.';';
    }
    if (!empty($this->letter_spacing)) {
      $css .= ' letter-spacing:'.$this->letter_spacing.';';
    }
    if (!empty($this->size)) {
      $css .= ' font-size:'.$this->size.';';
    }
    if (!empty($this->decoration)) {
      $css .= ' text-decoration:'.$this->decoration.';';
    }
    if (!empty($this->transform)) {
      $css .= ' text-transform:'.$this->transform.';';
    }
    if (!empty($this->align)) {
      $css .= ' text-align:'.$this->align.';';
    }
    if (!empty($this->border)) {
      $css .= ' border:'.$this->border.';';
    }
    if (!empty($this->padding)) {
      $css .= ' padding:'.$this->padding.';';
    }
    if (!empty($this->margin)) {
      $css .= ' margin:'.$this->margin.';';
    }
    return $css;
  }

  public function tag() {
    if ($this->tag_type == self::TAG_TR) return 'TR';
    if ($this->tag_type == self::TAG_TD) return 'TD';
    if ($this->tag_type == self::TAG_TABLE) return 'TABLE';
    return '';
  }

  public static function getByTag($type=0) {
    return S('dbo')->query('SELECT code, name'
                           .' FROM '.T('stylesheet')
                           .' WHERE tag_type='.Stylesheet::TAG_LINK
                           .' ORDER BY name')->asHash();
  }

  private function writeFile() {
    $css = '';
    foreach (Stylesheet::collect('stylesheet') as $stylesheet) {
      $css .= '.kwo-'.$stylesheet->code;
      if (in_array($stylesheet->tag_type, array(self::TAG_TR, self::TAG_TABLE))) {
        $css .= ' TD';
      }
      $css .= ' {';
      if (!empty($stylesheet->background)) {
        $css .= ' background:'.$stylesheet->background.';';
      }
      if (!empty($stylesheet->color)) {
        $css .= ' color:'.$stylesheet->color.';';
      }
      if (!empty($stylesheet->font)) {
        $css .= ' font-family:'.$stylesheet->font.';';
      }
      if (!empty($stylesheet->style)) {
        $css .= ' font-style:'.$stylesheet->style.';';
      }
      if (!empty($stylesheet->weight)) {
        $css .= ' font-weight:'.$stylesheet->weight.';';
      }
      if (!empty($stylesheet->letter_spacing)) {
        $css .= ' letter-spacing:'.$stylesheet->letter_spacing.';';
      }
      if (!empty($stylesheet->size)) {
        $css .= ' font-size:'.$stylesheet->size.';';
      }
      if (!empty($stylesheet->decoration)) {
        $css .= ' text-decoration:'.$stylesheet->decoration.';';
      }
      if (!empty($stylesheet->transform)) {
        $css .= ' text-transform:'.$stylesheet->transform.';';
      }
      if (!empty($stylesheet->line_height)) {
        $css .= ' line-height:'.$stylesheet->line_height.';';
      }
      if (!empty($stylesheet->word_spacing)) {
        $css .= ' word-spacing:'.$stylesheet->word_spacing.';';
      }
      if (!empty($stylesheet->border)) {
        $css .= ' border:'.$stylesheet->border.';';
      }
      if (!empty($stylesheet->padding)) {
        $css .= ' padding:'.$stylesheet->padding.';';
      }
      if (!empty($stylesheet->margin)) {
        $css .=' margin:'.$stylesheet->margin.';';
      }
      if (!empty($stylesheet->extra)) {
        $css .= ' '.trim($stylesheet->extra, ';').';';

      }
      $css .= ' }'."\n";
    }
    file_put_contents(self::path(), $css);
  }

  public static function path() {
    return WEB_PATH.'/'.CORE.'/styles/stylesheet.css';
  }

  public static function url() {
    return '/'.WEB_PATH.'/'.CORE.'/styles/stylesheet.css';
  }

}