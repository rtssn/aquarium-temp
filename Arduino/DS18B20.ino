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

#define FAN_CONTROL_PIN 6

HTTPClient http;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

bool getTempStatus = false;

//温度取得用タイマーカウンタ
unsigned long postLast = 0;

void setup()
{
    Serial.begin(115200);
    delay(5000);

    ConnectWiFi();

    FirstGetTemp();

    pinMode(FAN_CONTROL_PIN, OUTPUT);
}

void loop()
{
    GetTempTimer();
    CheckConnectWiFi();
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
        bool isFanOn = IsFanOn(sensor2_temp);

        FanControl(isFanOn);
        PostData(sensor1_temp, sensor2_temp, isFanOn);
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
            bool isFanOn = IsFanOn(sensor2_temp);

            FanControl(isFanOn);
            PostData(sensor1_temp, sensor2_temp, isFanOn);
        }

        postLast = postTime;
    }
}

bool IsFanOn(float temp)
{
    bool ret = false;

    if (temp >= 26)
    {
        ret = true;
    }

    return ret;
}

/**
 * 取得した温度を送信します。
 */
void PostData(float sensor1_temp, float sensor2_temp, bool isFanOn)
{
    String sensors = "[";
    sensors = sensors + "{\"temp\": " + String(sensor1_temp) + "},";
    sensors = sensors + "{\"temp\": " + String(sensor2_temp) + "}";
    sensors = sensors + "]";

    String sensorString = "\"sensors\": " + sensors;
    IPAddress ipAddress = WiFi.localIP();
    String ipAddressString = "\"ipAddress\": \"" + String(ipAddress[0]) + "." + String(ipAddress[1]) + "." +
                             String(ipAddress[2]) + "." + String(ipAddress[3]) + "\"";

    String isFanOnString = "\"isFanOn\": " + String(isFanOn);

    String data = "{" + sensorString + ", " + isFanOnString + ", " + ipAddressString + "}";

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
 * ファンの制御を行います。
 */
void FanControl(bool isFanOn)
{
    if (isFanOn == true)
    {
        for (int i = 0 i < 10; i++)
        {
            digitalWrite(FAN_CONTROL_PIN, HIGH);
            delay(100);
            digitalWrite(FAN_CONTROL_PIN, LOW);
            delay(100);
            digitalWrite(FAN_CONTROL_PIN, HIGH);
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
    if (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.print("WiFi dicconnected.");

        delay(10000);
        ConnectWiFi();
    }
}