<?php

class Campaign extends ActiveRecord 
{
  const STATUS_CANCELED = -10;
  const STATUS_DRAFT = 0;
  const STATUS_SCHEDULED = 1;
  const STATUS_PROCESSING = 2;
  const STATUS_DELIVERED = 3;

  public static $statuses = array(self::STATUS_CANCELED => 'annulée',
                                  self::STATUS_DRAFT => 'en cours de rédaction',
                                  self::STATUS_SCHEDULED => 'prête pour l’envoi', 
                                  self::STATUS_PROCESSING => 'en cours d’envoi',
                                  self::STATUS_DELIVERED => 'finalisée');

  const FLAG_BLACKLIST = 1;

  public static $flags = array(self::FLAG_BLACKLIST => 'utiliser la blacklist');

  protected function onBeforeCommit(&$attrs) {
    if (is_array($attrs['bases'])) {
      $attrs['bases'] = join(',', $attrs['bases']);
    }
    if ($attrs['body_id']) {
      $body = Body::getInstance($attrs['body_id']);
      $attrs['content_html'] = $body->content_html;
      $attrs['content_text'] = $body->content_text;
      $attrs['body_id'] = 0;
    }
  }

  protected function onBeforeStatusChange(&$attrs) {
    if (!$this->isItem()) return ;
    if ($this->status == self::STATUS_DELIVERED && $attrs['status'] < self::STATUS_DELIVERED) {
      throw new ActiveRecordException('cette campagne est terminée.');
    }
  }

  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM '.Smtp::TABLE
                     .    ' WHERE campaign_id='.$this->getId());
    $this->dbo->exec('DELETE FROM '.Act::TABLE
                     .    ' WHERE campaign_id='.$this->getId());
  }

  public function isDelivered() {
    return $this->status >= self::STATUS_DELIVERED;
  }

  public function getStats() {
    $acts = $this->acts();
    $hash = array('début de l’envoi' => $this->scheduled_at,
                  'fin de l’envoi' => $this->delivered_at,
                  'nbre destinaitaires' => (int) $acts[Act::TYPE_SENDING],
                  'nbre de messages aboutis' => (int) $acts[Act::TYPE_SUCCESS],
                  'nbre de messages ouverts' => (int) $acts[Act::TYPE_OPENING],
                  'nbre de cliques uniques' => (int) $acts[Act::TYPE_REDIRECTION],
                  'nbre d’erreurs temporaires' => (int) $acts[Act::TYPE_SOFTBOUNCE],
                  'nbre d’erreurs définitives' => (int) $acts[Act::TYPE_HARDBOUNCE],
                  'nbre de désabos' => (int) $acts[Act::TYPE_REMOVE],
                  'nbre de désabos (blacklist)' => (int) $acts[Act::TYPE_BLACKLIST]);
    $rs = $this->dbo->query('SELECT c.model_id, COUNT(c.model_id)'
                            .' FROM '.T('conversion').' AS c,'
                            .     ' '.Act::TABLE.' AS a'
                            .' WHERE c.visit_id=a.value'
                            .  ' AND c.converted_at >= "'.$this->scheduled_at.'"'
                            .  ' AND a.type='.Act::TYPE_VISIT
                            .  ' AND a.campaign_id='.$this->getId()
                            .' GROUP BY c.model_id');
    foreach ($rs->asHash() as $model_id => $count) {
      $hash['conversion - '.Model::name($model_id)] = $count;
    }
    if ($acts[Act::TYPE_REDIRECTION] >= 1) {
      $urls = $this->urls();
      foreach ($urls as $url_id => $count) {
        $url = str_replace(array('http://', 'https://'), 
                           array('', ''),
                           Url::asString($url_id));
        $hash['clic - '.$url] = $count;
      }
    }
    return $hash;
  }

  public function getBases() {
    $opts = array('filters' => array('id' => $this->bases));
    return Item::collect('base', $opts);
  }

  private function acts() {
    return $this->dbo->asHash('SELECT type, COUNT(type) AS n'
                              .' FROM '.Act::TABLE
                              .' WHERE campaign_id='.$this->getId()
                              .' GROUP BY campaign_id, type');
  }

  private function urls() {
    return $this->dbo->asHash('SELECT value, COUNT(type) AS n'
                              .' FROM '.Act::TABLE
                              .' WHERE campaign_id='.$this->getId()
                              .  ' AND type='.Act::TYPE_REDIRECTION
                              .' GROUP BY type, value');
  }

  public function getTextContent() {
    $content = $this->content_text;
    if (stripos($content, '((link))') === false && 
        stripos($content, '/push/view?((sig))') === false) {
      $content .= 'cliquez-ici : ((link)) pour visualisez ce message dans un navigateur web'."\n";
    }
    if (stripos($content, '((remove))') === false && 
        stripos($content, '/push/remove?((sig))') === false) {
      $content .= 'cliquez-ici : ((remove)) pour vous désinscrire'."\n";
    }
    return $content;
  }

  public function getHtmlContent() {
    $content = $this->content_html; 
    $content = preg_replace('/a\s+href\s*=\s*/Di', 'a href=', $content);
    $hostname = P('app.domain');
    $content = str_replace(array('"/'.DOC_PATH,
                                 "'/".DOC_PATH,
                                 '=/'.DOC_PATH,
                                 'url(/'.DOC_PATH, 
                                 '%28%28', '%29%29'),
                           array('"http://'.$hostname.'/'.DOC_PATH,
                                 "'http://".$hostname.'/'.DOC_PATH,
                                 '=http://'.$hostname.'/'.DOC_PATH,
                                 'url(http://'.$hostname.'/'.DOC_PATH,
                                 '((', '))'),
                           $content);
    
    if (!preg_match('/<body/i', $content)) {
      $html  = '<html>'."\n";
      $html .= '<head>'."\n";
      $html .= '<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>'."\n";
      $html .= '</head>'."\n";
      $style = '';
      if ($this->style) {
        $style = ' style="'.$this->style.'"';
      }
      $html .= '<body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0"'.$style.'>'."\n";
      $html .= $content."\n";
      $html .= '</body>'."\n";
      $html .= '</html>';
      $content = $html;
    }
    if (stripos($content, '((remove))') === false && 
        stripos($content, '/push/remove?((sig))') === false) {
      $content = str_ireplace('</body>', 
                              '<br/>((remove)) pour vous désinscrire'."\n".'</body>', 
                              $content);
    }
    if (stripos($content, '((open))') === false && 
        stripos($content, '/push/ping?((sig))') === false) {
      $content = str_ireplace('</body>', 
                              '((open))</body>',
                              $content);
    } 
    return $content;
  }

  public function getUrl() {
    return H::url('/push');
  }

  public function send($bases=null) {
    S('app')->hook('onBeforeCampaignSend',
                   array('campaign' => $this,
                         'bases' => &$bases));
    $sender = new Sender($this, $bases);
    S('app')->hook('onAfterCampaignSend',
                   array('campaign' => $this));
  }

  public static function bases($test=false) {
    $opts = array();
    $key = $test ? '|AND|flags' : '|!AND|flags';
    $opts['filters'][$key] = Base::FLAG_TEST;
    return Base::collect($opts)->asHash('id', 'name');
  }
  
  public static $exports = array(Act::TYPE_HARDBOUNCE => 'erreurs définitives',
                                 Act::TYPE_SOFTBOUNCE => 'erreurs temporaires',
                                 Act::TYPE_OPENING => 'ouvreurs',
                                 Act::TYPE_REDIRECTION => 'cliqueurs',
                                 Act::TYPE_REMOVE => 'désabonnés',
                                 Act::TYPE_BLACKLIST => 'blacklist');

  public static function getInstanceByItem($item) {
    $model_id = $item->getModelId();
    $record_id = $item->getId();
    $id = (int) S('dbo')->fetchValue('SELECT id'
                                     .' FROM '.T('campaign')
                                     .' WHERE model_id='.$model_id
                                     .  ' AND record_id='.$record_id
                                     .' LIMIT 1');
    $h = $item->asCampaignHash(); 
    if ($h['status'] > self::STATUS_SCHEDULED) {
      unset($h['status']);
      S('log')->trace('invalid status ('.$h['status'].')', __METHOD__);
    }
    unset($h['delivered_at']);
    $fields = S('dbo')->getTable(T('campaign'))->getFields();
    $attrs = array();
    foreach ($h as $key => $value) {
      if (!in_array($key, $fields)) continue ;
      $attrs[$key] = $value; 
    }
    $attrs['model_id'] = $model_id;
    $attrs['record_id'] = $record_id;
    $attrs['locale'] = $attrs['locale'] ?: Locale::FR;
    $attrs['from_name'] = $attrs['from_name'] ?: P('mail.from.name');
    $attrs['from_email'] = $attrs['from_email'] ?: P('mail.from.email');
    $attrs['reply_email'] = $attrs['reply_email'] ?: P('mail.from.email');
    if ($id < 1) {
      $campaign = Campaign::getInstance();
      $campaign->commit($attrs);
      return $campaign;
    }
    $campaign = Campaign::getInstance($id);
    if ($campaign->status >= self::STATUS_PROCESSING) {
      return $campaign;
    }
//    S('log')->debug($attrs);
    $campaign->commit($attrs);
    return $campaign;
  }

}
