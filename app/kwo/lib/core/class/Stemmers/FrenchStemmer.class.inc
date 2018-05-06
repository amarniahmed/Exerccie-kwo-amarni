<?php

class FrenchStemmer {

  private $letters = null;
  private $vowels = null;

  private $rv = -1;
  private $r1 = -1;
  private $r2 = -1;

  private $length = 0;
  private $word = null;

  private $last_step = null;
  private $last_step_alteration = false;

  private $step1_rules = null;
  private $step1_length = null;
  private $step1_rule = null;
  private $step1_suffix = null;

  private $step2a_suffixes = null;

  private $step2b_rules = null;
  private $step4_rules = null;
  
  private $p1 = array('par', 'col', 'tap');
  
  private $suffixes = null;

  public $debug = false;

  public function __construct() {
    $this->letters = array_merge(range('a', 'z'),
                                 array('â', 'à', 'ç', 'ë', 'é', 'ê', 'è',
                                       'ï', 'î', 'ô', 'û', 'ù'));
    $this->vowels = array('a', 'e', 'i', 'o', 'u', 'y', 'â', 'à',
                          'ë', 'é', 'ê', 'è', 'ï', 'î', 'ô', 'û', 'ù');
    $this->step1_rules = array(1  => array('ance', 'iqUe', 'isme', 'able',
                                           'iste', 'eux', 'ances', 'iqUes',
                                           'ismes', 'ables', 'istes'),
                               2  => array('atrice', 'ateur', 'ation', 'atrices',
                                           'ateurs', 'ations'),
                               3  => array('logie', 'logies'),
                               4  => array('usion', 'ution', 'usions', 'utions'),
                               5  => array('ence', 'ences'),
                               6  => array('ement', 'ements'),
                               7  => array('ité', 'ités'),
                               8  => array('if', 'ive', 'ifs', 'ives'),
                               9  => array('eaux'),
                               10 => array('aux'),
                               11 => array('euse', 'euses'),
                               12 => array('issement', 'issements'),
                               13 => array('amment'),
                               14 => array('emment'),
                               15 => array('ment', 'ments'));
    $this->step2a_suffixes = array('îmes', 'ît', 'îtes', 'i', 'ie', 'ies', 'ir',
                                   'ira', 'irai', 'iraIent', 'irais', 'irait',
                                   'iras', 'irent', 'irez', 'iriez', 'irions', 
                                   'irons', 'iront', 'is', 'issaIent', 'issais',
                                   'issait', 'issant', 'issante', 'issantes',
                                   'issants', 'isse', 'issent', 'isses', 'issez',
                                   'issiez', 'issions', 'issons', 'it');
    $this->step2b_rules = array(1 => array('ions'), 
                                2 => array('é', 'ée', 'ées', 'és', 'èrent', 'er', 'era',
                                            'erai', 'eraIent', 'erais', 'erait', 'eras', 
                                            'erez', 'eriez', 'erions', 'erons', 'eront', 
                                            'ez', 'iez'), 
                                3 => array('âmes', 'ât', 'âtes', 'a', 'ai', 'aIent', 
                                           'ais', 'ait', 'ant', 'ante', 'antes', 'ants',
                                           'as', 'asse', 'assent', 'asses', 'assiez',
                                           'assions'));
    $this->step4_rules = array(1 => array('ion'),
                               2 => array('ier', 'ière', 'Ier', 'Ière'),
                               3 => array('e'),
                               4 => array('ë'));
  }
  
  public function extract($word) {
    //    S('log')->debug($this->isVowel('î'));
    //    return ;
    $word = trim($word);
    $word = preg_replace('/œ/u', 'oe', $word);
    $length = strlen($word);
    $this->debug('length : '.$length);
    if ($length <= 3) return $word;
    $this->word = $word;
    $this->stem = $word;
    $this->stem = strtolower($this->stem);
    for ($i = 0; $i < $length; $i++) {
      $is_first = $i === 0;
      $is_last = $i === ($length - 1);
      $letter = $this->charAt($i);
      $replace = null;
      if ($letter === 'u' || $letter === 'i') { 
        if ((!$is_first && $this->isVowel($i - 1)) && 
            (!$is_last && $this->isVowel($i + 1))) {  
          $replace = $letter === 'i' ? 'I' : 'U';
        }
        elseif ($letter === 'u' && !$is_first && $this->charAt($i - 1) === 'q') {
          $replace = 'U';
        }
      }
      elseif ($letter === 'y' && 
              ((!$is_first && $this->isVowel($i - 1)) || 
               (!$is_last && $this->isVowel($i + 1)))) {
        $replace = 'Y';
      }
      if ($replace) { 
        //$this->debug($replace);
        $this->replaceCharAt($i, $replace);
      }
    }
    
    $this->debug('stem : '.$this->stem);
    
    $this->mark();

    $this->doStep('1');
    
    if ($this->last_step_alteration === false || 
        in_array($this->step1_suffix, array('amment', 'emment', 'ment', 'ments'))) {
      $this->doStep('2a');
      if ($this->last_step_alteration === false) {
        $this->doStep('2b');
      }
    }
    if ($this->last_step_alteration === true) {
      $this->doStep('3');
    }
    else {
      $this->doStep('4');
    }
    $this->doStep('5');
    $this->doStep('6');
    $this->doStep('7');

    //    S('log')->trace($this->word.' : '.$this->stem, __METHOD__);

    return $this->stem;
  }

  private function charAt($index, $origin=null) { 
    if (!is_null($origin)) {
      if ($origin < 0) return '';
      $index += $origin;
    }
    //    $str = $str ?: $this->stem;
    return substr($this->stem, $index, 1);
  }

  private function mark() {
    $this->rv = -1;
    $this->r1 = -1;
    $this->r2 = -1;

    $length = strlen($this->word);

    if (($this->isVowel(0) && $this->isVowel(1)) ||
        in_array(substr($this->stem, 0, 3), $this->p1)) {
      $this->rv = 3; 
    }
    else {
      for ($i = 1; $i < ($length - 1); $i++) {
        if ($this->isVowel($i)) { 
          $this->rv = $i + 1;
          break ;
        }
      }
    }

    for ($i = 1; $i < $length; $i++) {
      if (!$this->isVowel($i) && $this->isVowel($i - 1)) {
        $this->r1 = $i + 1;
        break ;
      }
    }

    if ($this->r1 >= 0) {
      //      $l1 = strlen($this->r1);
      $start = $this->r1 + 1;
      for ($i = $start; $i < $length; $i++) {
        if (!$this->isVowel($this->charAt($i)) && 
            $this->isVowel($this->charAt($i - 1))) {
          $this->r2 = $i + 1;
          break ;
        }
      }
    }

    if ($this->rv >= 0) $this->debug('rv ('.$this->rv.') : '.substr($this->stem, $this->rv));
    if ($this->r1 >= 0) $this->debug('r1 ('.$this->r1.') : '.substr($this->stem, $this->r1));
    if ($this->r2 >= 0) $this->debug('r2 ('.$this->r2.') : '.substr($this->stem, $this->r2));
  }

  private function doStep($step) {
    $stem = $this->stem;
    $this->last_step = $step;
    $this->last_step_alteration = false;
    if ($step == '1')      $this->applyStep1Rule();
    elseif ($step == '2a') $this->applyStep2aRule();
    elseif ($step == '2b') $this->applyStep2bRule();
    elseif ($step == '3')  $this->applyStep3Rule();
    elseif ($step == '4')  $this->applyStep4Rule();
    elseif ($step == '5')  $this->applyStep5Rule();
    elseif ($step == '6')  $this->applyStep6Rule();
    elseif ($step == '7')  $this->applyStep7Rule();
    $this->debug('step ['.$step.'] '.$this->stem);
    if ($stem != $this->stem) {
      $this->last_step_alteration = true;
    }
  }

  private function applyStep1Rule() {

    $this->step1_rule = 0;
    $this->step1_length = 0;
    $this->step1_suffix = null;

    $length = $this->length();

    $endings = array();
    for ($i = 1; $i <= 9; $i++) {     
      if ($i > $length) break ;
      $endings[$i] = substr($this->stem, -$i);
    }

    foreach ($this->step1_rules as $rule => $suffixes) {
      foreach ($suffixes as $suffix) {
        $n = strlen($suffix);
        if ($n <= $this->step1_length) continue ;
        if ($suffix === $endings[$n]) {
          $this->step1_length = $n;
          $this->step1_rule = $rule;
          $this->step1_suffix = $suffix; 
        }
      }
    }

    if ($this->step1_rule < 1) return ; 

    $this->debug('step [1] longest ('.$this->step1_rule.') : '.$this->step1_suffix);
        
    $suffix = $this->step1_suffix;
    $rule = $this->step1_rule;
    if ($rule === 1) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->removeSuffix($suffix);
      }
      return ;
    }
    if ($rule === 2) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->removeSuffix($suffix);
        if ($this->endsWith('ic')) {
          if ($this->isSuffixIn('ic', 'r2')) {
            $this->removeSuffix('ic');
          }
          else {
            $this->replaceSuffix('ic', 'iqU');
          }
        }
      }
      return ;
    }
    if ($rule === 3) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->replaceSuffix($suffix, 'log');
      }
      return ;
    }
    if ($rule === 4) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->replaceSuffix($suffix, 'u');
      }
      return ;
    }
    if ($rule === 5) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->replaceSuffix($suffix, 'ent');
      }
      return ;
    }
    if ($rule === 6) { 
      if ($this->isSuffixIn($suffix, 'rv')) {
        $this->removeSuffix($suffix);
      } 
      if ($this->isSuffixIn('iv', 'r2')) {
        $this->removeSuffix('iv');
        if ($this->isSuffixIn('at', 'r2')) {
          $this->removeSuffix('at');
        }
      }
      elseif ($this->endsWith('eus')) {  
        if ($this->isSuffixIn('eus', 'r2')) {
          $this->removeSuffix('eus');
        }
        elseif ($this->isSuffixIn('eus', 'r1')) { 
          $this->replaceSuffix('eus', 'eux'); 
        }
      }
      elseif ($this->isSuffixIn('abl', 'r2')) {
        $this->removeSuffix('abl');
      }
      elseif ($this->isSuffixIn('iqU', 'r2')) {
        $this->removeSuffix('iqU');
      }
      elseif ($this->isSuffixIn('ièr', 'rv')) {
        $this->replaceSuffix('ièr', 'i');
      }
      elseif ($this->isSuffixIn('Ièr', 'rv')) {
        $this->replaceSuffix('Ièr', 'i');
      }
      return ;
    }
    if ($rule === 7) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->removeSuffix($suffix);
      }
      if ($this->endsWith('abil')) {
        if ($this->isSuffixIn('abil', 'r2')) {
          $this->removeSuffix('abil');
        }
        else {
          $this->replaceSuffix('abil', 'abl');
        }
      }
      elseif ($this->endsWith('ic')) {
        if ($this->isSuffixIn('ic', 'r2')) {
          $this->removeSuffix('ic');
        }
        else {
          $this->replaceSuffix('ic', 'iqU');
        }
      }
      elseif ($this->isSuffixIn('iv', 'r2')) {
        $this->removeSuffix('iv');
      }
      return ;
    }
    if ($rule === 8) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->removeSuffix($suffix);
      }
      if ($this->isSuffixIn('at', 'r2')) {
        $this->removeSuffix('at');
        if ($this->endsWith('ic')) {
          if ($this->isSuffixIn('ic', 'r2')) {
            $this->removeSuffix('ic');
          }
          else {
            $this->replaceSuffix('ic', 'iqU');
          }
        }
      }
      return ;
    }
    if ($rule === 9) { 
      $this->replaceSuffix($suffix, 'eau');
      return ;
    }
    if ($rule === 10) {
      if ($this->isSuffixIn($suffix, 'r1')) {
        $this->replaceSuffix($suffix, 'al');
      }
      return ;
    }
    if ($rule === 11) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->removeSuffix($suffix);
      }
      elseif ($this->isSuffixIn($suffix, 'r1')) {
        $this->replaceSuffix($suffix, 'eux');
      }
      return ;
    }
    if ($rule === 12) {
      $i = $this->length() - strlen($suffix);
      $i--;
      $letter = $this->charAt($i);
      if ($this->isSuffixIn($suffix, 'r1') && !$this->isVowel($letter)) {
        $this->removeSuffix($suffix);
      }
      return ;
    }
    if ($rule === 13) {
      if ($this->isSuffixIn($suffix, 'rv')) {
        $this->replaceSuffix($suffix, 'ant');
      }
      return ;
    }
    if ($rule === 14) {
      if ($this->isSuffixIn($suffix, 'rv')) {
        $this->replaceSuffix($suffix, 'ent');
      }
      return ;
    }
    if ($rule === 15) {
      $i = $this->length() - strlen($suffix);
      $i--;
      $letter = $this->charAt($i);
      if (!$this->isVowel($letter)) return ;
      if ($this->isSuffixIn($letter.$suffix, 'rv')) {
        $this->removeSuffix($suffix);
      }
      return ;
    }
  }

  private function applyStep2aRule() {

    $length = $this->length();

    $endings = array();
    for ($i = 1; $i <= 8; $i++) {     
      if ($length <= $i) break ;
      $endings[$i] = substr($this->stem, -$i);
    }

    $index = null;
    $l = 0;
    $rv_n = $this->rv < 0 ? 0 : $length - $this->rv;

    foreach ($this->step2a_suffixes as $i => $suffix) {
      if ($l > strlen($suffix)) continue ;
      $n = strlen($suffix);
      if ($n > $rv_n) continue ;
      if ($endings[$n] === $suffix) {
        $l = $n;
        $index = $i;
      }
    }
    if (is_null($index)) return false;

    $suffix = $this->step2a_suffixes[$index];

    $this->debug('step [2a] longest : '.$suffix);
 
    $i = $length - $l;
    $i--;

    $letter = $this->charAt($i); 

    if ($this->isVowel($letter)) return false;

    //    $this->debug($letter.$suffix);

    if ($this->isSuffixIn($letter.$suffix, 'rv')) { 
      $this->removeSuffix($suffix);
      return true;
    }

    return false;
  }

  private function applyStep2bRule() {

    $length = $this->length();

    $endings = array();
    for ($i = 1; $i <= 7; $i++) {     
      if ($length <= $i) break ;
      $endings[$i] = substr($this->stem, -$i);
    }

    $rule_index = null;
    $suffix_index = null;
    $l = 0;
    $rv_n = $this->rv < 0 ? 0 : $length - $this->rv;
    foreach ($this->step2b_rules as $rule => $suffixes) {
      foreach ($suffixes as $i => $suffix) {
        if ($l > strlen($suffix)) continue ;
        $n = strlen($suffix);
        if ($n > $rv_n) continue ;
        if ($endings[$n] === $suffix) {
          $l = $n;
          $suffix_index = $i;
          $rule_index = $rule;
         
        }
      }
    }

    $suffix = $this->step2b_rules[$rule_index][$suffix_index];

    if (is_null($rule_index)) return false;

    $this->debug('step [2b] longest : '.$suffix);

    if ($rule_index === 1) {
      if ($this->isSuffixIn($suffix, 'r2')) {
        $this->removeSuffix($suffix);
      }
    }
    elseif ($rule_index === 2) {
      $this->removeSuffix($suffix);
    }
    else {
      $this->removeSuffix($suffix);
      if ($this->isSuffixIn('e', 'rv')) {
        $this->removeSuffix('e');
      }
    }
  }

  private function applyStep3Rule() {
    $i = $this->length();
    $i--;
    $letter = $this->charAt($i);
    if ($letter === 'Y') $this->replaceCharAt($i, 'i');
    elseif ($letter === 'ç') $this->replaceCharAt($i, 'c');
  }

  private function applyStep4Rule() {
    /*    $suffix = substr($this->stem, -2);
    if ($suffix[1] === 's' && 
        !in_array($suffix[0], array('a', 'i', 'o', 'u', 'è', 's'))) {
      $this->stem = substr($this->stem, 0, -1);
      }*/


    if (preg_match('/[^aiouès]s$/u', $this->stem) > 0) { 
      $this->stem = substr($this->stem, 0, -1);
    }

    $endings = array();
    for ($i = 1; $i <= 4; $i++) {     
      if ($this->length() <= $i) break ;
      $endings[$i] = substr($this->stem, -$i);
    }

    $rule_index = null;
    $suffix_index = null;
    $l = 0;
    $rv_n = $this->rv < 0 ? 0 : $this->length() - $this->rv;
    foreach ($this->step4_rules as $rule => $suffixes) {
      foreach ($suffixes as $i => $suffix) {
        if ($l > strlen($suffix)) continue ;
        $n = strlen($suffix);
        if ($n > $rv_n) continue ; 
        //$this->debug($suffix.' : '.$endings[$n]);
        if ($endings[$n] === $suffix) { 
          $l = $n;
          $suffix_index = $i;
          $rule_index = $rule;
        }
      }
    }
    
    if (is_null($rule_index)) return false;
    $suffix = $this->step4_rules[$rule_index][$suffix_index];
    
    if ($rule_index === 1) {
      $letter = substr($this->word, -4, 1);
      if ($this->isSuffixIn($suffix, 'r2') && 
          $this->isSuffixIn($suffix, 'rv') && 
          ($this->isSuffixIn('s'.$suffix, 'rv') || 
           $this->isSuffixIn('t'.$suffix, 'rv'))) {
        $this->removeSuffix($suffix);
      }
    }
    elseif ($rule_index === 2) {
      $this->replaceSuffix($suffix, 'i');
    }
    elseif ($rule_index === 3) {
      $this->removeSuffix($suffix);
    }
    else {
      if ($this->endsWith('gu'.$suffix)) {
        $this->removeSuffix($suffix);
      }
    }
  }

  private function length() {
    return strlen($this->stem);
  }

  private function applyStep5Rule() {
    $suffix = substr($this->stem, -3);
    if (in_array($suffix, array('enn', 'onn', 'ett', 'ell')) ||
        substr($this->stem, -4) === 'eill') {
      $this->stem = substr($this->stem, 0, -1);
    }
  }

  private function applyStep6Rule() {
    $i = $this->length() - 1;
    if ($this->isVowel($i)) return ;
    $i--;
    while ($i >= 0) {
      $letter = $this->charAt($i);
      $i--;
      if (!$this->isVowel($letter)) continue ;
      if ($letter != 'é' && $letter != 'è') return ;
      $this->replaceCharAt($i + 1, 'e');
      return ;
    }
  }
  
  private function replaceCharAt($index, $char) { 
    $start = substr($this->stem, 0, $index);
    $end = substr($this->stem, $index + 1);
    $this->stem = $start . $char . $end; 
  }

  private function applyStep7Rule() {
    $patterns = array('/I/u', '/U/u', '/Y/u');
    $replacements = array('i', 'u', 'y');
    $this->stem = preg_replace($patterns, $replacements, $this->stem);
  }

  private function isSuffixIn($suffix, $origin=null) {
    $origin = $this->{$origin};
    if ($origin < 0) return false;
    if (($origin + strlen($suffix)) > $this->length()) return false; 
    $sub = substr($this->stem, -strlen($suffix)); 
    return $suffix === $sub;
  }

  private function endsWith($suffix) {
    $sub = substr($this->stem, -strlen($suffix));
    return $suffix === $sub;
  }

  private function replaceSuffix($from, $to) {
    $this->stem  = substr($this->stem, 0, -strlen($from));
    $this->stem .= $to;
  }
  
  private function removeSuffix($suffix) {
    $this->stem = substr($this->stem, 0, -strlen($suffix));
  }

  private function isVowel($letter) {
    if (is_int($letter)) {
      $letter = $this->charAt($letter);
    }
    return in_array($letter, $this->vowels);
  }

  private function debug($str) {
    if ($this->debug === true) {
      echo $str."\n";
    }
  }

}