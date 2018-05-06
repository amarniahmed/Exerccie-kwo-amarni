<?php

try {

  $body = Body::getInstance($req->id);
  $res->resetTemplates();
  $res->write("<html>\n<body>".$body->content_html."</body>\n</html>");

}
catch (Exception $e) { $err->add($e); }