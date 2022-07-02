<?php

$configJson = file_get_contents('./config.json');
$config = json_decode($configJson);

$json = file_get_contents('php://input');
$data = json_decode($json);

$mysqli = new mysqli($config->host, $config->user, $config->password, $config->db);

if (mysqli_connect_error())
{
    die("can not connect db.");
}

$datetime = date('Y-m-d H:m:s');

$query = "INSERT INTO temp(`datetime`, `temp`) VALUES (`$datetime`, `$data->temp`);";

var_dump($query);

$stmt = $mysqli->prepare($query);
$result = $stmt->execute();

var_dump($stmt);
