<?php

/**
 * 温度データを表すクラスです。
 */
class TempData
{
    /**
     * @var float 温度です。
     */
    public $temp;

    /**
     * 温度データのコンストラクタです。
     * @param string パースするJSONを指定します。
     */
    function __construct($json)
    {
        $data = json_decode($json);

        $this->temp = floatval($data->temp);
    }
}
