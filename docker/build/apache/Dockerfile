FROM alpine:3.3

## Basic tools
RUN apk update && \
    apk upgrade && \
    apk add bash \
            curl \
            wget \
            ca-certificates \
            nano

## Timezone
ENV TIMEZONE Europe/Paris
RUN apk add --update tzdata &&\
    cp /usr/share/zoneinfo/${TIMEZONE} /etc/localtime &&\
    echo "${TIMEZONE}" > /etc/timezone

## Apache
RUN apk add apache2 \
            apache2-utils
RUN echo "IncludeOptional /etc/apache2/sites-enabled/*.conf" >> /etc/apache2/httpd.conf &&\
    echo "<Directory />" >> /etc/apache2/httpd.conf &&\
    echo "    Require all granted" >> /etc/apache2/httpd.conf &&\
    echo "</Directory>" >> /etc/apache2/httpd.conf &&\
    sed '/^DocumentRoot/d' /etc/apache2/httpd.conf -i &&\
    sed -e 's%#LoadModule expires_module%LoadModule expires_module%g' /etc/apache2/httpd.conf -i &&\
    sed -e 's%#LoadModule rewrite_module%LoadModule rewrite_module%g' /etc/apache2/httpd.conf -i &&\
    mkdir /run/apache2

## PHP
RUN apk add php-apache2 \
            php-bz2 \
            php-calendar \
            php-cli \
            php-ctype \
            php-curl \
            php-dom \
            php-gd \
            php-gettext \
            php-gmp \
            php-iconv \
            php-json \
            php-mcrypt \
            php-memcache \
            php-mysql \
            php-mysqli \
            php-openssl \
            php-pdo \
            php-pdo_mysql \
            php-pdo_sqlite \
            php-phar \
            php-posix \
            php-soap \
            php-sqlite3 \
            php-xmlreader \
            php-xmlrpc \
            php-zip \
            php-zlib

# Clean
RUN apk del tzdata &&\
    rm -rf /var/cache/apk/* &&\
    rm -rf /tmp/*

WORKDIR /var/web/app

ENTRYPOINT ["/usr/sbin/httpd", "-DFOREGROUND"]
