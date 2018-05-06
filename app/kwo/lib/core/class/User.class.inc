<?php

class User extends ActiveRecord
{
  const FLAG_WOMAN = 1;
  const FLAG_IMAGE = 2;
  const FLAG_CLIENT = 4;
  const FLAG_CONVERSION = 8;
  const FLAG_GODSON = 16;
  const FLAG_INTERACTION = 128;
  const FLAG_WELCOMED = 256;
  const FLAG_WEB = 512;
  const FLAG_ACCUSTOMED = 1024;
  const FLAG_SPONSOR = 2048;
  const FLAG_VIP = 4096;

  public static $flags = array(self::FLAG_WOMAN => 'femme',
                               self::FLAG_IMAGE => 'avatar',
                               self::FLAG_CLIENT => 'client',
                               self::FLAG_CONVERSION => 'conversion',
                               self::FLAG_GODSON => 'filleul',
                               self::FLAG_SPONSOR => 'parrain',
                               self::FLAG_INTERACTION => 'actif',
                               self::FLAG_WEB => 'origine web',
                               self::FLAG_ACCUSTOMED => 'habitué',
                               self::FLAG_VIP => 'VIP');

  const STATUS_CONFIRMED = 2;
  const STATUS_PENDING = 1;
  const STATUS_CLOSED = 0;
  const STATUS_DELETED = -10; //KDEV : Nicolas C. => application newplv

  const STATUS_ONLINE = 1;

  public static $statuses = array(self::STATUS_CONFIRMED => 'confirmé',
                                  self::STATUS_PENDING => 'ouvert',
                                  self::STATUS_CLOSED => 'fermé');

  const PRIVACY_PUBLIC = 0;
  const PRIVACY_NETWORK = 1;
  const PRIVACY_PRIVATE = 2;

  public static $privacies = array(self::PRIVACY_PUBLIC => 'tout le monde',
                                   self::PRIVACY_NETWORK => 'uniquement le réseau',
                                   self::PRIVACY_PRIVATE => 'personne');

  const COMPLAINT_INVALID = 1;
  const COMPLAINT_SPAM = 2;
  const COMPLAINT_ABUSE = 4;
  const COMPLAINT_BLACKLIST = 8;
  const COMPLAINT_SUSPICIOUS = 16;
  const COMPLAINT_ERROR = 32;

  public static $complaints = array(self::COMPLAINT_INVALID => 'invalide',
                                    self::COMPLAINT_SPAM => 'spam',
                                    self::COMPLAINT_ABUSE => 'abus',
                                    self::COMPLAINT_BLACKLIST => 'liste noire',
                                    self::COMPLAINT_SUSPICIOUS => 'douteux',
                                    self::COMPLAINT_ERROR => 'erreur');

  const TABLE_INFORMATION = 'core_user_information';
  const TABLE_PRIVILEGE = 'core_privilege_user';

  const QUOTA_HARD = 50;
  const QUOTA_SOFT = 200;

  private $profiles = null;
  private $sponsor_id = null;

  public function asPickerRow() {
    return $this->name.' ('.$this->id.')<br/><small>'.$this->email.'</small>';
  }

  public function asPickerHash() {
    $h = parent::asPickerHash();
    $h['name'] = $this->getName() ?: $this->email;
    return $h;
  }

  public function asView($view) {
    if ($view == 'preview') {
      return $this->email.' ('.$this->getId().')<br/>'.$this->name;
    }
    return parent::asView($view);
  }

  public function commit($attrs) {
    $informations = array();
    if (!empty($attrs['image'])) {
      $image = $attrs['image'];
      unset($attrs['image']);
    }
    $info_fields = array('quota_soft', 'quota_hard', 'birth_date', 'timezone_id');
    foreach ($info_fields as $key) {
      if (!isset($attrs[$key])) continue ;
      if ($key === 'birth_date') {
        $attrs[$key] = DH::toDb($attrs[$key]);
      }
      $informations[$key] = $attrs[$key];
      unset($attrs[$key]);
    }
    parent::commit($attrs);
    if (File::exists($image)) {
      try {
        if (!$this->hasHomeDirectory()) {
          $this->createHomeDirectory();
        }
        $width = P('avatar.width') > 1 ? P('avatar.width') : 36;
        $height = P('avatar.height') > 1 ? P('avatar.height') : 36;
        $image = Img::getInstance($image);
        $image = $image->copyTo('_a.jpg', true);
        $image->moveIn($this->getPath());
        $image->setSize($width, $height);
        $this->setFlag(self::FLAG_IMAGE);
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }
    if (!empty($informations)) {
      $this->setInformations($informations);
    }
  }

  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['email'])) {
      $id = $this->isItem() ? $this->getId() : 0;
      $ret = $this->dbo->fetchValue('SELECT COUNT(*)'
                                    .' FROM '.T('user')
                                    .' WHERE email=:email:'
                                    .  ' AND id<>'.$id,
                                    array(':email:' => $attrs['email']));
      if ($ret >= 1) {
        S('log')->trace('email already used ['.$attrs['email'].']',
                        __METHOD__);
        throw new Exception(l('error.login.used'));
      }
    }
    if (!$this->isItem()) {
      if (empty($attrs['password'])) {
        $attrs['password'] = SH::password();
      }
      if (!isset($attrs['status'])) {
        $attrs['status'] = self::STATUS_CONFIRMED;
      }
      if ($attrs['email'] && empty($attrs['name'])) {
        list ($name, ) = explode('@', $attrs['email']);
        $attrs['name'] = self::cleanName($name);
      }
    }
    if (isset($attrs['md5'])) {
      $attrs['password'] = $attrs['md5'];
      unset($attrs['md5']);
    }
    elseif (isset($attrs['password'])) {
      if (empty($attrs['password'])) {
        unset($attrs['password']);
      }
      elseif (strlen($attrs['password']) < 32) {
        $attrs['password'] = self::hashPassword($attrs['password']);
      }
    }
  }

  protected function onAfterStatusChange($old_status) {
    S('app')->hook('onUserStatusChange',
                   array('user' => $this,
                         'old_status' => $old_status));
  }

  public function checkPassword($password) {
    $hash = self::hashPassword($password);
    $password = S('dbo')->fetchValue('SELECT password FROM '.T('user')
                                     .             ' WHERE id='.$this->getId());
    return $password === $hash;
  }

  public function asFinderHash($h) {
    if ($this->status <= self::STATUS_CLOSED) $color = 'red';
    elseif ($this->status == self::STATUS_PENDING) $color = 'yellow';
    else $color = 'green';
    $h['_color'] = $color;
    return $h;
  }

  protected function onAfterCreate() {
    $this->getPath();
  }

  public function onAfterStore() {
    $attrs = S('req')->getAttributes();
    if (is_array($attrs['profiles'])) {
      foreach ($attrs['profiles'] as $profile => $attrs) {
        if (!Model::exists($profile)) continue ;
        $this->setProfile($attrs, $profile);
      }
    }
  }

  public function onRelationCreate($item) {
    if (Model::isProfile($item)) {
      $this->setProfiles();
    }
  }

  public function onRelationUpdate($item) {
    if (Model::isProfile($item)) {
      $this->setProfiles();
    }
  }

  public function onRelationDestroy($item) {
    if (Model::isProfile($item)) {
      $this->setProfiles();
    }
  }

  private function setProfiles() {
    $bits = 0;
    foreach (P('profiles') as $bit => $profile) {
      if (!Model::exists($profile)) continue ;
      $status = Model::hasField($profile, 'status') ? ' AND status>0' : '';
      $id = $this->dbo->fetchValue('SELECT id'
                                   .' FROM '.T($profile)
                                   .' WHERE user_id='.$this->getId()
                                   .$status);
      if ($id < 1) continue ;
      $bits += $bit;
    }
    $this->commit(array('profiles' => $bits));
  }

  protected function onAfterCommit($attrs) {
    if (isset($attrs['privileges'])) {
      $attrs['privileges'] = empty($attrs['privileges'])
                           ? array()
                           : $attrs['privileges'];
      $old = (int) $this->prior('privileges');
      $new = $attrs['privileges'];
      if ($old != array_sum($new)) {
        $bits = array();
        for ($i = 0; $i <= 31; $i++) {
          $n = pow(2, $i);
          if ($n > $old) break ;
          if ($old & $n) $bits[] = $n;
        }
        $old = $bits;
        $diff = array_diff($old, $new);
        foreach ($diff as $priv) {
          S('app')->hook('onPrivilegeRemove',
                         array('user' => $this,
                               'privilege' => strlen(decbin($priv))));
          //          S('log')->debug('remove ('.$priv.')');
        }
        $diff = array_diff($new, $old);
        foreach ($diff as $priv) {
          S('app')->hook('onPrivilegeAdd',
                         array('user' => $this,
                               'privilege' => strlen(decbin($priv))));
          //          S('log')->debug('add ('.$priv.')');
        }
      }
    }
    if (!empty($attrs['email']) && !$this->isNew()) {
      $email = $this->prior('email');
      if (!empty($email) && $email != $this->email) {
        $from_id = Email::asId($email);
        $to_id = Email::asId($this->email);
        S('app')->hook('onEmailChange',
                       array('user' => $this,
                             'from' => $from_id,
                             'to' => $to_id));
        Base::onEmailChange($from_id, $to_id);
        Subscription::onEmailChange($from_id, $to_id);
      }
    }
  }

  public function getConversionWorth($opts) {
    return 0.2;
  }

  public function getInformation($key) {
    return $this->dbo->fetchValue('SELECT '.$key
                                  .' FROM '.self::TABLE_INFORMATION
                                  .' WHERE user_id='.$this->getId());
  }

  public function setInformation($key, $value) {
    $this->setInformations(array($key => $value));
  }

  public function setInformations($h) {
    if (!is_array($h) || empty($h)) return ;
    $opts = array('filters' => array('user_id' => $this->getId()),
                  'limit' => 1);
    $h['user_id'] = $this->getId();
    $this->dbo->update(self::TABLE_INFORMATION, $h, $opts);
    if ($this->dbo->matchedRows() < 1) {
      $this->dbo->insert(self::TABLE_INFORMATION, $h);
    }
  }

  public function hasSponsor() {
    if ($this->sponsor_id === null) {
      $this->sponsor_id = (int) $this->dbo->fetchValue('SELECT user_id'
                                                       .' FROM '.T('sponsorship')
                                                       .' WHERE godson_id='.$this->getId());
    }
    if ($this->sponsor_id >= 1) return true;
  }

  public function getSponsorId() {
    if (!$this->hasSponsor()) return 0;
    return $this->sponsor_id;
   }

  public function getSponsor() {
    if (!$this->hasSponsor()) return null;
    try {
      return User::getInstance($this->sponsor_id);
    }
    catch (Exception $e) {}
    return null;
  }

  public function asIdentityHash($locale, $type=null, $options=null) {
    $h = array('name' => $this->name,
               'description' => $this->description);
    if ($this->hasFlag(self::FLAG_IMAGE)) {
      $h['image'] = '/'.$this->getPath().'_a.jpg';
    }
    return $h;
  }

  public function asEditHash() {
    $h = array();
    if ($this->isItem()) {
      if ($this->hasFlag(self::FLAG_IMAGE)) {
        $h['image'] = '/'.$this->getPath().'_a.jpg';
      }
      $h['quota_hard'] = $this->getInformation('quota_hard');
      $h['quota_soft'] = $this->getInformation('quota_soft');
      $h['birth_date'] = $this->getInformation('birth_date');
      $h['profiles']['person'] = array();
      if ($this->hasProfile('person')) {
        $person = $this->getProfile('person');
        $h['profiles']['person'] = $person->getAttributes();
      }
    }
    else {
      $h['quota_hard'] = P('user.quota.hard') ?: self::QUOTA_HARD;
      $h['quota_soft'] = P('user.quota.soft') ?: self::QUOTA_SOFT;
    }
    return $h;
  }

  public function getDetails() {
    $h = array('chemin' => '/'.$this->getPath(),
               'espace utilisé' => $this->getUsedSpace().' M / '.$this->getQuota().' M',
               'source' => '-');
    if ($this->hasSponsor()) {
      $sponsor = $this->getSponsor();
      $h['parrain'] = $this->getSponsor()->email.' '.Item::link($sponsor);
    }
    $origin_id = $this->getInformation('origin_id');
    if ($origin_id >= 1) {
      try {
        $h['source'] = ucwords(Origin::getInstance($origin_id)->name);
      }
      catch (Exception $e) {}
    }
    $rs = $this->dbo->query('SELECT created_at, visit_id'
                            .' FROM '.Interaction::TABLE
                            .' WHERE type='.Interaction::TYPE_AUTH
                            .  ' AND model_id='.$this->getModelId()
                            .  ' AND record_id='.$this->getId()
                            .' ORDER BY created_at DESC'
                            .' LIMIT 15');
    $dates = array();
    foreach ($rs as $row) {
      $line = substr($row['created_at'], 0, 16).' - ';
      $line .= Visit::link($row['visit_id']);
      $dates[] = $line;
    }
    $dates = join('<br/>', $dates);
    $h += array('nombre d’authentification(s)' => $this->getInformation('auth_count'),
                'dernière(s) authentification(s)' => $dates);

    // conversion
    $conversions = array();
    $rs = $this->dbo->query('SELECT *'
                            .' FROM '.T('conversion')
                            .' WHERE user_id='.$this->getId()
                            .' ORDER BY converted_at DESC'
                            .' LIMIT 20');
    foreach ($rs as $row) {
      $line = substr($row['converted_at'], 0 , 10).' - ';
      try {
        /*          $item = Item::getInstance($row['model_id'], $row['record_id'],
                                    array('ignore' => true));*/
        $conversion = Conversion::getInstance($row['id']);
        //          $conversion .= $item->asConversionName();
        $line .= Item::link($conversion, Model::name($conversion->model_id).' - '.$conversion->worth).' - ';
        $line .= Tracker::$mediums[$conversion->medium];
        if ($conversion->status == Conversion::STATUS_ADJUDGED) {
          $line .= ' *';
        }
      }
      catch (Exception $e) {
        $line .= Model::label($row['model_id']).' ('.$row['record_id'].')';
      }
      $conversions[] = $line;
    }
    if (!empty($conversions)) {
      $conversions = join('<br/>', $conversions);
      $h += array('dernière(s) conversion(s)' => $conversions);
    }
    // end conversion

    // alert
    $filters = array('user_id' => $this->getId(),
                     'status' => Alert::STATUS_ONLINE);
    $alerts = Alert::collect(array('filters' => $filters));
    $html = '<ul>';
    $opts = array('on' => array('click' => 'kwo.ux.onExec(this)'),
                  'data' => array('action' => '/back/core/item.disable',
                                  'args' => array('item' => ''),
                                  'confirm' => 'êtes-vous sûr ?',
                                  'model' => 'user',
                                  'callback' => 'hide.up'));
    foreach ($alerts as $alert) {
      $opts['data']['args']['item'] = $alert->getKey();
      $content = H::elt('a', 'supprimer', $opts);
      $content = $alert->asString().' '.$content;
      $html .= H::elt('li', $content);
    };
    $html .= '</ul>';
    if ($alerts->count() >= 1) {
      $h += array('alerte(s)' => $html);
    }
    // end alert

    return $h;
  }

  public function hasNotice($notice_id) {
    return $this->hasFlag(NH::asBit($notice_id), 'notices');
  }

  public function getNotices() {
    return $this->getMask('notices', true);
  }

  public function getPrivileges() {
    return $this->getMask('privileges', true);
  }

  public function addPrivilege($privilege_id, $opts=null) {
    if (!is_null($opts)) {
      $privilege = Privilege::getInstance($privilege_id);
      $privilege->grantTo($this, $opts);
    }
    if ($this->hasPrivilege($privilege_id)) return ;
    $this->setFlag(NH::asBit($privilege_id), 'privileges');
    S('app')->hook('onPrivilegeAdd',
                   array('user' => $this,
                         'privilege' => $privilege_id));
  }

  public function removePrivilege($privilege_id) {
    if (!$this->hasPrivilege($privilege_id)) return ;
    $this->unsetFlag(NH::asBit($privilege_id), 'privileges');
    S('app')->hook('onPrivilegeRemove',
                   array('user' => $this,
                         'privilege' => $privilege_id));
  }

  public function hasPrivilege($privilege_id) {
    return $this->hasFlag(NH::asBit($privilege_id), 'privileges');
  }

  public function hasAnyPrivilege($privileges) {
    foreach ($privileges as $privilege_id) {
      if ($this->hasPrivilege($privilege_id)) return true;
    }
    return false;
  }

  public function isAdministrator() {
    return $this->hasPrivilege(Privilege::ADMINISTRATOR);
  }

  public function isModerator() {
    return $this->hasPrivilege(Privilege::MODERATOR);
  }

  public function isSubscriber() {
    return $this->hasPrivilege(Privilege::SUBSCRIBER);
  }

  public function isCustomer() {
    $id = S('dbo')->fetchValue('SELECT id'
                               .' FROM '.T('order')
                               .' WHERE user_id='.$this->id
                               .  ' AND status>='.Order::STATUS_PAID
                               .' LIMIT 1');
    return $id > 0;
  }

  /*  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM social_privilege_user'
                     .    ' WHERE user_id='.$this->getId());
    $this->dbo->exec('DELETE FROM '.self::TABLE_INFORMATION
                     .    ' WHERE user_id='.$this->getId());
    $this->dbo->exec('DELETE FROM '.ACL::TABLE
                     .    ' WHERE user_id='.$this->getId());
    $this->dbo->exec('DELETE FROM '.T('subscription')
                     .    ' WHERE user_id='.$this->getId());
                     }*/

  public function destroy() {
    if (S('req')->isCli()) parent::destroy();
  }

  public function hasReceivedLetter($code) {
    $id = S('dbo')->fetchValue('SELECT d.id'
                               .' FROM '.T('delivery').' AS d,'
                               .     ' '.T('letter').' AS l'
                               .' WHERE l.code="'.$code.'"'
                               .  ' AND l.id=d.letter_id'
                               .  ' AND d.user_id='.$this->id
                               .' LIMIT 1');
    return $id >= 1;
  }

  public function hasAccessTo($item, $permissions=null) {
    return ACL::checkAccess($this, $item, $permissions);
  }

  public function avatar($opts=null) {
    $opts = (array) $opts;
    $attrs = array('src' => $this->getThumbnail(),
                   'class' => 'avatar');
    if (!empty($opts['title'])) {
      $attrs['title'] = $opts['title'];
    }
    if ($opts['refresh'] === true) {
      $attrs['src'] .= '?t='.time();
    }
    return H::elt('img', null, $attrs);
  }

  public function getThumbnail() {
    $src = $this->hasFlag(self::FLAG_IMAGE)
         ? '/'.$this->getPath().'_a.jpg'
         : PIX_URL.'/avatar.gif';
    return $src;
  }

  /*** PROFILE ***/

  public function getCustomerProfile() {
    $profile = P('profile.customer') ?: P('profile.person');
    return $this->getProfile($profile);
  }

  public function hasProfile($model=null) {
    if ($model === null) {
      $model = P('profile.user') ?: 'person';
    }
    return $this->getProfileId($model) >= 1;
  }

  private function getProfileId($model=null) {
    if ($model === null) {
      $model = P('profile.user') ?: 'person';
    }
    if (!isset($this->profiles[$model])) {
      $this->profiles[$model] = 0;
      try {
        $profile = Item::getInstance($model, array('user_id' => $this->getId()),
                                     array('ignore' => true));
        $this->profiles[$model] = $profile;
      }
      catch (Exception $e) {}
    }
    return $this->profiles[$model] instanceof ActiveRecord
         ? $this->profiles[$model]->getId()
         : $this->profiles[$model];
  }

  public function getProfile($model=null) {
    if ($model === null) {
      $model = P('profile.user') ?: 'person';
    }
    $record_id = $this->getProfileId($model);
    if ($record_id < 1) {
      throw new Exception('profile unknown');
    }
    if ($this->profiles[$model] instanceof ActiveRecord) {
      return $this->profiles[$model];
    }
    $this->profiles[$model] = Item::getInstance($model, $record_id);
    return $this->profiles[$model];
  }

  public function setProfile($attrs, $model=null) {
    if ($model === null) {
      $model = P('profile.user') ?: 'person';
    }
    if (!is_array($attrs) || empty($attrs)) {
      S('log')->error('invalid input', __METHOD__);
      return ;
    }
    unset($attrs['id']);
    $profile_id = (int) $this->getProfileId($model);
    $profile = Item::getInstance($model, $profile_id ?: true);
    if (!S('req')->isBack()) {
      $profile->validate($attrs);
    }
    $attrs['user_id'] = $this->getId();
    $profile->commit($attrs);
    $profile->setUser($this->getId());
    if (isset($attrs['civility'])) {
      if (($attrs['civility'] >= 1) !== $this->hasFlag(self::FLAG_WOMAN)) {
        if ($attrs['civility'] >= 1) {
          $this->setFlag(self::FLAG_WOMAN);
        }
        else {
          $this->unsetFlag(self::FLAG_WOMAN);
        }
      }
    }
    $this->profiles[$model] = $profile;
    return $profile;
  }

  /*** /PROFILE ***/

  public function credit($code, $bindings=null) {
    try {
      if (!Model::exists('movement')) return ;
      Movement::trigger($this, $code, $bindings);
    }
    catch (Exception $e) { S('log')->error($e, __METHOD__); }
  }

  public function getCredit() {
    $credit = (int) $this->dbo->fetchValue('SELECT SUM(amount)'
                                           .' FROM '.T('credit')
                                           .' WHERE user_id='.$this->getId()
                                           .' AND (expire_on="0000-00-00"'
                                           .     ' OR expire_on >= "'.date('Y-m-d').'")'
                                           .' GROUP BY user_id');
    $debit = (int) $this->dbo->fetchValue('SELECT SUM(amount)'
                                          .' FROM '.T('debit')
                                          .' WHERE user_id='.$this->getId()
                                          .' GROUP BY user_id');
    $diff = $credit - $debit;
    $diff = $diff < 0 ? 0 : (int) $diff;
    return $diff;
  }

  public function comment(ActiveRecord $item, $content) {
    if (empty($content)) {
      throw new Exception(l('error.content.empty'));
    }
    $comment = Comment::getInstance();
    $comment->commit(array('model_id' => $item->getModelId(),
                           'record_id' => $item->getId(),
                           'content' => $content,
                           'user_id' => $this->getId()));
    try {
      $item->onComment($comment);
    }
    catch (Exception $e) { S('log')->error($e, __METHOD__); }
    $comment->setUser($this);
    $item->incrementMeta(Meta::FIELD_COMMENTS);
    $item->setMetas(array(Meta::FIELD_CHILD => $comment->getId(),
                          Meta::FIELD_UPDATED_AT => DH::now()));
    $item->track(Interaction::TYPE_COMMENT,
                 array('value' => $comment->getId()));
    return $comment;
  }

  public function getManagedResources($model) {
    $opts = array('filters' => array('|MANAGER|' => $this->getId()));
    return Item::collect($model, $opts);
  }

  public function countManagedResources($model) {
    return Item::collect($model, array());
  }

  public function setEmail($email) {
    if (!Email::isValid($email, true)) {
      S('log')->error('set_email failure ['.$email.']', __METHOD__);
      throw new Exception(l('error.email.invalid'));
    }
    $n = $this->dbo->fetchValue('SELECT COUNT(*)'
                                .' FROM '.T('user')
                                .' WHERE email=:email:'
                                .  ' AND id<>:id:',
                                array(':email:' => $email,
                                      ':id:' => $this->getId()));
    if ($n >= 1) {
      S('log')->error('email exists ['.$email.']', __METHOD__);
      throw new Exception(l('error.email.used'));
    }
    $this->commit(array('email' => $email,
                        'status' => self::STATUS_PENDING));
    return true;
  }

  public function requestEmailConfirmation() {
    Letter::send('email.confirm', $this,
                 array('email' => $this->email,
                       'name' => $this->name,
                       'link' => $this->confirmUrl()));
  }

  public function confirmUrl() {
    $url = H::url('/email.confirm',
                  array('id' => $this->getId(),
                        'key' => SH::crypt($this->email)));
    return H::elt('a', $url, array('href' => $url));
  }

  public function setPassword($password) {
    $this->commit(array('password' => $password));
  }

  public function onInvitationAccept($invitation) {
    Network::addConnexion($invitation->user_id, $this->getId());
    S('app')->hook('onUserInvitationAccept', array('invitation' => $invitation));
  }

  public function onInvitationRefuse($invitation) {
    S('app')->hook('onUserInvitationRefuse', array('invitation' => $invitation));
  }

  public function onInvitationSend(Invitation $invitation) {
    $user = $invitation->getUser();
    $recipient = $invitation->getRecipient();
    $link = '/account/invitation.list';
    Letter::send('invitation.user', $recipient,
                 array('user' => $user->name,
                       'link' => $link),
                 array('locale' => $recipient->locale));
    S('app')->hook('onUserInvitationSend', array('invitation' => $invitation));
  }

  public function asInvitationSnippet($invitation) {
    $msg = '<strong>'.$invitation->getUser().'</strong>';
    $msg .= ' vous propose de rejoindre son réseau.';
    return $msg;
  }

  public function update($attrs) {
    unset($attrs['email']);
    $attrs = User::cleanAttributes($attrs);
    if (!User::isValidUser($attrs)) {
      throw new Exception(l('error.fields.required'));
    }
    $this->commit($attrs);
    return $attrs;
  }

  public function getPath() {
    $path = $this->path;
    if (empty($path)) {
      $path = self::getHash($this->getId());
      $this->setAttribute('path', $path);
    }
    return USR_PATH.'/'.$this->path.'/';
  }

  public function hasHomeDirectory() {
    return is_dir($this->getPath());
  }

  public function createHomeDirectory() {
    $path = $this->getPath();
    if (is_dir($path)) return true;
    $ret = Folder::create($path, true);
    if ($ret === false) {
      S('log')->error('home directory creation failure ['.$path.'] ('.$this->getId().')',
                      __METHOD__);
      throw new Exception('home directory creation failure');
    }
    return $ret;
  }

  public function getFiles() {
    if (!$this->hasHomeDirectory()) {
      return array();
    }
    $h = array();
    $files = glob($this->getPath().'*.*');
    if (!is_array($files) || count($files) < 1) {
      return $h;
    }
    foreach ($files as $file) {
      $filename = basename($file);
      if ($filename[0] != '_') {
        $h[] = array('name' => $filename,
                     'path' => $file);
      }
    }
    return $h;
  }

  public function hasFile($name) {
    $name = trim($name);
    $name = basename($name);
    if (empty($name)) return false;
    return file_exists($this->getPath().$name);
  }

  public function removeFile($name) {
    if (!$this->hasFile($name)) return ;
    $file = $this->getFile($name);
    return $file->unlink();
  }

  public function getFile($name) {
    $name = trim($name);
    $name = basename($name);
    return File::getInstance($this->getPath().$name);
  }

  public function getUsedSpace() {
    return $this->hasHomeDirectory()
         ? Folder::getInstance($this->getPath())->getUsedSpace()
         : 0;
  }

  public function getQuota() {
    $quota = $this->getInformation('quota_hard');
    $quota = $quota > 0 ? $quota : P('user.quota.hard');
    $quota = $quota > 0 ? $quota : self::QUOTA_HARD;
    return $quota;
  }

  public function hasAvailableSpace($type='both') {
    if ($type === 'hard' || $type === 'both') {
      if ($this->getUsedSpace() > $this->getQuota()) return false;
    }
    else {
      $quota = $this->getInformation('quota_soft');
      $quota = $quota > 0 ? $quota : P('user.quota.soft');
      $quota = $quota > 0 ? $quota : self::QUOTA_SOFT;
      if (count($this->getFiles()) > $quota) return false;
    }
    return true;
  }

  public function receive($opts=null) {
    if (!$this->hasHomeDirectory()) {
      $this->createHomeDirectory();
    }
    if (!$this->hasAvailableSpace('hard')) {
      S('log')->error('hard quota limit ('.$this->getId().')', __METHOD__);
      throw new Exception(l('error.upload.quota.hard'));
    }
    if (!$this->hasAvailableSpace('soft')) {
      S('log')->error('soft quota limit ('.$this->getId().')', __METHOD__);
      throw new Exception(l('error.upload.quota.soft'));
    }
    $opts = $opts === null ? array('img' => true) : $opts;
    $files = array();
    foreach ($_FILES as $name => $null) {
      $tmp = preg_replace('/[^a-zA-Z0-9_-]/', '', $name);
      $tmp = trim($tmp, '-');
      if ($tmp != $name) {
        S('log')->error('invalid input name ['.$name.']', __METHOD__);
        continue ;
      }
      $name = $tmp;
      if (strlen($_FILES[$name]['name']) < 4) continue ;
      $file = null;
      try {
        $file = UploadManager::user($this, $name);
        $files[$name] = $file;
      }
      catch (Exception $e) {
        if ($file instanceof File) {
          $file->unlink();
          throw new Exception('Veuillez envoyer une image valide /'
                            .' Please send a valid image');
        }
      }
    }
    //S('app')->hook('onFileUpload', $files);
    return $files;
  }

  public function getCredential() {
    $time = time();
    $ret  = hash('md5', $this->getId().$this->password.$time);
    $ret .= '-'.$this->getId();
    $ret .= '-'.$time;
    return $ret;
  }

  public function onSignup() {
    // only for online registration
    $req = S('req');
    $cookie = $req->getCookie();
    $attrs = array('user_id' => $this->getId());
    if (P('auth.sso') === true) {
      $attrs['user_secret'] = substr($this->password, 2, 8);
    }
    $cookie->setAttributes($attrs);
    S('ctx')->setAttribute('user_id', $this->getId());
    $this->convert();
    try {
      S('app')->hook('onUserCreate', array('user' => $this));
    }
    catch (Exception $e) {
      S('log')->error($e);
    }
    foreach (Notice::collect('notice') as $notice) {
      if (!$notice->hasFlag(Notice::FLAG_DEFAULT)) continue ;
      $this->setFlag($notice, 'notices');
    }
    Movement::trigger($this, 'user.signup');
    Base::addUser($this);
  }

  //-- STATIC -----------------------------------------------------------------

  public static function hashPassword($password) {
    $password = substr($password, 0, 64);
    return hash('md5', $password);
  }

  public static function signup($opts=array(), $return_string=false) {
    S('log')->trace('deprecated use $res->sendAuthenticationForm()', __METHOD__);
    S('res')->sendAuthenticationForm();
  }

  public static function isValidUser($h) {
    $protected = array('flags', 'locale', 'notices', 'password', 'path',
                       'privileges', 'profiles', 'status', 'md5',
                       'quota_soft', 'quota_hard');
    if (count(array_intersect(array_keys($h), $protected)) >= 1) {
      S('log')->debug($h, 'protected fields');
      return false;
    }
    $columns = array_merge(S('dbo')->getTable(T('user'))->getFields(),
                           S('dbo')->getTable(self::TABLE_INFORMATION)->getFields());

    $diff = array_diff(array_keys($h), $columns);
    if (!empty($diff)) {
      S('log')->debug($diff, __METHOD__);
      return false;
    }
    return true;
  }

  public static function getHash($id) {
    if ($id < 1) {
      S('log')->error('invalid uid ('.$id.')', __METHOD__);
      throw new Exception('invalid uid');
    }
    $index = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $res = '';
    do {
      $res = $index[$id % 62] . '/' . $res;
      $id = floor($id / 62);
    } while ($id);
    return substr($res, 0, -1);
    /*$index = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $out = '';
    $length = strlen($index);
    $t = floor(log10($id) / log10($length));
    do {
      $b = pow($length, $t);
      $a = floor($id / $b);
      $id -= ($a * $b);
      $out .= $index[$a].'/';
      --$t;
    } while ($t >= 0);
    return trim($out, '/');*/
  }

  public static function cleanAttributes($attrs) {
    unset($attrs['id']);
    if (isset($attrs['name'])) {
      $attrs['name'] = self::cleanName($attrs['name']);
    }
    return $attrs;
  }

  private static function cleanName($name) {
    $name = strip_tags($name);
    $name = SH::stripAccents($name);
    $name = preg_replace('/[^a-zA-Z0-9@\.-]/u', '_', $name);
    $name = preg_replace('/(_){2,}/u', '_', $name);
    $name = trim($name, ' _');
    return $name;
  }

  public static function onAppReset() {
    if (!defined('USR_PATH') || strlen(USR_PATH) < 2 || !is_dir(USR_PATH)) {
      S('log')->error('failure ['.USR_PATH.']', __METHOD__);
      return ;
    }
    Shell::removeDir(USR_PATH);
  }

}
