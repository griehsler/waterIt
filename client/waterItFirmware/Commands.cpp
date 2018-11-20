#include <Arduino.h>
#include "Commands.h"

IncomingCommand::~IncomingCommand()
{
}

void IncomingCommand::readFrom(JsonObject &json)
{
    name = json["name"].as<String>();
}

void OutgoingCommand::writeTo(JsonObject &json)
{
}

void DefaultCommand::writeTo(JsonObject &json)
{
    json["name"] = name;
}