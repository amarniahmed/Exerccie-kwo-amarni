<?php

class W {

  const WIDGET_CLASS = 'widget-action';

  public static function abuse(ActiveRecord $item, $opts=null) {
    $opts = (array) $opts;
    $class = $opts['class'] ?: 'widget-action-abuse';
    $icon  = $opts['icon']  ?: '<img src="'.PIX_URL.'/flag-off.png" />';
    $label = $opts['label'] ?: l('abuse.label');
    $title = $opts['title'] ?: l('abuse.title');
    $attrs = array('href' => 'javascript:void(0)',
                   'class' => array(self::WIDGET_CLASS, $class),
                   'title' => $title,
                   'on' => array('click' => 'new Kwo.Composer.Abuse(this)',
                                 'mouseover' => '$(this).turn()',
                                 'mouseout' => '$(this).turn()'),
                   'data' => array('item' => $item->getKey()));
    $html = H::elt('a', '<span>' . $icon . $label . '</span>', $attrs);
    return $html;
  }

  public static function advert($format=null, $opts=null) {
    return Advert::display($format, $opts);
  }

  public static function alert($item=null, $opts=null) {
    $opts = (array) $opts;
    $class = $opts['class'] ?: 'kwo-alert-button';
    $icon  = $opts['icon']  ?: '<img src="'.PIX_URL.'/empty.gif" />';
    $label = $opts['label'] ?: l('alert.widget.link');
    $title = $opts['title'] ?: $opts['title'];
    $datas = array();
    if ($item instanceof ActiveRecord) {
      $datas['item'] = $item->getKey();
    }
    else {
      $datas['keyword'] = str_replace('"', ' ', (string) $item);
    }
    $attrs = array('href' => 'javascript:void(0)',
                   'class' => array(self::WIDGET_CLASS, $class),
                   'title' => $title,
                   'on' => array('click' => 'new Kwo.Composer.Alert(this)'),
                   'data' => $datas);
    $html = H::elt('a', '<span>' . $icon . $label . '</span>', $attrs);
    return $html;
  }

  public static function auth() {
    $html = Template::render('widget.auth', true);
    return '<div id="kwo-auth-box">'.$html.'</div>';
  }

  public static function favorite(ActiveRecord $item, $opts=null) {
    // ajouter à ma sélection
    $opts = (array) $opts;
    $class = $opts['class'] ?: 'kwo-favorite-button';
    $icon  = $opts['icon']  ?: '<img src="'.PIX_URL.'/favorite.png" />';
    $label = $opts['label'] ?: l('favorite.widget.label');
    $title = $opts['title'] ?: $opts['title'];
    $attrs = array('href' => 'javascript:void(0)',
                   'class' => array(self::WIDGET_CLASS, $class),
                   'title' => $title,
                   'on' => array('click' => 'new Kwo.Composer.Favorite(this)'),
                   'data' => array('item' => $item->getKey()));
    $html = H::elt('a', '<span>' . $icon . $label . '</span>', $attrs);
    return $html;
  }

  public static function fan(ActiveRecord $item, $opts=null) {
    $opts = (array) $opts;
    $class = $opts['class'] ?: 'kwo-fan-button';
    $icon  = $opts['icon'] ?: '<img src="'.PIX_URL.'/fan.png" />';
    $label = $opts['label'] ?: l('fan.widget.label');
    $title = $opts['title'] ?: $opts['title'];
    $attrs = array('href' => 'javascript:void(0)',
                   'class' => array(self::WIDGET_CLASS, $class),
                   'title' => $title,
                   'on' => array('click' => 'new Kwo.Composer.Favorite(this)'),
                   'data' => array('item' => $item->getKey()));
    $html = H::elt('a', '<span>' . $icon . $label . '</span>', $attrs);
    return $html;
  }

  public static function captcha() {
    return '<span class="kwo-captcha-box">'.Template::render('captcha', true).'</span>';
  }

  public static function cart() {
    $cookie = S('req')->getCookie();
    $order_id = (int) $cookie->getAttribute('order_id');
    $html = c(l('error.cart.empty'));
    if ($order_id >= 1) {
      try {
        $order = Order::getInstance($order_id, array('ignore' => true));
        if ($order->countPurchases() >= 1) {
          $html = '<a href="javascript:void(Kwo.Cart.view())">';
          $html .= c(l('cart.show'));
          $html .= '<span class="kwo-cart-count"> ('.$order->countPurchases().' '.l('cart.item(s)').')</span>';
          $html .= '</a>';
        }
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
        $cookie->removeAttribute('order_id');
      }
    }
    return '<span id="kwo-cart-widget">'.$html.'</span>';
  }

  public static function comment(ActiveRecord $item, $opts=null) {
    $content = H::get('/comments',
                      array('item' => $item->getKey()),
                      array('item' => $item));
    $attrs = array('class' => 'widget-comments',
                   'data' => array('item' => $item->getKey()));
    $html = H::elt('div', $content, $attrs);
    return $html;
  }

  public static function currencies() {
    $opts = array('key' => 'currencies');
    return HttpClient::call('/currencies.widget', null, $opts);
  }

  public static function feed($url, $opts=array()) {
    $class = $opts['class'] ? $opts['class'] : 'kwo-feed-button';
    $label = $opts['label'] ? $opts['label'] : l('feed.widget.label');
    $icon = $opts['icon'] ? $opts['icon'] : PIX_URL.'/feed.icon.png';
    $title = $opts['title'] ? ' title="'.$opts['title'].'"' : '';

    $html  = '<a href="javascript:void(0)" class="widget-action '.$class.'"'.$title."\n";
    $html .= '   data-url="'.$url.'"';
    $html .= '   onclick="new Kwo.Composer.Feed(this)"><span><img src="'.$icon.'"/>'.$label.'</span></a>';

    return $html;
  }

  public static function form($args, $opts=array()) {
    if (is_numeric($args)) {
      $args = array('id' => $args);
    }
    elseif (!is_array($args)) {
      $args = array('code' => $args);
    }
    $locale = $opts['locale'] ?: S('res')->getLocale();
    $bindings = $opts['bindings'];
    $form = Form::getInstance($args);
    return $form->asHtml($locale, $bindings);
  }

  public static function locales($template=null) {
    $locales = S('app')->getLocales();
    $template = $template ?: 'locales';
    if (!is_array($locales)) {
      return '';
    }
    $bindings['locale'] = S('res')->getLocale();
    foreach ($locales as $locale) {
      $bindings['locales'][$locale] = I18N::$locale_labels[$locale];
    }
    return Template::render($template, $bindings);
  }

  public static function moderation($item, array $actions) {
    if (!S('req')->isModerator()) return ;
    $urls = array();
    foreach ($actions as $key => $value) {
      if (is_numeric($key)) {
        $key = $value;
      }
      if (empty($value)) continue ;
      if ($key === 'lock') {
        if (!$item->hasBit(Meta::BIT_LOCKED)) {
          $urls['fermer'] = '/item.lock?item='.$item->getKey();
        }
      }
      elseif ($key === 'pin') {
        if (!$item->hasBit(Meta::BIT_PINNED)) {
          $urls['figer'] = '/item.pin?item='.$item->getKey();
        }
      }
      elseif ($key === 'destroy') {
        $urls['supprimer'] = '/item.destroy?item='.$item->getKey();
      }
      elseif ($key === 'ban') {
        if ($item->user_id >= 1) {
          $urls['bannir'] = '/user.ban?user_id='.$item->user_id;
        }
      }
      else {
        $urls[$key] = $value.'/-/item/'.$item->getKey();
      }
    }
    if (count($urls) < 1) return '';
    foreach ($urls as $label => &$url) {
      if ($url[0] === '/') {
        $url = 'Kwo.exec(\''.$url.'\', null, {confirm:true, callback:true})';
      }
      elseif ($url[0] ===  'D') {
        $url = 'new Kwo.Dialog(\''.substr($url, 1).'\')';
      }
      $url = '<a href="javascript:void(0)" onclick="'.$url.'">'.$label.'</a>';
    }
    $html = '<div class="kwo-moderator-actions">'."\n";
    $html .= join(' - '."\n", $urls);
    $html .= '</div>';
    return $html;
  }

  public static function newsletter($code, $opts=array()) {
    try {
      $item = Base::getInstance(
                                array('code' => $code,
                                      'locale' => S('res')->getLocale()));
    }
    catch (Exception $e) { return 'Oops! invalid newsletter ('.$code.')!'; }

    $class = $opts['class'] ? ' '.$opts['class'] : ' kwo-newsletter-button';
    $label = $opts['label'] ? : l('widget.push.label');
    $icon = $opts['icon'] ?: PIX_URL.'/newsletter.icon.png';
    $title = $opts['title'] ? ' title="'.$opts['title'].'"' : '';

    $data = ' data-item="'.$item->getKey().'"';
    $html  = '<a href="javascript:void(0)" class="widget-action'.$class.'"'.$title."\n";
    $html .= '   onclick="new Kwo.Class.Newsletter(this)"'.$data.'><span><img src="'.$icon.'"/>'.$label.'</span></a>';

    return $html;
  }

  public static function opinion($item) {
    $html  = '<span class="utils opinion-utils">'."\n";
    $html .= self::thumb($item);
    $html .= '</span>';
    return $html;
  }

  public static function poll($code) {
    return Poll::getInstance(array('code' => $code))->asHtml();
  }

  public static function rate(ActiveRecord $item, $opts=array()) {
    $args = array();
    $args['max'] = Valuation::RATE_MAX;
    $args['item'] = $item->getKey();
    if ($args['value'] === true || empty($args['value'])) {
      $args['value'] = (int) Meta::get($item, Meta::FIELD_RATING);
    }
    else {
      $args['value'] = (int) $args['value'];
    }
    return Template::render('rating.notes', $args);
  }

  public static function share($item=null, $opts=null) {
    $opts = (array) $opts;
    if (!($item instanceof ActiveRecord)) {
      $item = is_string($item) ? $item : '/';
      $item = Url::asItem($item);
    }
    $datas = ' data-item="'.$item->getKey().'"';
    if ($opts['count'] === true) {
      $n = $item->getMeta(Meta::FIELD_SHARES);
      if ($n >= 1) {
        $label .= ' | '.$n;
      }
    }
    if (isset($opts['mode'])) {
      $datas .= ' data-mode="'.$opts['mode'].'"';
    }
    $class = $opts['class'] ? $opts['class'] : 'kwo-share-button';
    $label = isset($opts['label']) ? $opts['label'] : l('share.widget.label');
    $icon  = isset($opts['icon']) ? $opts['icon'] : PIX_URL.'/share/share-icon-16x16.png';
    $img = '';
    if (!empty($icon)) {
      $img = '<img src="'.$icon.'" alt="" />';
    }
    if (strpos($img, '.png') !== false) {
      $class .= ' png';
    }
    $title = $opts['title'] ? ' title="'.$opts['title'].'"' : '';
    $html  = '<a href="javascript:void(0)" class="widget-action '.$class.'"'.$title."\n";
    $html .= '   onclick="new Kwo.Composer.Share(this)"'.$datas.'><span>'.$img.$label.'</span></a>';
    return $html;
  }

  public static function thumb($item) {
    $metas = $item->getMetas(Meta::FIELD_DISLIKES, Meta::FIELD_LIKES);
    $args = array('item' => $item->getKey(),
                  'values' => array('up' => (int) $metas[Meta::FIELD_LIKES],
                                    'down' => (int) $metas[Meta::FIELD_DISLIKES]));
    return Template::render('rating.thumbs', $args);
  }

  public static function like($item, $opts=null) {
    $opts = (array) $opts;
    $attributes = array('class' => array('widget-i-like'),
                        'on' => array('click' => 'new Kwo.Class.Like(this)'),
                        'data' => array('item' => $item->getKey()));
    $label = $opts['label'] ?: 'I like';
    if (($n = $item->getMeta(Meta::FIELD_LIKES)) > 0) {
      $label .= ' <span>('.$n.')</span>';
    }
    return H::elt('span', $label, $attributes);
  }

  public static function tracker($name, $group=null) {
    S('req')->track($name, $group=null);
  }

}
