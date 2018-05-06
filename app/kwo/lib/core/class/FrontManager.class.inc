<?php

class FrontManager
{
  private static $whitelist = array(
    'body',
    'error', 'captcha', 'delivery.open', 'xml.rpc',
    'repository.server', 'affiliate',
    'currency.set', 'upload.check',
    'admin.login', 'admin.redirect', 'admin.signin',
    'diagnosis', 'date.select',
    'campaign.ping', 'campaign.proxy', 'campaign.smtp',
    'campaign.coupon', 'campaign.view', 'campaign.remove',
    'file.get', 'file.manager', 'file.preview', 'file.upload',
    'oauthv1.tokens', 'oauthv2.tokens', 'oauth.login.callback', 'oauth.login',
    'sitemap', 'sitemapindex', 'track', 'locale.set',
    'newsletter.signup', 'newsletter.subscribe',
    'translation.edit', 'translation.list', 'translation.save',
    'translator', 'email.confirm', 'email.confirm.request',
    'signup', 'user.id', 'user.signup', 'user.logout', 'user.login',
    'valuation', 'valuation.save', 'password.send', 'varnish', 'redirect');

  public static function inWhitelist($action) {
    return S('req')->isFront() && in_array($action, self::$whitelist);
  }

}