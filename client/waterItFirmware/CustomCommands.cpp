#include "CustomCommands.h"

const int subCapacity = JSON_OBJECT_SIZE(7);

WaterLevelMeasureResult::WaterLevelMeasureResult(bool isLow)
{
    this->isLow = isLow;
}

void WaterLevelMeasureResult::writeTo(JsonObject &json)
{
    OutgoingCommand::writeTo(json);
    json["name"] = "waterLevel";
    json["isLow"] = isLow;
}

HumidityLevelMeasureResult::HumidityLevelMeasureResult(int value)
{
    Serial.print("receiving: ");
    Serial.println(value);
    level = value;
}

void HumidityLevelMeasureResult::writeTo(JsonObject &json)
{
    OutgoingCommand::writeTo(json);
    json["name"] = "humidityLevel";
    json["humidity"] = level;
}

HumidityLevelSettingsResult::HumidityLevelSettingsResult(String settings)
{
    this->settings = settings;
}

void HumidityLevelSettingsResult::writeTo(JsonObject &json)
{
    Serial.print("current settings: ");
    Serial.println(settings);

    OutgoingCommand::writeTo(json);
    json["name"] = "humiditySettings";

    JsonObject &subResult = json.createNestedObject("data");
    StaticJsonBuffer<subCapacity> jb;
    JsonObject &settingsJson = jb.parseObject(settings);

    subResult["min1"] = settingsJson["min1"].as<int>();
    subResult["min2"] = settingsJson["min2"].as<int>();
    subResult["max1"] = settingsJson["max1"].as<int>();
    subResult["max2"] = settingsJson["max2"].as<int>();
}

void FeedWaterCommand::readFrom(JsonObject &json)
{
    IncomingCommand::readFrom(json);
    duration = json["duration"].as<int>();
}

void SleepCommand::readFrom(JsonObject &json)
{
    IncomingCommand::readFrom(json);
    duration = json["duration"].as<int>();
}