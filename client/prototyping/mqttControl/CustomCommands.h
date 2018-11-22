#pragma once

#include "Commands.h"

class SetIntervalCommand : public IncomingCommand
{
  public:
    int interval;
    void readFrom(JsonObject &json) override;
};