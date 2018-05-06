<?php

class Lead extends ActiveRecord {

  const STATUS_CLOSED = -10;
  const STATUS_PROCESSED = -5;
  const STATUS_PENDING = 0;
  const STATUS_TO_CALL_BACK = 5;
  const STATUS_PROCESSING = 10;
  const STATUS_PENDING_CONTROL = 15;
  const STATUS_PROCESSING_CONTROL = 20;
  const STATUS_PENDING_ASSIGNMENT = 30;
  const STATUS_TO_REASSIGN = 40;
  const STATUS_DISPATCHED = 50;
  const STATUS_ASSIGNED = 100;

  public static $statuses = array(self::STATUS_CLOSED => 'clos',
                                  self::STATUS_PROCESSED => 'traité',
                                  self::STATUS_PENDING => 'en cours',
                                  self::STATUS_TO_CALL_BACK => 'à rappeler',
                                  self::STATUS_PROCESSING => 'en traitement',
                                  self::STATUS_PENDING_CONTROL => 'à contrôler',
                                  self::STATUS_PROCESSING_CONTROL => 'en cours de contrôle',
                                  self::STATUS_PENDING_ASSIGNMENT => 'en attente d\'attribution',
                                  self::STATUS_TO_REASSIGN => 'à réattribuer',
                                  self::STATUS_DISPATCHED => 'envoyé pour attribution',
                                  self::STATUS_ASSIGNED => 'attribué');

  const TYPE_FORM = 1;
  const TYPE_WCB = 2;
  const TYPE_CONTACT = 3;

  public static $types = array(self::TYPE_FORM => 'formulaire web',
                               self::TYPE_WCB => 'web call back',
                               self::TYPE_CONTACT => 'formulaire contact');

  const MOTIVE_NONE = 0;
  const MOTIVE_INVALID = 1;
  const MOTIVE_BLACKLIST = 2;
  const MOTIVE_UNREACHABLE = 3;

  public static $motives = array(self::MOTIVE_NONE => '-',
                                 self::MOTIVE_INVALID => 'invalide',
                                 self::MOTIVE_BLACKLIST => 'blacklisté',
                                 self::MOTIVE_UNREACHABLE => 'injoignable');

  const PRIORITY_LOW = 0;
  const PRIORITY_NORMAL = 1;

  public static $priorities = array(self::PRIORITY_LOW => 'faible',
                                    self::PRIORITY_NORMAL => 'normale');



  public function isClosed() {
    return $this->status == self::STATUS_CLOSED;
  }

  public function isProcessed() {
    return $this->status == self::STATUS_PROCESSED;
  }

  public function isPending() {
    return $this->status == self::STATUS_PENDING;
  }

  public function isAssigned() {
    return $this->status == self::STATUS_ASSIGNED;
  }

  public function lock($user) {
    $this->setUser($user);
  }

  public function withdraw() {
    if (!$this->isAssigned()) return;
    $this->commit(array('status' => self::STATUS_TO_REASSIGN,
                        'user_id' => 0));
  }

  public function addCall() {
    $this->commit(array('called_at' => DH::now(),
                        'calls' => ($this->calls + 1)));
  }

  public function setAppointment($date) {
    $this->commit(array('call_date' =>DH::toDb($date),
                        'status' => self::STATUS_TO_CALL_BACK));
  }

  // KWO -----------------------------------------------------------------------

  public function isOnline() {
    return $this->status >= self::STATUS_PENDING;
  }

  public function asEditHash() {
    if (!$this->isItem()) return array();
    $datas = current($this->datas);
    $html = '';
    $html .= '<table class="fixed">'."\n";
    $html .= '<thead>'."\n";
    $html .= '<tr>'."\n";
    $html .= '<th>Champ</th>'."\n";
    $html .= '<th>Valeur</th>'."\n";
    $html .= '</tr>'."\n";
    $html .= '</thead>'."\n";
    $html .= '<tbody>'."\n";
    foreach ($this->datas as $code => $datas) {
      $html .= '<tr><td colspan="2" style="font-weight:bold; text-align:center;">'.strtoupper($code).'</td></tr>'."\n";
      foreach ($datas as $field) {
        $html .= '<tr>'."\n";
        $html .= '<td>'.$field['label'].'</td>'."\n";
        $html .= '<td>'.$field['value'].'</td>'."\n";
        $html .= '</tr>'."\n";
      }
    }
    $html .= '</tbody>'."\n";
    $html .= '</table>'."\n";
    $h = array('datas' => $html);
    return $h;
  }

  public function asFinderHash($h) {
    $contact = '';
    if ($this->contact_id > 0) {
      $contact = $this->getContact();
      $contact = $contact->first_name.' '.$contact->last_name;
    }
    $h['contact'] = $contact;
    $h['status'] = c(self::$statuses[$this->status]);
    $h['type'] = c(self::$types[$this->type]);
    return $h;
  }

  // HOOKS ---------------------------------------------------------------------

  public function onAfterCommit($hash) {
    // @todo : set name ?
  }

  public function onAfterStatusChange($old_status) {
    S('app')->hook('onLeadStatusChange',
                   array('lead' => $this,
                         'old_status' => $old_status));
  }

}