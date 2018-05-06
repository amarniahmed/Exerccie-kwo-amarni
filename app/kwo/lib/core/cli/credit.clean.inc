<?php

$dbo->exec('DELETE FROM '.T('credit')
           .    ' WHERE expire_on < "'.DH::now().'"');

