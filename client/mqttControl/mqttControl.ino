#include <Arduino.h>
#include <Network.h>

#include "Bus.h"
#include "Commands.h"

Storage _storage;
Settings _settings(&_storage);
Network _network(&_settings);

String incomingTopic = "plants/1/toDevice";
String outgoingTopic = "plants/1/fromDevice";

Bus _bus(&_settings);
Commands _commands(&_bus);

void setup()
{
    Serial.begin(115200);

    assignNetworkSettings();
    _network.setup();
    _commands.set_callback(commandReceived);
}

void assignNetworkSettings()
{
    _settings.deviceName = "Plant Backpack #1";
    _settings.hostName = "plant1";
    _settings.otherAPSSID = "GRIE";
    _settings.otherAPPassword = "HelloKitty";

    _settings.mqttEnabled = true;
    _settings.mqttServer = "newton.home";
    _settings.mqttUserName = "device";
    _settings.mqttPassword = "cebewuby";
    _settings.mqttPublishTopic = outgoingTopic;
    _settings.mqttSubscribeTopic = incomingTopic;
}

void loop()
{
    _bus.loop();
}

void commandReceived(Command command)
{
    Serial.println(command.name);

    if (command.name == "ping")
    {
        Command pongCommand;
        pongCommand.name = "pong";
        _commands.send(pongCommand);
    }
}
