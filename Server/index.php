<?php

require_once('./lib/write-to-db.php');
require_once('./lib/TempData.php');

$writeToDb = new WriteToDb();

$json = file_get_contents('php://input');
$tempData = new TempData($json);

$writeToDb->Insert($tempData->temp);
