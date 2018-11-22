#include "HumiditySensor.h"
#include <ArduinoJson.h>

//#define DEBUG

HumiditySensor::HumiditySensor(Storage *storage, uint8_t sensorPowerPin, uint8_t sensorMeasurePin)
{
    _storage = storage;
    _sensorPowerPin = sensorPowerPin;
    _sensorMeasurePin = sensorMeasurePin;

    pinMode(_sensorPowerPin, OUTPUT);
    digitalWrite(_sensorPowerPin, LOW);
    pinMode(_sensorMeasurePin, INPUT);
}

int HumiditySensor::getMeasure()
{
    Serial.print("Collecting new measure ... ");
    // turn sensor power on only as short as possible to reduce sensor wear (oxidation) to a minimum

    digitalWrite(_sensorPowerPin, HIGH); // turn sensor on
    delay(250);                          // give it a little time to initialize
    int rawValue = analogRead(_sensorMeasurePin);
    digitalWrite(_sensorPowerPin, LOW); // turn sensor off
    adjustMinMax(rawValue);
    int normalized = normalize(rawValue);
    Serial.println(normalized);
    return normalized;
}

void HumiditySensor::resetStoredData()
{
    _minValue1 = DEFAULT_MIN_VALUE;
    _minValue2 = DEFAULT_MIN_VALUE;
    _maxValue1 = DEFAULT_MAX_VALUE;
    _maxValue2 = DEFAULT_MAX_VALUE;
    storeMinMax();
}

String HumiditySensor::getMinMaxSettings()
{
    return _storage->readFile("minmax.json");
}

void HumiditySensor::loadMinMax()
{
    String values = getMinMaxSettings();

    if (values.length() > 0)
    {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject &data = jsonBuffer.parseObject(values);
        _minValue1 = data["min1"].as<int>();
        _minValue2 = data["min2"].as<int>();
        _maxValue1 = data["max1"].as<int>();
        _maxValue2 = data["max2"].as<int>();

#ifdef DEBUG
        Serial.println("loaded minmax data");
#endif
    }
#ifdef DEBUG
    else
        Serial.println("no minmax data available, using defaults");
#endif

    _minValue = _max(_minValue1, _minValue2);
    _maxValue = _min(_maxValue1, _maxValue2);
}

void HumiditySensor::storeMinMax()
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &data = jsonBuffer.createObject();
    data["min1"] = _minValue1;
    data["min2"] = _minValue2;
    data["max1"] = _maxValue1;
    data["max2"] = _maxValue2;

    String newSettings;
    data.prettyPrintTo(newSettings);
    _storage->writeFile("minmax.json", newSettings);

    Serial.println("stored minmax data");
}

void HumiditySensor::adjustMinMax(int newValue)
{
    loadMinMax();

    if (newValue < _minValue)
    {
        _minValue1 = _min(_minValue1, _minValue2); // keep the lowest value ever read
        _minValue2 = newValue;                     // take the new low one

        String log = "adjusted known min values: ";
        log = log + "(" + _minValue1 + ", " + _minValue2 + ")";
        Serial.println(log);

        storeMinMax();
    }

    if (newValue > _maxValue)
    {
        _maxValue1 = _max(_maxValue1, _maxValue2);
        _maxValue2 = newValue;

        String log = "adjusted known max values: ";
        log = log + "(" + _maxValue1 + ", " + _maxValue2 + ")";
        Serial.println(log);

        storeMinMax();
    }
}

int HumiditySensor::normalize(int value)
{
    // normalize
    double relativeValue = ((double)(value - _minValue)) / _max(_maxValue - _minValue, 1);
    // rectify
    relativeValue = _max(0, _min(1, relativeValue));
    // invert (we're reporting humidity, sensor reports resistance == dryness)
    int normalized = 255 - (255.0 * relativeValue);

#ifdef DEBUG
    String debugMessage = "min: ";
    debugMessage = debugMessage + _minValue + "(" + _minValue1 + "," + _minValue2 + ")";
    Serial.println(debugMessage);
    debugMessage = "max: ";
    debugMessage = debugMessage + _maxValue + "(" + _maxValue1 + "," + _maxValue2 + ")";
    Serial.println(debugMessage);
    debugMessage = "current: ";
    debugMessage = debugMessage + value;
    Serial.println(debugMessage);
    debugMessage = "relative: ";
    debugMessage = debugMessage + relativeValue;
    Serial.println(debugMessage);
#endif

    return normalized;
}