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
        file_put_contents(__DIR__ . '/../log', $result . "\n", FILE_APPEND);
    }

    /**
     * ログの書き込みを行います。
     * @param string $log 書き込むログを指定します。
     */
    static function Write($log)
    {
        file_put_contents(__DIR__ . '/../log', $log, FILE_APPEND);
    }
}
