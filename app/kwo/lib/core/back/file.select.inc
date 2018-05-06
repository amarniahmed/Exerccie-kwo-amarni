<?php

$req->getSession()->setAttribute('selected_file_path',
                                 trim($req->file_path, '/'));

$res->write("Kwo.warn('".c(l('le fichier a bien été selectionné'))."')");