#include <Arduino.h>

int LEVEL_SENSOR_PIN = D6;

void setup()
{
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(LEVEL_SENSOR_PIN, INPUT);

    Serial.begin(115200);

    digitalWrite(BUILTIN_LED, LOW);
}

void loop()
{
    int value = digitalRead(LEVEL_SENSOR_PIN);
    Serial.println(value);
    digitalWrite(BUILTIN_LED, value);
}