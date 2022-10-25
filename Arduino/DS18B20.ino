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

const char *root_ca = "-----BEGIN CERTIFICATE-----\n"
                      "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
                      "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
                      "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
                      "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
                      "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
                      "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
                      "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
                      "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
                      "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
                      "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
                      "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
                      "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
                      "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
                      "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
                      "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
                      "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
                      "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
                      "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
                      "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
                      "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
                      "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
                      "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
                      "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
                      "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
                      "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
                      "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
                      "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
                      "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
                      "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
                      "-----END CERTIFICATE-----";

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
    IPAddress ipAddress = WiFi.localIP();
    String ipAddressString =
        String(ipAddress[0]) + "." + String(ipAddress[1]) + "." + String(ipAddress[2]) + "." + String(ipAddress[3]);

    String roomTemp = "{\n";
    roomTemp = roomTemp + "\"sensorName\":\"Room temp\",\n";
    roomTemp = roomTemp + "\"uuid\":\"b76768294c7611eda9a80cd467fe4d86\",\n";
    roomTemp = roomTemp + "\"values\":[\n";
    roomTemp = roomTemp + "{\n";
    roomTemp = roomTemp + "\"valueName\":\"室温\",\n";
    roomTemp = roomTemp + "\"dataType\":\"float\",\n";
    roomTemp = roomTemp + "\"value\":\"" + String(sensor1_temp) + "\"\n";
    roomTemp = roomTemp + "}\n";
    roomTemp = roomTemp + "]";

    String tankTemp = "{\n";
    tankTemp = tankTemp + "\"sensorName\":\"Tank temp\",\n";
    tankTemp = tankTemp + "\"uuid\":\"91967da94c7611eda9bd57ae55e07111\",\n";
    tankTemp = tankTemp + "\"values\":[\n";
    tankTemp = tankTemp + "{\n";
    tankTemp = tankTemp + "\"valueName\":\"水温\",\n";
    tankTemp = tankTemp + "\"dataType\":\"float\",\n";
    tankTemp = tankTemp + "\"value\":\"" + String(sensor2_temp) + "\"\n";
    tankTemp = tankTemp + "}\n";
    tankTemp = tankTemp + "]";
    ;

    String isFanOnData = "{\n";
    isFanOnData = isFanOnData + "\"sensorName\":\"Is Fan On\",\n";
    isFanOnData = isFanOnData + "\"uuid\":\"79802645520411ed8566cc49f61866be\",\n";
    isFanOnData = isFanOnData + "\"values\":[\n";
    isFanOnData = isFanOnData + "{\n";
    isFanOnData = isFanOnData + "\"valueName\":\"ファン\",\n";
    isFanOnData = isFanOnData + "\"dataType\":\"bool\",\n";
    isFanOnData = isFanOnData + "\"value\":\"" + String(isFanOn) + "\"\n";
    isFanOnData = isFanOnData + "}\n";
    isFanOnData = isFanOnData + "]";

    String ipAddressData = "{\n";
    ipAddressData = ipAddressData + "\"sensorName\":\"IP Addresss\",\n";
    ipAddressData = ipAddressData + "\"uuid\":\"7d80a1c6520411ed86e1aaa0535bd9db\",\n";
    ipAddressData = ipAddressData + "\"values\":[\n";
    ipAddressData = ipAddressData + "{\n";
    ipAddressData = ipAddressData + "\"valueName\":\"IP Address\",\n";
    ipAddressData = ipAddressData + "\"dataType\":\"string\",\n";
    ipAddressData = ipAddressData + "\"value\":\"" + ipAddressString + "\"\n";
    ipAddressData = ipAddressData + "}\n";
    ipAddressData = ipAddressData + "]";

    String sensors = "[\n";
    sensors = sensors + roomTemp + ",\n";
    sensors = sensors + tankTemp + ",\n";
    sensors = sensors + isFanOn + ",\n";
    sensors = sensors + ipAddressData + "]\n";

    SendData(sensors);
}

/**
 * エラー情報を送信します。
 */
void PostError()
{
    IPAddress ipAddress = WiFi.localIP();
    String ipAddressString =
        String(ipAddress[0]) + "." + String(ipAddress[1]) + "." + String(ipAddress[2]) + "." + String(ipAddress[3]);

    String message = "{\n";
    message = message + "\"sensorName\":\"Message\",\n";
    message = message + "\"uuid\":\"e081cc23527411eda30c884471273147\",\n";
    message = message + "\"values\":[\n";
    message = message + "{\n";
    message = message + "\"valueName\":\"メッセージ\",\n";
    message = message + "\"dataType\":\"string\",\n";
    message = message + "\"value\":\"" + ipAddressString + "\"\n";
    message = message + "}\n";
    message = message + "]";

    String sensors = "[" + message + "]\n";

    SendData(sensors);
}

/**
 * データの送信を行います。
 */
void SendData(String sensors)
{
    String json = "{\n";
    json = json + "\"apiKey\":\"381922b9515511edb962d08aa34278bc38347245515511ed85b28275d2ffc971\",\n";
    json = json + "\"deviceUuid\":\"82d08228a35148b59ad4a11fcefef1d5\",\n";
    json = json + "\"sensoers\": " + sensors;
    json = json + "}";

    Serial.println(json);

    http.begin("https://iot-api.sana.moe/api/Data/post", root_ca);
    http.addHeader("Content-Type", "application/json");
    http.POST(json);
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