#pragma once

#include <Arduino.h>
#include <Settings.h>

#define incomingTopic "toDevice"
#define outgoingTopic "fromDevice"

#define firmwareHost "myhost"
#define firmwareAddress "/waterit/firmware/"

void assignNetworkSettings(Settings *settings)
{
    settings->deviceName = "Device1";
    settings->hostName = "device1";
    settings->otherAPSSID = "MYWIFI";
    settings->otherAPPassword = "MyPassword";

    settings->mqttEnabled = true;
    settings->mqttServer = "mqtthost";
    settings->mqttUserName = "username";
    settings->mqttPassword = "password";
    settings->mqttPublishTopic = outgoingTopic;
    settings->mqttSubscribeTopic = incomingTopic;
}