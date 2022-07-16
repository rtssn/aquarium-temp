<?php

require_once('TempData.php');

/**
 * 温度データを表すクラスです。
 */
class TelemetryData
{
    /**
     * @var array[TempData] センサー一覧です。
     */
    public $sensors = array();

    /**
     * @var string IPアドレスです。
     */
    public $ipAddress = "";

    public $isFanOn = false;


    /**
     * 温度データのコンストラクタです。
     * @param string パースするJSONを指定します。
     */
    function __construct($json)
    {
        $data = json_decode($json);

        $this->sensors = $data->sensors;

        foreach ($data->sensors as $index => $sensor)
        {
            $tempData = new TempData();
            $tempData->name = "sensor " . $index;
            $tempData->temp = floatval($sensor->temp);

            $sensors[] = $tempData;
        }

        $this->ipAddress = $data->ipAddress;
        $this->isFanOn = (bool)$data->isFanOn;
    }
}
