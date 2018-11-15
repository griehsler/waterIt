#pragma once

#include <Arduino.h>
#include <functional>

#define BUS_CALLBACK std::function<void(char *)> callback

class Bus
{
public:
  Bus(String incomingTopic, String defaultOutgoingTopic);
  void loop();

  void send(String topic, String payload);
  void send(String payload);

  void set_callback(BUS_CALLBACK);

private:
  String _incomingTopic;
  String _defaultOutgoingTopic;
  void connect();
  void mqtt_callback(char *topic, byte *payload, unsigned int length);

  BUS_CALLBACK;
};
