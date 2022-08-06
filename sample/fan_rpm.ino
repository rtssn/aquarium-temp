unsigned long lastPulseTime;
unsigned long pulseInterval;
unsigned long lastUpdateTime;
uint16_t rpm;

const double PWM_Hz = 2000;  // PWM周波数
const uint8_t PWM_level = 8; // PWM分解能 16bit(1～256)
const uint8_t PWM_CH = 1;    // チャンネル

void setup()
{
    Serial.begin(115200);
    delay(5000);

    lastPulseTime = 0;
    pulseInterval = 0;

    pinMode(6, INPUT);
    attachInterrupt(6, SenseRotation, FALLING); // 0 = D2, FALLING = 立ち下がり

    pinMode(8, OUTPUT);

    //チャンネルと周波数の分解能を設定
    ledcSetup(PWM_CH, PWM_Hz, PWM_level);
    //モータのピンとチャンネルの設定
    ledcAttachPin(8, PWM_CH);
}

void loop()
{
    GetFanRpm();
    ledcWrite(PWM_CH, 50)
}

void GetFanRpm()
{
    unsigned long cur = millis();
    if (cur - lastUpdateTime > 1000)
    {
        rpm = 60000000 / (pulseInterval * 2); // RPMを求める
        Serial.println(rpm);

        lastUpdateTime = cur;
    }
}

// 回転パルス立ち下がりエッジ検出用
void SenseRotation()
{
    unsigned long cur = micros();
    unsigned long dif = cur - lastPulseTime;                             // 前回のエッジとの差分
    pulseInterval = (pulseInterval - (pulseInterval >> 2)) + (dif >> 2); // 滑らかに
    lastPulseTime = cur;
}