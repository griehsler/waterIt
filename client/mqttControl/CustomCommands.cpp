#include "CustomCommands.h"

void SetIntervalCommand::readFrom(JsonObject &json)
{
    IncomingCommand::readFrom(json);
    interval = json["interval"].as<int>();
}