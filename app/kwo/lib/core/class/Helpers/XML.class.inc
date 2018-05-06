<?php

class XML {

  public static function renameNode() {
    $newNode = $oldNode->ownerDocument->createElement('new_element_name');
    foreach ($oldNode->attributes as $attribute) {
      $newNode->setAttribute($attribute->nodeName, $attribute->nodeValue);
    }
    while ($oldNode->firstChild) {
      $newNode->appendChild($oldNode->firstChild);
    }
    $oldNode->ownerDocument->replaceChild($newNode, $oldNode);
  }

}

