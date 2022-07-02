#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 33

/**
 * POST間隔(ms)
 */
#define POST_INTERVAL = 5000;

Preferences preferences;
char ssid[33];
char password[65];
float tempC = 0;

WiFiServer server(80);
HTTPClient http;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/**
 * Wi-Fi接続
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

void setup()
{
    M5.begin();
    Serial.begin(115200);
    delay(500);

    ConnectWiFi();

    M5.Lcd.setRotation(3);
    M5.Lcd.setTextFont(4);
    M5.lcd.println("IP Address:");
    M5.Lcd.println(WiFi.localIP());

    server.begin();
}

void loop()
{
    GetTemp();
    PostData();
    ShowDisplay();
}

/**
 * ディスプレイ表示
 */
void ShowDisplay()
{
    M5.lcd.setCursor(0, 0);
    M5.lcd.setTextFont(4);
    M5.lcd.println("IP Address:");
    M5.Lcd.println(WiFi.localIP());
    m5.Lcd.print("temp: ");
    M5.lcd.print(tempC);
}

//送信用カウンタ
bool isFirst = true;
unsigned long postLast = 0;
unsigned long postTime = 0;

void PostData()
{
    postTime = millis();
    if (postTime - postLast > 5000 || isFirst == true)
    {
        String data = "{\"temp\": " + String(tempC) + "}";

        http.begin("http://192.168.11.100/test/aqua/write-to-db.php");
        http.addHeader("Content-Type", "application/json");
        http.POST(data);
        Serial.println(http.getString());
        http.end();

        postLast = postTime;
        isFirst = false;
    }
}

/**
 * DS18B20から温度取得
 */
void GetTemp()
{
    // Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    // Serial.println("DONE");
    tempC = sensors.getTempCByIndex(0);

    if (tempC != DEVICE_DISCONNECTED_C)
    {
        // Serial.print("Temperature for the device 1 (index 0) is: ");
        // Serial.println(tempC);
    }
    else
    {
        // Serial.println("Error: Could not read temperature data");
    }
}