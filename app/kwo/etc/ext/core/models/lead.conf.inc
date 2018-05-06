<?php

$parameter['id'] = 814;
$parameter['name'] = 'lead';
$parameter['has_many'] = array('beneficiary' => array('through' => 'core_lead_beneficiary'));
$parameter['casts'] = array('datas' => 'hash',
                            'description' => 'hash');