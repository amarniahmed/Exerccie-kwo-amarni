<?php

if ($req->hasAttribute('range')) {
  for ($i = 1; $i < 254; $i++) {
    $ip = $req->range.'.'.$i;
    if (IH::ping($ip, 22)) {
      $res->write($ip.' : OK'."\n");
    }
  }
}
elseif ($req->hasAttribute('ip')) {
  $ip = $req->ip;
  $port = $req->port ?: 22;
  if (IH::ping($ip, $port)) {
    $res->write($ip.' : OK'."\n");
  }
  else {
    $res->write($ip.' : OK'."\n");
  }
}
else {
  $res->write('pong ['.P('app.name').']');
}
