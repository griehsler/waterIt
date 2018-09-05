#include "Network.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <climits>

#define MAX_OUTPUT_VALUE 255; // report humidity level 0..MAX_OUTPUT_VALUE
#define DEFAULT_MIN_VALUE 1024;
#define DEFAULT_MAX_VALUE 1024;
int SENSOR_POWER_PIN = D2;
// #define DEBUG

Storage _storage;
Settings _settings(&_storage);
Network _network(&_settings);

WiFiClient _wifiClient;
PubSubClient _client(_wifiClient);

int minValue = DEFAULT_MIN_VALUE;
int minValue1 = DEFAULT_MIN_VALUE;
int minValue2 = DEFAULT_MIN_VALUE;
int maxValue = DEFAULT_MAX_VALUE;
int maxValue1 = DEFAULT_MAX_VALUE;
int maxValue2 = DEFAULT_MAX_VALUE;

void setup()
{
    Serial.begin(115200);

    pinMode(SENSOR_POWER_PIN, OUTPUT);
    digitalWrite(SENSOR_POWER_PIN, LOW);

    assignNetworkSettings();
    _network.setup();
    _client.setServer("hawking.home", 1883);

    adjustMinMax(getCurrentMeasure());
}

void loop()
{
    int measure = getCurrentMeasure();
    String message = "plantsensor:";
    message += measure;

    if (_client.connected() || _client.connect(createClientId().c_str(), "sensor", "cebewuby"))
    {
        _client.publish("waterIt/sensor", message.c_str());
        Serial.print("message sent: ");
        Serial.println(message);
    }

    delay(5000);
}

void assignNetworkSettings()
{
    _settings.deviceName = "Plant Sensor #1";
    _settings.hostName = "plantsensor";
    _settings.otherAPSSID = "GRIE";
    _settings.otherAPPassword = "HelloKitty";
}

String createClientId()
{
    String clientId = "WaterSensor-";
    clientId += String(random(0xffff), HEX);
    return clientId;
}

int getCurrentMeasure()
{
    // turn sensor power on only as short as possible to reduce sensor wear (oxidation) to a minimum

    digitalWrite(SENSOR_POWER_PIN, HIGH); // turn sensor on
    delay(250); // give it a little time to initialize
    int rawValue = analogRead(A0);
    digitalWrite(SENSOR_POWER_PIN, LOW); // turn sensor off
    adjustMinMax(rawValue);
    return normalize(rawValue);
}

void adjustMinMax(int newValue)
{
    if (newValue < minValue)
    {
        minValue1 = _min(minValue1, minValue2); // keep the lowest value ever read
        minValue2 = newValue;                   // take the new low one
        minValue = _max(minValue1, minValue2);  // use the bigger of the tracked ones for normalization (filtering out one-shot low spike)

        String log = "adjusted known min values: ";
        log = log + "(" + minValue1 + ", " + minValue2 + ")->" + minValue;
        Serial.println(log);
    }

    if (newValue > maxValue)
    {
        maxValue1 = _max(maxValue1, maxValue2);
        maxValue2 = newValue;
        maxValue = _min(maxValue1, maxValue2);

        String log = "adjusted known max values: ";
        log = log + "(" + maxValue1 + ", " + maxValue2 + ")->" + maxValue;
        Serial.println(log);
    }
}

int normalize(int value)
{
    // normalize
    double relativeValue = ((double)(value - minValue)) / _max(maxValue - minValue, 1);
    // rectify
    relativeValue = _max(0, _min(1, relativeValue));
    // invert (we're reporting humidity, sensor reports resistance == dryness)
    int normalized = 255 - (255.0 * relativeValue);

#ifdef DEBUG
    String debugMessage = "min: ";
    debugMessage = debugMessage + minValue + "(" + minValue1 + "," + minValue2 + ")";
    Serial.println(debugMessage);
    debugMessage = "max: ";
    debugMessage = debugMessage + maxValue + "(" + maxValue1 + "," + maxValue2 + ")";
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