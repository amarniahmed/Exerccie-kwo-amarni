<?php

if (!file_exists(Mail::DKIM_PRIV)) {
  Shell::exec('openssl genrsa -out '.Mail::DKIM_PRIV.' 384');
}

if (!file_exists(Mail::DKIM_PUB)) {
  Shell::exec('openssl rsa -in '.Mail::DKIM_PRIV.' -out '.Mail::DKIM_PUB.' -pubout -outform PEM');
}

$content = file_get_contents(Mail::DKIM_PUB);
$lines = explode("\n", $content) ;

$txt_record = Mail::DKIM_SELECTOR.'._domainkey IN TXT "v=DKIM1\; k=rsa\; g=*\; s=email\; h=sha1\; t=s\; p=';
foreach ($lines as $line) {
  if (strpos($line, '-----') === 0) continue ;
  $txt_record .= $line ;
}
$txt_record .= '\;"' ;

echo $txt_record;