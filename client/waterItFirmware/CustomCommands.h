#pragma once
#include <Arduino.h>
#include "Commands.h"

class PongCommand : public OutgoingCommand
{
public:
  String name;
  String info;
  void writeTo(JsonObject &json) override;
};

class SendWaterLevelMeasureCommand : public IncomingCommand
{
};

class WaterLevelMeasureResult : public OutgoingCommand
{
public:
  WaterLevelMeasureResult(bool isLow);
  bool isLow;
  void writeTo(JsonObject &json) override;
};

class SendHumidityLevelMeasureCommand : public IncomingCommand
{
};

class HumidityLevelMeasureResult : public OutgoingCommand
{
public:
  HumidityLevelMeasureResult(int level);
  int level;
  void writeTo(JsonObject &json) override;
};

class ResetHumidityLevelSettingsCommand : public IncomingCommand
{
};

class SendHumidityLevelSettingsCommand : public IncomingCommand
{
};

class HumidityLevelSettingsResult : public OutgoingCommand
{
public:
  HumidityLevelSettingsResult(String settings);
  String settings;
  void writeTo(JsonObject &json) override;
};

class FeedWaterCommand : public IncomingCommand
{
public:
  int duration;
  void readFrom(JsonObject &json) override;
};

class SleepCommand : public IncomingCommand
{
public:
  int duration;
  void readFrom(JsonObject &json) override;
};

class UpdateCommand : public IncomingCommand
{
public:
  String fileName;
  void readFrom(JsonObject &json) override;
};