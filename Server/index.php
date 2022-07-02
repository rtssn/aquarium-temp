<?php

require_once('./lib/WriteToDb.php');
require_once('./lib/TempData.php');

$writeToDb = new WriteToDb();

$json = file_get_contents('php://input');
$tempData = new TempData($json);

$result = $writeToDb->Insert($tempData->temp);

if ($result == true)
{
    echo "OK";
}
else
{
    echo "NG";
}
