#include <M5StickC.h>
#include <WiFi.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 33

Preferences preferences;
char ssid[33];
char password[65];
float tempC = 0;

WiFiServer server(80);
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
    ServerRecieve();
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
    m5.Lcd.println("temp: ");
    M5.lcd.print(tempC);
}

/**
 * サーバー受信
 */
void ServerRecieve()
{
    WiFiClient client = server.available(); // クライアントからの要求を待つ
    if (client)
    {                                  // if you get a client,
        Serial.println("New Client."); //シリアル・モニタに"New Client."を表示
        String currentLine = "";       // 入力用の文字列をクリア
        while (client.connected())
        { //クライアントと接続中は繰り返す処理
            if (client.available())
            {
                if (c == '\n')
                {
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code
                        // (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's
                        // coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        // 要求を正常に
                        // 受け取ったときの応答
                        client.println("Content-type:text/html");
                        client.println();
                        client.print("temp:");
                        client.print(tempC);
                        client.print(" C");
                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but
                                      // a carriage return character,
                    currentLine += c; // add it to the end of
                                      // the currentLine
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
}

/**
 * DS18B20から温度取得
 */
void GetTemp()
{
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");
    tempC = sensors.getTempCByIndex(0);

    if (tempC != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the device 1 (index 0) is: ");
        Serial.println(tempC);
    }
    else
    {
        Serial.println("Error: Could not read temperature data");
    }
}