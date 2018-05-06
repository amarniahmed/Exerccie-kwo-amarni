<?php

class Pagination
{
  private $current = null;
  private $found_rows = null;
  private $limit = null;
  private $window_limit = null;
  private $window_size = null;

  public function __construct(array $opts=array()) {
    $this->found_rows = (int) $opts['found_rows'];
    if (isset($opts['offset']) && !isset($opts['page'])) {
      S('log')->trace('DEPRECATED: replace offset by page', __METHOD__);
      $opts['page'] = $opts['offset'];
      unset($opts['offset']);
    }
    $this->limit = isset($opts['page']) ? (int) $opts['limit'] : 1;
    // fix aleksandr
    $this->current = isset($opts['page']) && $opts['page'] > 0 ? $opts['page'] : 1;
    $this->window_size = isset($opts['window_size']) ? (int) $opts['window_size'] : 10;
    $this->window_limit = isset($opts['window_limit']) ? (int) $opts['window_limit'] : null;
  }

  public function asHtml(array $opts=array()) {
    if ($this->limit < 1 || $this->found_rows <= $this->limit) {
      return '';
    }
    $window_size = isset($opts['window_size'])
                 ? (int) $opts['window_size']
                 : $this->window_size;

    $window_limit = isset($opts['window_limit'])
                  ? (int) $opts['window_limit']
                  : $this->window_limit;

    $max = ceil($this->found_rows / $this->limit);

    if ($window_limit > 1 && $window_limit < $max) {
      $max = $window_limit;
    }

    $current = $this->current > $max ? $max : $this->current;

    $med = $window_size / 2;

    $first = $prev = $next = $last = 0;

    $window_first = 1;
    $window_last = $max;

    if ($max > $window_size) {

      $left = $current < $med ? $current - 1 : $med - 1;
      $right = ($current + $med) >= $max ? $max - $current : $med;

      if ($left < ($med - 1)) {
        $right += $med - $current;
      }

      if ($right < $med) {
        $left += $med - ($max - $current);
      }

      if ($current < $max) {
        $next = $current + 1;
      }

      if ($current > 1) {
        $prev = $current - 1;
      }

      if (($current - $left) > 1) {
        $first = 1;
      }

      if (($current + $right) <= $max && $right > 0) {
        $last = $max;
      }

      $window_first = $current - $left;
      $window_last = $current + $right;

    }

    /*
    $hash['current'] = $this->current;
    $hash['found_rows'] = $this->found_rows;
    $hash['max'] = $max;
    $hash['med'] = $med;
    $hash['left'] = $left;
    $hash['right'] = $right;
    $hash['first'] = $first;
    $hash['last'] = $last;
    $hash['prev'] = $prev;
    $hash['next'] = $next;
    $hash['window_first'] = $window_first;
    $hash['window_last'] = $window_last;
    S('log')->debug($hash);
    */

    $class       = $opts['class'] ?: 'pagination-page';
    $first_label = $opts['first'] ?: '&laquo;';
    $last_label  = $opts['last']  ?: '&raquo;';
    $prev_label  = $opts['prev']  ?: '&lsaquo;';
    $next_label  = $opts['next']  ?: '&rsaquo;';
    $separator   = $opts['sep']   ?: '&sdot;';
    $before_flag = (boolean) $opts['before_flag'];//false;    // Hugues : add opts['before_flag']
    $after_flag = (boolean) $opts['after_flag'];//false;    // Hugues : add opts['after_flag']
    //    $separator   .= "\n";

    $onclick = $href = null;
    if (isset($opts['onclick'])) {
      $onclick = $opts['onclick'];
      $href = $opts['href'];
    }
    elseif (isset($opts['href'])) {
      $href = $opts['href'];
    }
    else {
      $href = '?page=((page))';
    }

    if ($before_flag || ($first > 0 && strlen($first_label))) { // Hugues : add $before_flag ||
      $before_flag = true;
      $anchors[] = '<a'.$this->buildAnchor($onclick, $href, $first).' class="pagination-first '.$class.'">'.$first_label.'</a>';
    }

    if ($before_flag || ($prev > 0 && strlen($prev_label))) { // Hugues : add $before_flag ||
      $before_flag = true;
      $anchors[] = '<a'.$this->buildAnchor($onclick, $href, $prev).' rel="prev" class="pagination-prev '.$class.'">'.$prev_label.'</a>';
    }

    $anchors[] = '<span class="pagination-numbers">'; // Hugues : add
    foreach (range($window_first, $window_last) as $index) {
      $selected = $this->current == $index ? ' pagination-selected' : '';
      $rel = $index == $this->current - 1 ? ' rel="prev"' : ($index == $this->current + 1 ? ' rel="next"' : '');
      $anchors[] = '<a'.$this->buildAnchor($onclick, $href, $index).$rel.' class="pagination-number '.$class.$selected.'">'.$index.'</a>';
    }
    $anchors[] = '</span>'; // Hugues : add

    if ($after_flag || ($next > 0 && strlen($next_label))) { // Hugues : add $after_flag ||
      $after_flag = true;
      $anchors[] = '<a'.$this->buildAnchor($onclick, $href, $next).' rel="next" class="pagination-next '.$class.'">'.$next_label.'</a>';
    }

    if ($after_flag || ($last > 0 && strlen($last_label))) { // Hugues : add $after_flag ||
      $after_flag = true;
      $anchors[] = '<a'.$this->buildAnchor($onclick, $href, $last).' class="pagination-last '.$class.'">'.$last_label.'</a>';
    }

    $html = '';
    if (strlen($opts['before']) > 1) {
      $html  .= '<span class="pagination-before">'.$opts['before'].'</span>';
      if ($before_flag) {
        $html .= $separator;
      }
    }

    $html .= join($separator, $anchors);
    if (strlen($opts['after']) > 1) {
      if ($after_flag) {
        $html .= $separator;
      }
      $html .= '<span class="pagination-after">'.$opts['after'].'</span>';
    }

    return '<span class="pagination">'."\n".$html."\n".'</span>';
  }

  private function buildAnchor($onclick, $href, $page) {
    // fix aleksandr
    if ($onclick !== null) {
      if ($href === true) {
        $str  = ' href="?page='.($page).'"';
      }
      else {
        $str  = ' href="javascript:void(0)"';
      }
      $str .= ' onclick="'.str_replace('((page))', ($page), $onclick).'; return false;"';
    }
    else {
      $str = ' href="'.str_replace('((page))', ($page), $href).'"';
    }
    $str .= ' data-page="'.($page).'"';
    return $str;
  }

  public static function getInstance($opts) {
    $class = __CLASS__;
    return new $class($opts);
  }

}
