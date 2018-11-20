#pragma once

#include "Storage.h"

#define MAX_OUTPUT_VALUE 255; // report humidity level 0..MAX_OUTPUT_VALUE
#define DEFAULT_MIN_VALUE 1024;
#define DEFAULT_MAX_VALUE 1024;

class HumiditySensor
{
  public:
    HumiditySensor(Storage *storage, uint8_t sensorPowerPin, uint8_t sensorMeasurePin);
    int getMeasure();
    void resetStoredData();
    String getMinMaxSettings();

  private:
    Storage *_storage;
    uint8_t _sensorPowerPin;
    uint8_t _sensorMeasurePin;

    int _minValue = DEFAULT_MIN_VALUE;
    int _minValue1 = DEFAULT_MIN_VALUE;
    int _minValue2 = DEFAULT_MIN_VALUE;
    int _maxValue = DEFAULT_MAX_VALUE;
    int _maxValue1 = DEFAULT_MAX_VALUE;
    int _maxValue2 = DEFAULT_MAX_VALUE;

    void loadMinMax();
    void storeMinMax();
    int normalize(int value);
    void adjustMinMax(int newValue);
};