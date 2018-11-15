#pragma once

#include <Arduino.h>
#include "Bus.h"

struct Command
{
  String name;
};

#define COMMAND_CALLBACK void (*callback)(Command)

class Commands
{
public:
  Commands(Bus *bus);
  void set_callback(COMMAND_CALLBACK);
  void send(Command command);

private:
  Bus *_bus;
  COMMAND_CALLBACK;

  void read(char *message);
};