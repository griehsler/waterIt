#include "Bus.h"

#include <PubSubClient.h>
#include <WiFiClient.h>

WiFiClient _wifiClient;
PubSubClient _client(_wifiClient);

Bus::Bus(Settings *settings)
{
    _settings = settings;

    _client.setServer(_settings->mqttServer.c_str(), _settings->mqttServerPort);
}

void Bus::loop()
{
    if (!_client.connected())
        connect();
    _client.loop();
}

void Bus::connect()
{
    using namespace std::placeholders;
    _client.setCallback(std::bind(&Bus::mqtt_callback, this, _1, _2, _3));

    while (!_client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        boolean connectionSuccess = false;
        if (_settings->mqttUserName)
            connectionSuccess = _client.connect(clientId.c_str(), _settings->mqttUserName.c_str(), _settings->mqttPassword.c_str());
        else
            connectionSuccess = _client.connect(clientId.c_str());
        if (connectionSuccess)
        {
            Serial.println("connected");

            if (_settings->mqttSubscribeTopic)
            {
                _client.subscribe(_settings->mqttSubscribeTopic.c_str());
                Serial.print("subscribed to: ");
                Serial.println(_settings->mqttSubscribeTopic);
            }
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(_client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void Bus::send(String topic, String payload)
{
    if (!_client.connected())
        connect();

    _client.publish(topic.c_str(), payload.c_str());
    delay(100); // wait a tiny little bit to let publish operations finish
}

void Bus::send(String payload)
{
    send(_settings->mqttPublishTopic, payload);
}

void Bus::mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    char charBuffer[length + 1];
    for (unsigned int i = 0; i < length; i++)
    {
        charBuffer[i] = (char)payload[i];
    }
    charBuffer[length] = '\0';

    if (callback)
    {
        callback(charBuffer);
    }
    else
    {
        Serial.println("Received message:");
        Serial.println(charBuffer);
    }
}

void Bus::set_callback(BUS_CALLBACK)
{
    this->callback = callback;
}