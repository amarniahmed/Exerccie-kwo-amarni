<?php

try {

  $visit = Visit::getInstance((int) $req->id);

  $summary = $visit->getSummary();

  $h = array();
  $h[l('date')] = $summary['created_on'];
  $h[l('heure')] = substr($summary['created_at'], 0, 5);
  $h[l('app')] = $summary['app'];

  $h[l('id visite')] = $visit->id;
  $h[l('id visiteur')] = $visit->visitor_id;
  $link = $visit->number;
  if ($visit->number > 1) {
    $link = '<a href="javascript:void(0)" class="button-siblings">'.$visit->number.'</a>';
  }
  $h[l('visite num')] = $link;
  $h[l('précédente')] = $summary['span'] > 0 ? l('il y a ((count)) jour(s)', array('count' => $visit->span)) : '-';
  $h[l('durée')] = intval($visit->duration / 60).' mn ';
  if ($visit->duration % 60) {
    $h[l('durée')] .= ($visit->duration % 60).' s';
  }
  $link = $visit->depth;
  if ($visit->depth > 0) {
    $link = '<a href="javascript:void(0)" class="button-path">'.$visit->depth.' pages</a>';
  }
  $h[l('parcours')] = $link;
  $value = '0';
  $conversions = array();
  if ($visit->conversions > 0) {
    $value = $visit->conversions;
    $conversions = Conversion::asRows($visit);
  }
  $h[l('client ip')] = '<a href="javascript:void(0)" class="button-siblings" data-type="ip">'.$summary['ip'].'</a>';
  $h[l('client host')] = '<a href="javascript:void(0)" class="button-ripe">'.$summary['host'].'</a>';
  if ($visit->term > 0) {
    $summary['referer'] = '<a href="javascript:void(0)" class="button-siblings" data-type="source">'.$summary['referer'].'</a>';
  }
  $h[l('origine')] = $summary['referer'];
  $h[l('système')] = $summary['system'];
  $h[l('pays')] = $summary['country'];
  if ($visit->user_id >= 1) {
    $h[l('user')] = $summary['user'];
  }

  $ctx->url = str_ireplace('http://', '', $summary['url']);
  $ctx->visit = $visit;
  $ctx->rows = $h;
  $ctx->conversions = $conversions;
  $ctx->events = $visit->getPath();
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }