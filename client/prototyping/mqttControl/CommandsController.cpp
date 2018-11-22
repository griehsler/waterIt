#include "CommandsController.h"

const int capacity = JSON_OBJECT_SIZE(2);

CommandsController::CommandsController(Bus *bus)
{
    _bus = bus;

    using namespace std::placeholders;
    _bus->set_callback(std::bind(&CommandsController::read, this, _1));
}

void CommandsController::setCreateCallback(COMMAND_CREATE_CALLBACK)
{
    this->createCallback = createCallback;
}

void CommandsController::setExecuteCallback(COMMAND_EXECUTE_CALLBACK)
{
    this->executeCallback = executeCallback;
}

void CommandsController::read(char *message)
{
    StaticJsonBuffer<capacity> jb;
    JsonObject &obj = jb.parseObject(message);
    if (obj.success())
    {
        String commandName = obj["name"].as<String>();
        IncomingCommand *newCommand = createCommand(commandName);
        newCommand->readFrom(obj);
        executeCallback(newCommand);
        delete newCommand;
    }
    else
    {
        Serial.println("Received malformed command:");
        Serial.println(message);
    }
}

IncomingCommand *CommandsController::createCommand(String name)
{
    IncomingCommand *newCommand = nullptr;
    if (createCallback)
        newCommand = createCallback(name);

    if (newCommand == nullptr)
        newCommand = new DefaultCommand;

    return newCommand;
}

void CommandsController::send(OutgoingCommand *command)
{
    StaticJsonBuffer<capacity> jb;
    JsonObject &obj = jb.createObject();
    command->writeTo(obj);

    String message = "";
    obj.printTo(message);
    _bus->send(message);
}