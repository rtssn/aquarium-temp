<?php

class Log
{
    /**
     * var_dump()を利用したログの書き込みを行います。
     * @param mixed $var ログに書き込むオブジェクトを指定します。
     */
    static function VarDumpLogWrite($var)
    {
        ob_start();
        var_dump($var);
        $result = ob_get_clean();

        $datetime = date('Y-m-d H:i:s');
        $log = "[$datetime]:\n$result\n";

        file_put_contents(__DIR__ . '/../log', $log, FILE_APPEND);
    }

    /**
     * ログの書き込みを行います。
     * @param string $log 書き込むログを指定します。
     */
    static function Write($text)
    {
        $datetime = date('Y-m-d H:i:s');
        $log = "[$datetime]: $text\n";
        file_put_contents(__DIR__ . '/../log', $log, FILE_APPEND);
    }
}
