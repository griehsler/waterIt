#include <Arduino.h>
#include <Network.h>

#include "Bus.h"
#include "CommandsController.h"
#include "CustomCommands.h"

Storage _storage;
Settings _settings(&_storage);
Network _network(&_settings);

String incomingTopic = "plants/1/toDevice";
String outgoingTopic = "plants/1/fromDevice";

Bus _bus(&_settings);
CommandsController _commandsController(&_bus);

void setup()
{
    Serial.begin(115200);

    assignNetworkSettings();
    _network.setup();
    _commandsController.setCreateCallback(createIncomingCommand);
    _commandsController.setExecuteCallback(commandReceived);
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

IncomingCommand *createIncomingCommand(String name)
{
    if (name == "setInterval")
        return new SetIntervalCommand;
    return nullptr;
}

void commandReceived(IncomingCommand *command)
{
    if (command->name == "ping")
    {
        DefaultCommand pongCommand;
        pongCommand.name = "pong";
        _commandsController.send(&pongCommand);
    }
    else if (command->name == "sendStatus")
    {
        DefaultCommand statusCommand;
        statusCommand.name = "status";
        _commandsController.send(&statusCommand);
    }
    else if (command->name == "setInterval")
    {
        SetIntervalCommand *setInterval = (SetIntervalCommand *)command;
        Serial.print("received setInterval, new interval=");
        Serial.println(setInterval->interval);
    }
}
