# 使用ライブラリ
* OneWire
  https://github.com/PaulStoffregen/OneWire
* DallasTemperature
  https://github.com/milesburton/Arduino-Temperature-Control-Library

# VS Code Arudino プラグイン設定

.vscode/arduino.json の設定は適宜変更してください。

# Wi-Fi設定

```C++
#include <Preferences.h>

Preferences preferences;

void setup() {
  preferences.begin("AP-info", false);            // 名前空間"AP-info"の指定と書き込みモード（false)
  preferences.putString("ssid", "your-ssid");     // ssidの値を指定
  preferences.putString("pass", "your-password"); // passwordの値を指定 
  preferences.end();                              // 処理の終了
}

void loop() {
}
```

## 参考
https://karakuri-musha.com/inside-technology/arduino-m5stickc-02-connect-wifi-for-nossid/

# データベース設定
```JSON
{
    "host": "your database host",
    "db": "your database name",
    "user": "your username",
    "password": "your pasword"
}
```

# tool/reset.py
M5 Stamp C3Uをリセットするスクリプトです。
動作させるにはpyserialが必要です。