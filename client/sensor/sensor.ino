#include "Network.h"
#include <PubSubClient.h>
#include <WiFiClient.h>

// #define DEBUG

Storage _storage;
Settings _settings(&_storage);
Network _network(&_settings);

WiFiClient _wifiClient;
PubSubClient _client(_wifiClient);

void setup()
{
    Serial.begin(115200);

    assignNetworkSettings();
    _network.setup();
    _client.setServer("hawking.home", 1883);
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
    return analogRead(A0);
}