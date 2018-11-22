#include <Arduino.h>
#include <Storage.h>

int _waterLevelSensorPin;
int _pumpPin;

void setupGPIO(int pumpPin, int waterLevelSensorPin)
{
    _pumpPin = pumpPin;
    _waterLevelSensorPin = waterLevelSensorPin;

    pinMode(_waterLevelSensorPin, INPUT);
    pinMode(_pumpPin, OUTPUT);
}

bool measureWaterLevel()
{
    int value = digitalRead(_waterLevelSensorPin);
    Serial.print("checked water level: ");
    Serial.println(value);
    return value == 0;
}

void runPump(int duration)
{
    Serial.print("running pump...");
    digitalWrite(_pumpPin, HIGH);
    delay(duration);
    digitalWrite(_pumpPin, LOW);
    Serial.println("done");
}

void deepSleep(uint64_t duration)
{
    Serial.print("going to deep sleep");
    ESP.deepSleep(1000 * duration);
}