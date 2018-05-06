<?php

Session::clean();

Captcha::clean();

$dbo->query('DELETE FROM '.AuthManager::TABLE_ATTEMPT
            .    ' WHERE TIMESTAMPDIFF(DAY, created_at, NOW()) > 1'); 

$dbo->query('DELETE FROM '.Item::TABLE_TRACER
            .    ' WHERE TIMESTAMPDIFF(DAY, inserted_at, NOW()) > 1'); 


