<?php

final class XH
{
  public static function asHash($doc) {
    if (is_string($doc)) {
      $xml = $doc;
      $doc = new DOMDocument();
      $doc->preserveWhiteSpace = false;
      $doc->loadXML('<?xml version="1.0" encoding="UTF-8" ?>' . "\n" . $xml);
      $doc->encoding = 'UTF-8';
    }
    if ($doc instanceof DOMDocument) {
      $node = $doc->documentElement;
    }
    else {
      $node = $doc;
    }
    $tree = array();
    self::nodeTree($node, $tree);
    $out = self::nodeHash($tree['children']);
    return $out;
  }

  public static function asTree($doc) {
    if (is_string($doc)) {
      $xml = $doc;
      $doc = new DOMDocument();
      $doc->preserveWhiteSpace = false;
      @$doc->loadXML($xml);
      $doc->encoding = 'UTF-8';
    }

    if ($doc instanceof DOMDocument) {
      $node = $doc->documentElement;
    }
    else {
      $node = $doc;
    }
    $out = array();
    self::nodeTree($node, $out);
    return $out;
  }

  public static function nodeHash($children) {
    $parts = array();
    foreach ($children as $child) {
      $tag = $child['tag'];
      $val = null;
      if (is_null($child['children'])) {
        $val = $child['value'];
      }
      else {
        $val = self::nodeHash($child['children']);
      }
      if (!isset($parts[$tag])) {
        $parts[$tag] = $val;
      }
      elseif (is_array($parts[$tag]) && array_key_exists(0, $parts[$tag])) {
        $parts[$tag][] = $val;
      }
      else {
        $parts[$tag] = array($parts[$tag], $val);
      }
    }
    $out = array();
    foreach ($parts as $key => $value) {
      $out[$key] = $value;
    }
    return $out;
  }

  private static function nodeTree($node, &$elt) {
    $elt = array('tag' => $node->tagName);
    if (!$node->hasChildNodes()) {
      $elt['value'] = '';
    }
    elseif ($node->firstChild->nodeType === XML_TEXT_NODE) {
      $elt['value'] = $node->textContent;
    }
    else {
      foreach ($node->childNodes as $n) {
        $child = array();
        self::nodeTree($n, $child);
        $elt['children'][] = $child;
      }
    }
  }

  public static function encode($val) {
    return htmlspecialchars($val, ENT_XML1|ENT_DISALLOWED);
  }

  public static function node2data($node) {
    
  }

}