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

    ob_start();
    var_dump($telemetryData);
    $result = ob_get_clean();
    file_put_contents($logFile, $result . "\n", FILE_APPEND);

    $sensor1Temp = $telemetryData->sensors[0]->temp;
    $sensor2Temp = $telemetryData->sensors[1]->temp;

    $result = $aquariumTempDb->Insert($sensor1Temp, $sensor2Temp, $telemetryData->isFanOn, $telemetryData->ipAddress);

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
