<?php

class Group extends ActiveRecord {

  const TABLE_GROUP_USER = 'core_group_user';

  const FLAG_PRIVATE = 1;

  public static $flags = array(self::FLAG_PRIVATE => 'privé');

  public function onInvitationAccept(Invitation $invitation) {
    if ($invitation->recipient_model_id != Model::id('user')) return ;
    $row = array('group_id' => $this->getId(),
                 'user_id' => $invitation->recipient_record_id);
    $table = $this->dbo->getTable(self::TABLE_GROUP_USER);
    $table->insert($row, array('unique' => true));
  }

  public function onInvitationSend(Invitation $invitation) {
    $user = $invitation->getUser();
    $recipient = $invitation->getRecipient();
    $link = $this->asUrl();
    if ($recipient instanceof User) {
      $link = '/account/invitation.list';
    }
    Letter::send('invitation.group', $recipient,
                 array('user' => $user->name,
                       'group' => $this->name,
                       'link' => $link),
                 array('locale' => $locale));
  }

  public function asInvitationSnippet($invitation) {
    $msg = '<strong>'.$invitation->getUser().'</strong>';
    $msg .= ' vous propose de rejoindre le groupe "'.c($this->name).'".';
    return $msg;
  }
  
  public function asInvitationHTML($item, $locale=null) {
    return 'hello';
  }



}