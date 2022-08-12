<?php

require_once('./lib/AquariumTempDb.php');
require_once('./lib/TelemetryData.php');
require_once('./lib/Log.php');

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

    Log::Write($json);

    $telemetryData = new TelemetryData($json);

    if ($telemetryData->deviceId != "")
    {
        Log::VarDumpLogWrite($telemetryData);

        $sensor1Temp = $telemetryData->sensors[0]->temp;
        $sensor2Temp = $telemetryData->sensors[1]->temp;

        $result = $aquariumTempDb->Insert($sensor1Temp, $sensor2Temp, $telemetryData->isFanOn, $telemetryData->ipAddress, $telemetryData->deviceId);

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
        echo "NG";
    }
}

/**
 * GET時の処理です。
 */
function Get()
{
    $now = isset($_GET['now']);

    $aquariumTempDb = new AquariumTempDb();
    header("Content-Type: application/json; charset=utf-8");
    $data = $aquariumTempDb->GetData();


    if ($now == false)
    {
        $json = json_encode($data);
        echo $json;
    }
    else
    {
        $now = $data[count($data) - 1];
        $json = json_encode($now);
        echo $json;
    }
}
