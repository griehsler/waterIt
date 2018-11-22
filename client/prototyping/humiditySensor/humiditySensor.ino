#include <climits>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <Network.h>

#define MAX_OUTPUT_VALUE 255; // report humidity level 0..MAX_OUTPUT_VALUE
#define DEFAULT_MIN_VALUE 1024;
#define DEFAULT_MAX_VALUE 1024;
int SLEEP_TIME = 1000 * 1000 * 60 * 15; // 15 minutes
int SENSOR_POWER_PIN = D5;
int SENSOR_MEASURE_PIN = A0;

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

    measureAndSend();
    ESP.deepSleep(SLEEP_TIME);
}

void loop()
{
}

void measureAndSend()
{
    int measure = getCurrentMeasure();
    String message = "plantsensor:";
    message += measure;

    if (_client.connected() || _client.connect(createClientId().c_str(), "sensor", "cebewuby"))
    {
        _client.publish("waterIt/sensor", message.c_str());
        delay(100);
        _client.disconnect();
        Serial.print("message sent: ");
        Serial.println(message);
    }
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
    delay(250);                           // give it a little time to initialize
    int rawValue = analogRead(SENSOR_MEASURE_PIN);
    digitalWrite(SENSOR_POWER_PIN, LOW); // turn sensor off
    adjustMinMax(rawValue);
    return normalize(rawValue);
}

void adjustMinMax(int newValue)
{
    loadMinMax();

    if (newValue < minValue)
    {
        minValue1 = _min(minValue1, minValue2); // keep the lowest value ever read
        minValue2 = newValue;                   // take the new low one

        String log = "adjusted known min values: ";
        log = log + "(" + minValue1 + ", " + minValue2 + ")";
        Serial.println(log);

        storeMinMax();
    }

    if (newValue > maxValue)
    {
        maxValue1 = _max(maxValue1, maxValue2);
        maxValue2 = newValue;

        String log = "adjusted known max values: ";
        log = log + "(" + maxValue1 + ", " + maxValue2 + ")";
        Serial.println(log);

        storeMinMax();
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

void loadMinMax()
{
    String values = _storage.readFile("minmax.json");

    if (values.length() > 0)
    {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject &data = jsonBuffer.parseObject(values);
        minValue1 = data["min1"].as<int>();
        minValue2 = data["min2"].as<int>();
        maxValue1 = data["max1"].as<int>();
        maxValue2 = data["max2"].as<int>();

#ifdef DEBUG
        Serial.println("loaded minmax data");
#endif
    }
#ifdef DEBUG
    else
        Serial.println("no minmax data available, using defaults");
#endif

    minValue = _max(minValue1, minValue2);
    maxValue = _min(maxValue1, maxValue2);
}

void storeMinMax()
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &data = jsonBuffer.createObject();
    data["min1"] = minValue1;
    data["min2"] = minValue2;
    data["max1"] = maxValue1;
    data["max2"] = maxValue2;

    String newSettings;
    data.prettyPrintTo(newSettings);
    _storage.writeFile("minmax.json", newSettings);

    Serial.println("stored minmax data");
}