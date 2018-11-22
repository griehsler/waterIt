#pragma once

#include "Bus.h"
#include "Commands.h"

#define COMMAND_CREATE_CALLBACK IncomingCommand *(*createCallback)(String)
#define COMMAND_EXECUTE_CALLBACK void (*executeCallback)(IncomingCommand *)

class CommandsController
{
public:
  CommandsController(Bus *bus);
  void setCreateCallback(COMMAND_CREATE_CALLBACK);
  void setExecuteCallback(COMMAND_EXECUTE_CALLBACK);
  void send(OutgoingCommand *command);

private:
  Bus *_bus;
  COMMAND_CREATE_CALLBACK;
  COMMAND_EXECUTE_CALLBACK;

  void read(char *message);
  IncomingCommand *createCommand(String name);
};