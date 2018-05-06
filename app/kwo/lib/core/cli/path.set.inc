<?php

foreach (User::collect('User') as $user) {
  $path = $user->getPath();
  $path = substr($path, 9);
  $path = trim($path, '/');
  $user->setAttribute('path', $path);
}