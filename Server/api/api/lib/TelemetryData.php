<?php

require_once('TempData.php');

/**
 * 温度データを表すクラスです。
 */
class TelemetryData
{
    /**
     * @var int IPアドレスです。
     */
    public $deviceId = "";

    /**
     * @var array[TempData] センサー一覧です。
     */
    public $sensors = array();

    /**
     * @var string IPアドレスです。
     */
    public $ipAddress = "";

    /**
     * @var int ファン制御の状態です。
     */
    public $isFanOn = 0;


    /**
     * 温度データのコンストラクタです。
     * @param string パースするJSONを指定します。
     */
    function __construct($json)
    {
        $data = json_decode($json);

        if (isset($data->message) == false)
        {
            $this->sensors = $data->sensors;

            foreach ($data->sensors as $index => $sensor)
            {
                $tempData = new TempData();
                $tempData->name = "sensor " . $index;
                $tempData->temp = floatval($sensor->temp);

                $sensors[] = $tempData;
            }

            $this->deviceId = $data->deviceId;
            $this->ipAddress = $data->ipAddress;
            $this->isFanOn = $data->isFanOn;
        }
    }
}
