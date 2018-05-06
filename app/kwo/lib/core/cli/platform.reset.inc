<?php

if (Shell::confirm('Are you sure') ===  false) {
  $res->write("\nOops! aborting ... \n");
  return ;
}

Cache::clean();
Session::clean();
Logger::clean();
