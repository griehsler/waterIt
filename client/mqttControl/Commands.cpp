#include "Commands.h"
#include <ArduinoJson.h>

const int capacity = JSON_OBJECT_SIZE(2);

Commands::Commands(Bus *bus)
{
    _bus = bus;

    using namespace std::placeholders;
    _bus->set_callback(std::bind(&Commands::read, this, _1));
}

void Commands::set_callback(COMMAND_CALLBACK)
{
    this->callback = callback;
}

void Commands::read(char *message)
{
    StaticJsonBuffer<capacity> jb;
    JsonObject &obj = jb.parseObject(message);
    if (obj.success())
    {
        Command newCommand;
        newCommand.name = obj["name"].as<String>();
        callback(newCommand);
    }
    else
    {
        Serial.println("Received malformed command:");
        Serial.println(message);
    }
}

void Commands::send(Command command)
{
    StaticJsonBuffer<capacity> jb;
    JsonObject &obj = jb.createObject();
    obj["name"] = command.name;

    String message = "";
    obj.printTo(message);
    _bus->send(message);
}
