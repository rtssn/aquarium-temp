<?php

require_once('./lib/AquariumTempDb.php');
require_once('./lib/TelemetryData.php');

$method = $_SERVER["REQUEST_METHOD"];

switch ($method)
{
    case "POST":
        Post();
        break;

    case "GET":
        Get();
        break;
}

/**
 * POST字の処理です。
 */
function Post()
{
    $aquariumTempDb = new AquariumTempDb();
    $json = file_get_contents('php://input');
    $telemetryData = new TelemetryData($json);

    $logFile = './log';

    $datetime = date('Y-m-d H:i:s');
    $log = "$datetime: $json \n";

    file_put_contents($logFile, $log, FILE_APPEND);

    $temp = $telemetryData->sensors[0]->temp;
    file_put_contents($logFile, $temp . "\n", FILE_APPEND);

    $result = $aquariumTempDb->Insert($temp);

    if ($result == true)
    {
        echo "OK";
    }
    else
    {
        echo "NG";
    }
}

/**
 * GET時の処理です。
 */
function Get()
{
    $aquariumTempDb = new AquariumTempDb();
    header("Content-Type: application/json; charset=utf-8");
    $data = $aquariumTempDb->Select();
    $json = json_encode($data);
    echo $json;
}
