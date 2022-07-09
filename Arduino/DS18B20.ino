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
 * POST間隔(ms)
 */
#define POST_INTERVAL 60000

HTTPClient http;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float tempC;
bool getTempStatus;

unsigned long lastPulseTime;
unsigned long pulseInterval;
unsigned long lastUpdateTime;
uint16_t rpm;

const double PWM_Hz = 2000;  // PWM周波数
const uint8_t PWM_level = 8; // PWM分解能 16bit(1～256)
const uint8_t PWM_CH = 1;    // チャンネル

//温度取得用タイマーカウンタ
unsigned long postLast = 0;

void setup()
{
    Serial.begin(115200);
    delay(5000);

    ConnectWiFi();

    FirstGetTemp();

    lastPulseTime = 0;
    pulseInterval = 0;

    pinMode(6, INPUT);
    attachInterrupt(6, SenseRotation, FALLING); // 0 = D2, FALLING = 立ち下がり

    pinMode(8, OUTPUT);

    // チャンネルと周波数の分解能を設定
    ledcSetup(PWM_CH, PWM_Hz, PWM_level);
    // モータのピンとチャンネルの設定
    ledcAttachPin(8, PWM_CH);
}

void loop()
{

    GetTempTimer();
}

void GetFanRpm()
{
    unsigned long cur = millis();
    if (cur - lastUpdateTime > 1000)
    { // 60ms間隔でLEDを更新

        rpm = 60000000 / (pulseInterval * 2); // RPMを求める
        Serial.println(rpm);

        lastUpdateTime = cur;
    }
}

/**
 * 初回取得の処理です。
 */
void FirstGetTemp()
{
    delay(2000);
    GetTemp();

    if (getTempStatus == true)
    {
        PostData();
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
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

/**
 * Wi-Fi状態の確認。切断されていれば再接続を行います。
 */
void CheckConnectWiFi()
{

    if (WiFi.status() != WL_DISCONNECTED)
    {
        delay(500);

        Serial.print("WiFi dicconnected.");

        delay(2000);
        ConnectWiFi();
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
        GetTemp();

        if (getTempStatus == true)
        {
            PostData();
        }

        postLast = postTime;
    }
}

/**
 * 取得した温度を送信します。
 */
void PostData()
{
    String tempCString = "\"temp\": " + String(tempC);
    IPAddress ipAddress = WiFi.localIP();
    String ipAddressString = "\"ipAddress\": \"" + String(ipAddress[0]) + "." + String(ipAddress[1]) + "." +
                             String(ipAddress[2]) + "." + String(ipAddress[3]) + "\"";

    String rpmString = "\"fanRpm\": " + String(rpm);

    String data = "{" + tempCString + ", " + rpmString + ", " + ipAddressString + "}";

    http.begin("http://192.168.11.100/test/aqua/api/");
    http.addHeader("Content-Type", "application/json");
    http.POST(data);
    Serial.println(http.getString());
    http.end();
}

/**
 * DS18B20から温度取得
 */
void GetTemp()
{
    sensors.requestTemperatures();
    tempC = sensors.getTempCByIndex(0);

    if (tempC != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the device 1 (index 0) is: ");
        Serial.println(tempC);
        getTempStatus = true;
    }
    else
    {
        Serial.println("Error: Could not read temperature data");
        getTempStatus = false;
    }
}

// 回転パルス立ち下がりエッジ検出用
void SenseRotation(void)
{
    unsigned long cur = micros();
    unsigned long dif = cur - lastPulseTime;                             // 前回のエッジとの差分
    pulseInterval = (pulseInterval - (pulseInterval >> 2)) + (dif >> 2); // 滑らかに
    lastPulseTime = cur;
}
