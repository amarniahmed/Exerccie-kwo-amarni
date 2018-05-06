<?php 

/*********************************************************
  ShortCode class
  Old Regex v.1 : /(\[.*:.*\#?.*\]((.+\n?)+)\[\/.*\])|(\[.*\]((.+\n?)+)\[\/.*\])|(\[.*:.*\#?.*\])|(\[.*\])/U
  Old Regex v.2 : /(\[(\w+)\b:(.*\#?.*)\]((.+\n?)*)\[\/.*\])|(\[.*\](.*)\[\/.*\])|(\[.*:.*\#?.*\])|(\[.*:.*\])|(\[.*\])/U
  Old Regex v.3 : /\[(\w+)(\W?)(\w+\#?\w+)(?:(\/))?\](?:(?<=(\/)\])|((.+\n?)*)\[\/.*\])/U
**********************************************************/

class ShortCode {
  
  CONST ANCHOR_CODE = 'ancre';
  CONST SHARE_CODE  = 'share';
  CONST HOOK_METHOD = 'onShortCodeReplace';
  CONST ITEM_METHOD = 'asShortCodeContent';
  CONST CONFIG_CODE = 'app.shortcodes.codes';
  CONST REGEX       = '(\[(\w+)\b(:*)(.*\#?.*)\/\])|(\[(\w+)\b:(.*\#?.*)\]((.+\n?)*)\[\/.*\])|(\[(\w+)\](.*)\[\/.*\])';

  public static function replace($content=null, $locale=I18N::LOCALE_FR) {
    if (!$content) {
      return '';
    } 
    $app = S('app');
    if (!P(self::CONFIG_CODE)) {
      return $content;
    }
    $matches = array();
    $short_codes = P(self::CONFIG_CODE);
    if (preg_match_all('/'.self::REGEX.'/U', $content, $matches)) {     
      $matches = $matches[0];      
      rsort($matches);       
      $manager = $app->getManager(); 
      if ($app->hasManager() && method_exists($manager, self::HOOK_METHOD)) {
        $app_hook_method = self::HOOK_METHOD;
      } 
      $item_hook_method = self::ITEM_METHOD;
      foreach ($matches as $match) {   
        $replace = $anchor = null;    
        list($short_code, $args) = explode(':', substr($match, 1, strpos($match, ']')-(strpos($match, '/]') > 0 ? 2 : 1)));
        if (!in_array($short_code, $short_codes)) continue;
        if (strpos($args, '#') > 0) list($args, $anchor) = explode('#', $args);
        if (strpos($match, '[/') > 0) $label = substr($match, strpos($match, ']')+1, strpos($match, '[/')-strpos($match, ']')-1);
        else $label = '';
        switch ($short_code) {
          case self::ANCHOR_CODE:
            $replace = self::asAnchorContent($args);
          break;
          case self::SHARE_CODE:
            $replace = self::asShareContent();
          break;
          default:
            if ($app_hook_method) {
              $replace = $manager::$app_hook_method(array('short_code' => $short_code,
                                                          'args'  => $args,
                                                          'label' => $label));
            } 
            if (!$replace) {
              if (!Model::exists($short_code)) continue;
              if (is_numeric($args) && Item::exists($short_code, $args)) {       
                $item = Item::getInstance($short_code, $args);
              } 
              elseif (Item::exists($short_code, array('code' => $args))) { 
                $item = Item::getInstance($short_code, array('code' => $args));
              } 
              else { 
                $item = null;
              }
              $opts = array();
              if ($label)  $opts['label'] = $label;
              if ($anchor) $opts['anchor'] = $anchor;    
              $item_hook_method = $item && $item->locales > 0 ? $item_hook_method : 'asHtml';   
              $replace = $item ? (method_exists($item, $item_hook_method) ? $item->$item_hook_method($opts) 
                                                                          : self::asDefaultContent($item, $opts)) : $label;
            }
        }           
        $content = str_replace($match, $replace, $content);     
      }
    }
    return $content;
  }
  
  public static function asDefaultContent($item, $opts) {
    $content = $opts['label'] ?: $item->getName();
    $url = $item->asUrl();
    if ($opts['anchor']) {
      $url .= '#'.$opts['anchor'];
    }
    return '<a href="'.$url.'">'.$content.'</a>';
  }
  
  public static function asAnchorContent($name='') {
    return '<a name="'.$name.'"></a>';
  }
  
  public static function asShareContent() {         
    return Template::render('social.share');
  }
  

}
