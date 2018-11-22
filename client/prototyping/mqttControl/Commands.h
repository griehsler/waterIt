#pragma once

#include <ArduinoJson.h>

class IncomingCommand
{
public:
  String name;
  virtual void readFrom(JsonObject &json);
  virtual ~IncomingCommand();
};

class OutgoingCommand
{
public:
  virtual void writeTo(JsonObject &json);
};

class DefaultCommand : public IncomingCommand, public OutgoingCommand
{
public:
  void writeTo(JsonObject &json) override;
};
