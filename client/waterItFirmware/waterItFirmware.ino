#include <Arduino.h>
#include <Network.h>

#include "Bus.h"
#include "CommandsController.h"
#include "CustomCommands.h"
#include "gpio.h"
#include "HumiditySensor.h"

#include "private.h"

int LEVEL_SENSOR_PIN = D6;
int WATER_FEED_PIN = D7;
int SENSOR_POWER_PIN = D5;
int SENSOR_MEASURE_PIN = A0;

Storage _storage;
Settings _settings(&_storage);
Network _network(&_settings);

Bus _bus(&_settings);
CommandsController _commandsController(&_bus);
HumiditySensor _humiditySensor(&_storage, SENSOR_POWER_PIN, SENSOR_MEASURE_PIN);

void setup()
{
    Serial.begin(115200);

    setupGPIO(WATER_FEED_PIN, LEVEL_SENSOR_PIN);

    assignNetworkSettings(&_settings);
    _network.setup();
    _commandsController.setCreateCallback(createIncomingCommand);
    _commandsController.setExecuteCallback(commandReceived);
}

void loop()
{
    _bus.loop();
}

IncomingCommand *createIncomingCommand(String name)
{
    if (name == "sendWaterLevel")
        return new SendWaterLevelMeasureCommand;
    if (name == "sendHumidityLevel")
        return new SendHumidityLevelMeasureCommand;
    if (name == "sendHumiditySettings")
        return new SendHumidityLevelSettingsCommand;
    if (name == "resetHumiditySettings")
        return new ResetHumidityLevelSettingsCommand;
    if (name == "feedWater")
        return new FeedWaterCommand;
    if (name == "sleep")
        return new SleepCommand;
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
    else if (command->name == "sendWaterLevel")
    {
        bool isLow = measureWaterLevel();
        WaterLevelMeasureResult levelResultCommand(isLow);
        _commandsController.send(&levelResultCommand);
    }
    else if (command->name == "sendHumidityLevel")
    {
        int humidity = _humiditySensor.getMeasure();
        HumidityLevelMeasureResult humidityResultCommand(humidity);
        _commandsController.send(&humidityResultCommand);
    }
    else if (command->name == "sendHumiditySettings")
    {
        String humiditySensorSettings = _humiditySensor.getMinMaxSettings();
        HumidityLevelSettingsResult humiditySettingsCommand(humiditySensorSettings);
        _commandsController.send(&humiditySettingsCommand);
    }
    else if (command->name == "resetHumiditySettings")
    {
        _humiditySensor.resetStoredData();
    }
    else if (command->name == "feedWater")
    {
        FeedWaterCommand *feedCommand = (FeedWaterCommand *)command;
        runPump(feedCommand->duration);
    }
    else if (command->name == "sleep")
    {
        SleepCommand *sleepCommand = (SleepCommand *)command;
        deepSleep(sleepCommand->duration);
    }
    else
    {
        Serial.print("Received unsupported command: ");
        Serial.println(command->name);
    }
}