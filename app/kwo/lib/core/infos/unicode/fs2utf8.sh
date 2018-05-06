#!/bin/sh

/usr/bin/find . -type f \( -name '*.php' -o -name '*.inc' -o -name '*.psp' -o -name '*.conf' -name '*.css' -o -name '*.js' -o -name '*.xml' \) -print -exec iconv -f ISO-8859-1 -t UTF-8 {} -o {}.utf8 \;
/usr/bin/find . -type f -name '*.utf8' -print -exec rename .utf8 '' {} \;

