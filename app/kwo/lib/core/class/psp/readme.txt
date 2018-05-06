/* --------------------------------------------------------------------------------------*/

/***
 - psp.type : cybermut | paybox | mercanet | paypal | ogone
 - mode : test | production
***/

// parameters : cybermut
//  - bank : cm | cic | obc
/***
$parameter['params'] = array('tpe' => '6058032',
                                     'bank' => 'cm',
                                     'societe' => 'annakaszer',
                                     'key' => 'd17d02caeebb7736035f4e8162f232dc704a2744');
***/


// parameters : mercanet
/***
$parameter['params'] = array('merchant_id' => '039788912200017');
***/


// parameters : paybox
/***
$parameter['params'] = array('site' => '1999888',
                                     'rang' => '99',
                                     'identifiant' => '2');
***/

// parameters : paypal
/***
$parameter['params'] = array('url' => 'https://www.paypal.com/cgi-bin/webscr',
                                     'args' => array('business' => 'fxbois@kernix.com'));
***/



// parameters : ogone
// - mode : test | prod
/***
$parameter['params'] = array('pspid' => 'BALINEA');
***/

