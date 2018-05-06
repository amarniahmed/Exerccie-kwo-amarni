<?php 

$ctx->user_select   = 'user-select';
$ctx->box_sizing    = 'box-sizing';
$ctx->border_radius = 'border-radius';
$ctx->outline       = 'outline';
$ctx->opacity       = 'opacity';
$ctx->cursor        = 'pointer';

$browser = $req->getRemoteClient();

if (strpos($browser, 'firefox') !== false) {
  $prefix = '-moz-';
  $ctx->box_sizing    = $prefix.'box-sizing';
  $ctx->border_radius = $prefix.'border-radius';
  $ctx->outline       = $prefix.'outline';
  $ctx->user_select   = $prefix.'user-select';
  //  $ctx->opacity       = $prefix.'opacity';
  $ctx->opacity       = 'opacity';
}
elseif (strpos($browser, 'msie') !== false) {
  $ctx->cursor = 'hand';
}
elseif (strpos($browser, 'webkit') !== false) {
  $prefix = '-webkit-';
  $ctx->box_sizing    = $prefix.'box-sizing';
  $ctx->border_radius = $prefix.'border-radius';
  $ctx->user_select   = $prefix.'user-select';
}
elseif (strpos($browser, 'opera') !== false) {
  $ctx->box_sizing = 'box-sizing';
}
else {
  $prefix = '-ms-';
  $ctx->box_sizing = $prefix.'box-sizing';
}
// DejaVu Sans Mono, Droid Sans Mono,
$ctx->font = 'Kwo Sans, Droid Sans, Gill Sans,Tahoma,Lucida Sans Unicode,Sans';
$ctx->font_size = UiManager::FONTSIZE;

$ctx->color1 = UiManager::COLOR1;
$ctx->color2 = UiManager::COLOR2;
$ctx->color3 = UiManager::COLOR3; 

$ctx->input_color_on  = '#f8f7f3';
$ctx->input_color_off = '#f5f2e9';

$ctx->light_color  = '#eee';

$res->cache();

$res->setContentType('css');
$res->useTemplates(null, true);

//  $ctx->opacity    = 'filter:alpha(opacity=70)';