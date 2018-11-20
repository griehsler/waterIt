#pragma once

#include <Arduino.h>
#include <functional>
#include <Settings.h>

#define BUS_CALLBACK std::function<void(char *)> callback

class Bus
{
public:
  Bus(Settings *settings);
  void loop();

  void send(String topic, String payload);
  void send(String payload);

  void set_callback(BUS_CALLBACK);

private:
  void connect();
  void mqtt_callback(char *topic, byte *payload, unsigned int length);

  Settings *_settings;
  BUS_CALLBACK;
};
