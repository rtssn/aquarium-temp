#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/**
 * 1-Wireのピン
 */
#define ONE_WIRE_BUS 33

/**
 * POST間隔(ms)
 */
#define POST_INTERVAL 60000

Preferences preferences;
char ssid[33];
char password[65];
float tempC = 0;
bool getTempStatus = false;

HTTPClient http;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
{
    M5.begin();
    Serial.begin(115200);
    delay(500);

    ConnectWiFi();

    M5.Axp.ScreenBreath(8);
    M5.Lcd.setRotation(3);

    FirstGetTemp();
}

void loop()
{
    GetTemp();

    if (getTempStatus == true)
    {
        PostDataTimer();
    }

    ShowDisplay();

    CheckConnectWiFi();
}

/**
 * 初回取得の処理です。
 */
void FirstGetTemp()
{
    delay(2000);
    GetTemp();
    ShowDisplay();

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
 * ディスプレイ表示処理です。
 */
void ShowDisplay()
{
    M5.lcd.setCursor(0, 0);
    M5.lcd.setTextFont(2);
    M5.lcd.println("IP Address:");
    M5.Lcd.println(WiFi.localIP());
    M5.lcd.print("RSSI: ");
    M5.Lcd.println(WiFi.RSSI());
    M5.lcd.setTextFont(4);
    m5.Lcd.print("temp: ");
    M5.lcd.print(tempC);
}

//送信用カウンタ
unsigned long postLast = 0;

/**
 * 温度データ送信のタイマー関数です。
 */
void PostDataTimer()
{
    unsigned long postTime = millis();
    unsigned long diff = postTime - postLast;

    if (diff > POST_INTERVAL)
    {
        PostData();

        postLast = postTime;
    }
}

/**
 * 取得した温度を送信します。
 */
void PostData()
{
    String data = "{\"temp\": " + String(tempC) + "}";

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