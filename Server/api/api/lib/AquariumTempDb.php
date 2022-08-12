<?php

require_once('Log.php');

/**
 * 温度情報データベースを操作するクラスです。
 */
class AquariumTempDb
{
    /**
     * @var Config 設定情報です。
     */
    private $config = null;

    /**
     * @var mysqli データベースのクライアントです。
     */
    private $db = null;

    /**
     * データベースへの接続をコンストラクタで初期化します。
     */
    function __construct()
    {
        $configJson = file_get_contents(__DIR__ . '/../config.json');
        $this->config = json_decode($configJson);

        $this->ConnectToDb();
    }

    /**
     * データベースへ挿入を行います。
     * @param float $sensor1Temp センサー1の温度を指定します。
     * @param float $sensor2Temp センサー2の温度を指定します。
     * @param bool $isFanOn ファンが回転しているかを指定します。
     * @param string $ipAddress 送信元のIPアドレスを指定します。
     * @param string $ipAddress デバイスIDを指定します。
     * @return bool 結果を返します。trueなら成功、falseなら失敗を示します。
     */
    function Insert($sensor1Temp, $sensor2Temp, $isFanOn, $ipAddress, $deviceId)
    {
        $result = false;

        if ($this->db != null)
        {
            $datetime = date('Y-m-d H:i:s');
            $query = "INSERT INTO `temp` (`datetime`, `deviceId`, `sensor1Temp`, `sensor2Temp`, `isFanOn`, `ipAddress`) VALUES ('$datetime', '$deviceId', $sensor1Temp, $sensor2Temp, $isFanOn, '$ipAddress');";

            Log::VarDumpLogWrite($query);

            try
            {
                $stmt = $this->db->prepare($query);
                $result = $stmt->execute();
            }
            catch (Exception $ex)
            {
                Log::VarDumpLogWrite($ex);
            }
        }

        return $result;
    }

    function GetData($limit = 3600)
    {
        $list = $this->GetList($limit);
        $maxTemp = $this->GetMaxTemp('sensor2Temp');
        $minTemp = $this->GetMinTemp('sensor2Temp');

        $ret = array(
            'maxTemp' => $maxTemp,
            'minTemp' => $minTemp,
            'list' => $list
        );

        return $ret;
    }

    function GetList($limit = 3600)
    {
        $data = array();

        if ($this->db != null)
        {
            $query = "SELECT `datetime`, `sensor1Temp`, `sensor2Temp`, `isFanOn` FROM `temp` WHERE `datetime` > ( NOW( ) - INTERVAL 1 DAY ) ORDER BY `id` ASC LIMIT $limit;";


            $stmt = $this->db->prepare($query);
            $stmt->execute();

            $datetime = '';

            $datetime = '';
            $sensor1Temp = 0;
            $sensor2Temp = 0;
            $isFanOn = 0;

            $stmt->bind_result($datetime, $sensor1Temp, $sensor2Temp, $isFanOn);
            while ($stmt->fetch())
            {
                $data[] = array(
                    'datetime' => $datetime,
                    'sensor1Temp' => $sensor1Temp,
                    'sensor2Temp' => $sensor2Temp,
                    'isFanOn' => $isFanOn
                );
            }
        }

        return $data;
    }

    function GetMaxTemp($sensorName)
    {
        $ret = array();

        if ($this->db != null)
        {
            $query = "SELECT `datetime`, `$sensorName` FROM `temp` WHERE `$sensorName` = (SELECT MAX(`$sensorName`) FROM temp WHERE `datetime` > ( NOW( ) - INTERVAL 1 DAY )) AND `datetime` > ( NOW( ) - INTERVAL 1 DAY ) LIMIT 1;";

            $stmt = $this->db->prepare($query);
            $stmt->execute();

            $datetime = '';
            $maxTemp = 0;

            $stmt->bind_result($datetime, $maxTemp);
            $stmt->fetch();

            $ret['datetime'] = $datetime;
            $ret['temp'] = $maxTemp;
        }

        return $ret;
    }

    function GetMinTemp($sensorName)
    {
        $ret = array();

        if ($this->db != null)
        {
            $query = "SELECT `datetime`, `$sensorName` FROM `temp` WHERE `$sensorName` = (SELECT MIN(`$sensorName`) FROM temp WHERE `datetime` > ( NOW( ) - INTERVAL 1 DAY )) AND `datetime` > ( NOW( ) - INTERVAL 1 DAY ) LIMIT 1;";

            $stmt = $this->db->prepare($query);
            $stmt->execute();

            $datetime = '';
            $minTemp = 0;

            $stmt->bind_result($datetime, $minTemp);
            $stmt->fetch();

            $ret['datetime'] = $datetime;
            $ret['temp'] = $minTemp;
        }

        return $ret;
    }

    /**
     * データベースへの接続を行います。
     */
    private function ConnectToDb()
    {
        $config = $this->config;

        var_dump($config);
        exit;
    }
}
