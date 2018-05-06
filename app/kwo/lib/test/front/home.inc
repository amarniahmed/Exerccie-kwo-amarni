<?php

try {

  $opts = array(
    'include' => array(
      'residence',
      'building',
      'Settlement',
    ),
    'filters' => array(
      'name' => Residence::nom_residence,
      'name'=>Building::nom_batiment,
      'name'=>Settlement::Loft,
     ),
  )

  $settlements = Settlement::collect($opts);

  $ctx->settlements = $settlements;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
