<?php

class Policy extends ActiveRecord {

  protected function onBeforeDestroy() {
    if ($this->getAdmins()->numRows() >= 1) {
      throw new ActiveRecordException('Opération impossible, certains administrateurs '.
                                      'sont associés à ce groupe.');
    }
  }

  protected function onBeforeCommit(&$attrs) {
    if (is_array($attrs['extensions'])) {
      $attrs['extensions'] = join(',', $attrs['extensions']);
    }
    if (isset($attrs['actions'])) {
      $attrs['actions'] = join(',', $attrs['actions']);
    }
  }

  public function getActions() {
    $actions = array();
    foreach (BackManager::getMenus() as $menu) {
      foreach ($menu['entries'] as $label => $url) {
        if ($url == '--') continue ;
        $actions[$url] = '<strong>'.c($menu['label']).'</strong> &gt; '.$label;
      }
    }
    return $actions;
  }

  public function hasNoAction() {
    return strlen($this->actions) < 3;
  }

}