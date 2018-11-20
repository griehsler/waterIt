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
    return value == 0;
}

void runPump(int duration)
{
    digitalWrite(_pumpPin, HIGH);
    delay(duration);
    digitalWrite(_pumpPin, LOW);
}

void deepSleep(uint64_t duration)
{
    Serial.print("going to deep sleep");
    ESP.deepSleep(1000 * duration);
}