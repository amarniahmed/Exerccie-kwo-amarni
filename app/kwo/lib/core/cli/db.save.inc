<?php

Shell::exec('mysqldump '.P('app.dsn.name').' -r db.sql');
Shell::exec('ls -lah db.sql');

