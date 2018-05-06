<?php

class ACL {

  const TABLE = 'core_acl';

  const PERM_FULL = 255;
  const PERM_READ = 2;
  const PERM_WRITE = 4;
  const PERM_MODERATE = 8;


/***
réserver les perm > 100 à des perms spécifiques au model_id
***/


  public static function getPermissions($user, $item) {
    $set = S('dbo')->fetchValue('SELECT permissions'
                               .' FROM '.self::TABLE
                               .' WHERE user_id='.$user->getId()
                               .  ' AND model_id='.$item->getModelId()
                               .  ' AND record_id='.$item->getId());
    return $set;
  }

  public static function checkAccess($user, $item, $permissions=null) {
    $permissions = $permissions ? $permissions : self::PERM_FULL;
    if ($user->isAdministrator()) return true;
    if ($item->hasAttribute('user_id') && $item->user_id == $user->getId()) {
      return true;
    }
    if (method_exists($item, 'onAccess')) {
      return $item->onAccess($user, $permissions);
    }
    $row = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId(),
                 'user_id' => $user->getId());
    return S('dbo')->getTable(self::TABLE)->hasRow($row);
  }

  public static function grantAccess($user, $item, $permissions=null) {
    $permissions = $permissions === null ? self::PERM_FULL : $permissions;
    S('app')->hook('onBeforeAccessGrant',
                   array('user' => $user,
                         'item' => $item,
                         'permissions' => $permissions));
    $row = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId(),
                 'user_id' => $user->getId());
    if (S('dbo')->getTable(self::TABLE)->hasRow($row)) {
      self::removeAccess($user, $item, $permissions);
    }
    $row['permissions'] = $permissions;
    S('dbo')->insert(self::TABLE, $row);
    S('app')->hook('onAfterAccessGrant',
                   array('user' => $user,
                         'item' => $item,
                         'permissions' => $permissions));
  }

  public static function removeAccess($user, $item, $permissions=null) {
    $permissions = $permissions === null ? self::PERM_FULL : $permissions;
    S('app')->hook('onBeforeAccessRemove',
                   array('user' => $user,
                         'item' => $item,
                         'permissions' => $permissions));
    S('dbo')->exec('DELETE FROM '.self::TABLE
                   .' WHERE model_id='.$item->getModelId()
                   .  ' AND record_id='.$item->getId()
                   .  ' AND user_id='.$user->getId()
                   .' LIMIT 1');
    S('app')->hook('onAfterAccessRemove',
                   array('user' => $user,
                         'item' => $item,
                         'permissions' => $permissions));
  }

  public static function items($user, $model, $filters=null) {
    $dbo = S('dbo');
    $set = $dbo->asSet('SELECT record_id'
                       .' FROM '.self::TABLE
                       .' WHERE user_id='.$user->getId()
                       .  ' AND model_id='.Model::id($model));
    $filters = (array) $filters;
    $filters = $filters + array('id' => $set);
    $opts = array('filters' => $filters,
                  'order' => array('name' => 'asc'));
    return Item::collect($model, $opts);
  }

  public static function users($item, $permissions=null) {
    // TODO: gérer permissions dans la requête
    if (is_array($item)) {
      $model_id = (int) $item['model_id'];
      $record_id = (int) $item['record_id'];
    }
    else {
      $model_id = $item->getModelId();
      $record_id = $item->getId();
    }
    return S('dbo')->asArray('SELECT u.id, u.email, u.name, u.status, a.permissions'
                             .' FROM '.self::TABLE.' AS a,'
                             .     ' '.T('user').' AS u'
                             .' WHERE a.model_id='.$model_id
                             .  ' AND a.record_id='.$record_id
                             .  ' AND a.user_id=u.id'
                             .' ORDER BY email');
  }

  public static function asPermissionRows($item) {
    $html = '<ul>';
    foreach (self::users($item) as $user) {
      $html .= '<li data-user-id="'.$user['id'].'">'."\n";
      $html .= '<span class="destroy">x</span>';
      $classes = array('content');
      if ($user['status'] < User::STATUS_ONLINE) {
        $classes[] = 'offline';
      }
      $html .= '<span class="'.join(' ', $classes).'">'.$user['email'].'</span>';
      $html .= '</li>'."\n";
    }
    $html .= '</ul>';
    return $html;
  }

}
