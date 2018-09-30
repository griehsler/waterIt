#include <Arduino.h>

int SLEEP_TIME = 1000 * 1000 * 10; // 10 seconds

void setup()
{
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(D6, OUTPUT);

    // Serial.begin(115200);

    digitalWrite(BUILTIN_LED, HIGH);
    digitalWrite(D6, HIGH);
    delay(1000);

    ESP.deepSleep(SLEEP_TIME);
}

void loop()
{
}
