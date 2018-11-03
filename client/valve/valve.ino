#include <Arduino.h>

int SLEEP_TIME = 1000 * 1000 * 5; // 10 seconds
int VALVE_PIN = D7;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(VALVE_PIN, OUTPUT);

    // Serial.begin(115200);

    //digitalWrite(BUILTIN_LED, LOW);
    //digitalWrite(VALVE_PIN, HIGH);
    //delay(1000);

    // ESP.deepSleep(SLEEP_TIME);
}

void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(VALVE_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(VALVE_PIN, LOW);
    delay(1000);
}
