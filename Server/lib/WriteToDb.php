<?php

/**
 * 端末からPOSTされた温度情報をデータベースに書き込むクラスです。
 */
class WriteToDb
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
     * @param float $temp 挿入する温度を指定します。
     * @return bool 結果を返します。trueなら成功、falseなら失敗を示します。
     */
    function Insert($temp)
    {
        $result = false;

        if ($this->db != null)
        {
            $datetime = date('Y-m-d H:m:s');
            $query = "INSERT INTO `temp` (`datetime`, `temp`) VALUES ('$datetime', $temp);";

            $stmt = $this->db->prepare($query);
            $result = $stmt->execute();
        }

        return $result;
    }

    /**
     * データベースへの接続を行います。
     */
    private function ConnectToDb()
    {
        $config = $this->config;

        $this->db = new mysqli($config->host, $config->user, $config->password, $config->db);

        if (mysqli_connect_error())
        {
            die("can not connect db.");
        }
    }
}
