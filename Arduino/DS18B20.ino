#include <Adafruit_NeoPixel.h>
#include <DallasTemperature.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <Preferences.h>
#include <WiFi.h>

/**
 * 1-Wireのピン
 */
#define ONE_WIRE_BUS 5

/**
 * ファン制御ピン
 */
#define FAN_CONTROL_PIN 4

/**
 * ステータス送信ボタンピン
 */
#define BUTTON_PIN 9

/**
 * ステータスLEDピン
 */
#define LED_PIN 2

/**
 * ファン稼働温度
 */
#define FAN_ON_TEMP 26

/**
 * ファン停止温度
 */
#define FAN_OFF_TEMP 25

/**
 * POST間隔(ms)
 */
#define POST_INTERVAL 60000

/*
const double PWM_Hz = 2000;  // PWM周波数
const uint8_t PWM_level = 8; // PWM分解能 16bit(1～256)
const uint8_t PWM_CH = 1;    // チャンネル
*/

const String deviceId = "\"82d08228a35148b59ad4a11fcefef1d5\"";

HTTPClient http;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_NeoPixel pixels(1, LED_PIN, NEO_GRB + NEO_KHZ800);

bool getTempStatus = false;

//温度取得用タイマーカウンター
unsigned long postLast = 0;

// WiFi接続確認用タイマーカウンター
unsigned long checkConnectWiFiLas = 0;

DeviceAddress deviceAddress1;
DeviceAddress deviceAddress2;

String hexDeviceAddress1;
String hexDeviceAddress2;

bool isFanOn = false;

/**
 * 初期化関数です。
 */
void setup()
{
    //チャンネルと周波数の分解能を設定
    // ledcSetup(PWM_CH, PWM_Hz, PWM_level);
    //モータのピンとチャンネルの設定
    // ledcAttachPin(8, PWM_CH);

    Serial.begin(115200);
    Serial.println("init...");

    pixels.begin();
    sensors.begin();

    pinMode(FAN_CONTROL_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    pixels.setPixelColor(0, pixels.Color(255, 27, 44)); // Colorメソッド内の引数の順番は赤、緑、青
    delay(100);
    pixels.setBrightness(32);
    pixels.show();

    delay(5000);

    GetDeviceAddress();
    ConnectWiFi();
    StatusSendToSerial();

    FirstGetTemp();
}

/**
 * メインループ関数です。
 */
void loop()
{
    CheckConnectWiFi();
    SetStatusLED();
    GetTempTimer();

    int pushButton = digitalRead(BUTTON_PIN);
    if (pushButton == 0)
    {
        StatusSendToSerial();
    }
}

/**
 * 初回取得の処理です。
 */
void FirstGetTemp()
{
    delay(2000);
    float sensor1_temp = GetTemp(0);
    float sensor2_temp = GetTemp(1);

    if (getTempStatus == true)
    {
        IsFanOn(sensor2_temp);

        FanControl();
        PostData(sensor1_temp, sensor2_temp, isFanOn);
    }
    else
    {
        PostError();
    }
}

/**
 * 温度取得のタイマー関数です。
 */
void GetTempTimer()
{
    unsigned long postTime = millis();
    unsigned long diff = postTime - postLast;

    if (diff > POST_INTERVAL)
    {
        float sensor1_temp = GetTemp(0);
        float sensor2_temp = GetTemp(1);

        if (getTempStatus == true)
        {
            IsFanOn(sensor2_temp);

            FanControl();
            PostData(sensor1_temp, sensor2_temp, isFanOn);
        }
        else
        {
            PostError();
        }

        postLast = postTime;
    }
}

/**
 * 取得した温度を送信します。
 */
void PostData(float sensor1_temp, float sensor2_temp, bool isFanOn)
{
    String sensors = "[";
    sensors = sensors + "{\"address\": \"" + hexDeviceAddress1 + "\", \"temp\": " + String(sensor1_temp) + "},";
    sensors = sensors + "{\"address\": \"" + hexDeviceAddress2 + "\", \"temp\": " + String(sensor2_temp) + "}";
    sensors = sensors + "]";

    String sensorString = "\"sensors\": " + sensors;
    IPAddress ipAddress = WiFi.localIP();
    String ipAddressString = "\"ipAddress\": \"" + String(ipAddress[0]) + "." + String(ipAddress[1]) + "." +
                             String(ipAddress[2]) + "." + String(ipAddress[3]) + "\"";

    String isFanOnString = "\"isFanOn\": " + String(isFanOn);

    String deviceIdString = "\"deviceId\": " + deviceId;

    String data = "{" + sensorString + ", " + isFanOnString + ", " + ipAddressString + ", " + deviceIdString + "}";

    Serial.println(data);

    http.begin("http://192.168.11.100/test/aqua/api/");
    http.addHeader("Content-Type", "application/json");
    http.POST(data);
    Serial.println(http.getString());
    http.end();
}

/**
 * エラー情報を送信します。
 */
void PostError()
{
    String sensors = "[";
    sensors = sensors + "{\"address\": \"" + hexDeviceAddress1 + "\"}";
    sensors = sensors + "{\"address\": \"" + hexDeviceAddress2 + "\"}";
    sensors = sensors + "]";

    String data = "{\"message\": \"sensor error\", \"sensors\": " + sensors + "}";

    Serial.println(data);

    http.begin("http://192.168.11.100/test/aqua/api/");
    http.addHeader("Content-Type", "application/json");
    http.POST(data);
    Serial.println(http.getString());
    http.end();
}

/**
 * DS18B20から温度取得
 */
float GetTemp(uint8_t index)
{
    sensors.requestTemperatures();

    float tempC = sensors.getTempCByIndex(index);
    if (tempC != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the device 1 (index " + String(index) + ") is: ");
        Serial.println(tempC);
        getTempStatus = true;
    }
    else
    {
        Serial.println("Error: Could not read temperature data");
        getTempStatus = false;
    }

    return tempC;
}

/**
 * ファンを稼働させるかの判定を行います。
 */
void IsFanOn(float temp)
{
    if (temp >= FAN_ON_TEMP)
    {
        isFanOn = true;
    }
    else if (temp <= FAN_OFF_TEMP)
    {
        isFanOn = false;
    }
}

/**
 * ファンの制御を行います。
 * 12Vファンを5Vで稼働させているので起動失敗防止に255回オンオフ繰り返し
 */
void FanControl()
{
    if (isFanOn == true)
    {
        for (int i = 0; i < 255; i++)
        {
            digitalWrite(FAN_CONTROL_PIN, HIGH);
            delay(10);
            digitalWrite(FAN_CONTROL_PIN, LOW);
        }
        digitalWrite(FAN_CONTROL_PIN, HIGH);
    }
    else
    {
        digitalWrite(FAN_CONTROL_PIN, LOW);
    }
}

/**
 * Wi-Fi接続処理です。
 */
void ConnectWiFi()
{
    Preferences preferences;
    char ssid[33];
    char password[65];

    // Wi-Fiアクセスポイント情報取得
    preferences.begin("AP-info", true);
    preferences.getString("ssid", ssid, sizeof(ssid));
    preferences.getString("pass", password, sizeof(password));
    preferences.end();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Colorメソッド内の引数の順番は赤、緑、青
        pixels.show();
        delay(250);
        Serial.print(".");
        pixels.setPixelColor(0, pixels.Color(0, 0, 0)); // Colorメソッド内の引数の順番は赤、緑、青
        pixels.show();
        delay(250);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();
}

/**
 * Wi-Fi状態の確認。切断されていれば再接続を行います。
 */
void CheckConnectWiFi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("WiFi dicconnected.");

        delay(10000);
        ConnectWiFi();
    }
}

/**
 * 1-Wireにつながっているデバイスアドレスの取得を行います。
 */
void GetDeviceAddress()
{
    Serial.println("device address1:");
    if (sensors.getAddress(deviceAddress1, 0))
    {
        for (int i = 0; i < 8; i++)
        {
            char buf[2];
            sprintf(buf, "%x", deviceAddress1[i]);
            hexDeviceAddress1 = hexDeviceAddress1 + String(buf) + " ";
            Serial.print(deviceAddress1[i], HEX);
        }
        hexDeviceAddress1.trim();

        Serial.println();
    }

    Serial.println("device address2:");
    if (sensors.getAddress(deviceAddress2, 1))
    {
        for (int i = 0; i < 8; i++)
        {
            char buf[2];
            sprintf(buf, "%x", deviceAddress2[i]);
            hexDeviceAddress2 = hexDeviceAddress2 + String(buf) + " ";
            Serial.print(deviceAddress2[i], HEX);
        }
        hexDeviceAddress2.trim();

        Serial.println();
    }
}

/**
 * ステータスLEDの状態を設定します。
 */
void SetStatusLED()
{
    uint8_t g = 0;
    uint8_t b = 0;

    bool isError = false;

    if (isFanOn == true)
    {
        g = 255;
    }
    else
    {
        g = 0;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        b = 255;
    }
    else
    {
        b = 0;
        isError = true;
    }

    if (isError == false)
    {
        pixels.setPixelColor(0, pixels.Color(0, g, b));
    }
    else
    {
        pixels.setPixelColor(0, pixels.Color(255, 27, 44));
    }

    pixels.show();
}

/**
 * 状態をシリアル通信で送信します。
 */
void StatusSendToSerial()
{
    Serial.println("");
    Serial.println("===============================================");
    Serial.println("WiFi Status: ");

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi dicconnected.");
    }
    else
    {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }

    Serial.println("");

    Serial.println("Sensor Status");
    Serial.println("Sensor Count: ");
    Serial.println(sensors.getDS18Count());
    Serial.println("Sensor1 Address: ");
    Serial.println(hexDeviceAddress1);
    Serial.println("Sensor2 Address: ");
    Serial.println(hexDeviceAddress2);

    Serial.println("===============================================");
    Serial.println("");
}