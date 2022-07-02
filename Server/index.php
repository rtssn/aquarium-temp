<?php

require_once('./lib/AquariumTempDb.php');
require_once('./lib/TempData.php');

$aquariumTempDb = new AquariumTempDb();


if ($_SERVER["REQUEST_METHOD"] == "POST")
{
    $json = file_get_contents('php://input');
    $tempData = new TempData($json);

    $result = $aquariumTempDb->Insert($tempData->temp);

    if ($result == true)
    {
        echo "OK";
    }
    else
    {
        echo "NG";
    }
}
else
{
    header("Content-Type: application/json; charset=utf-8");
    $data = $aquariumTempDb->Select();
    $json = json_encode($data);
    echo $json;
}
