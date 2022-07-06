<?php

require_once('./lib/AquariumTempDb.php');
require_once('./lib/TempData.php');

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
