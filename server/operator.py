import paho.mqtt.client as mqtt
# import datetime
import json

incomingTopic = "plants/1/fromDevice"
outgoingTopic = "plants/1/toDevice"

humidityThreshold = 128
pumpDuration = 2000
sleepDuration = 5000


def on_connect(client, userdata, flags, rc):
    if rc != 0:
        print("Connection failed with result code "+str(rc))
        exit(1)
    client.subscribe(incomingTopic)


def on_message(client, userdata, msg):
    command = json.loads(msg.payload)
    #alert("received: " + str(command))
    name = command["name"]
    if name == "humidityLevel":
        humidity = command["humidity"]
        alert("received humidity measure: {}".format(humidity))
        if humidity < humidityThreshold:
            alert("triggering pump")
            sendCommandAdv("feedWater", "duration", pumpDuration)
            sendCommand("sendWaterLevel")
        else:
            sendCommandAdv("sleep", "duration", sleepDuration)
            sendCommand("sendHumidityLevel", True)
    elif name == "waterLevel":
        isLow = bool(command["isLow"])
        if isLow:
            alert("Water tank is low!")
        sendCommandAdv("sleep", "duration", sleepDuration)
        sendCommand("sendHumidityLevel", True)
    else:
        alert("Unrecognized command:")
        alert(command)


def sendMessage(message, retain):
    #alert("sending: " + message)
    client.publish(outgoingTopic, message, 1, retain)


def sendCommand(name, retain=False):
    sendMessage('{"name":"' + name + '"}', retain)


def sendCommandAdv(name, paramName, paramValue, retain=False):
    message = '"name":"{}","{}":{}'.format(name, paramName, str(paramValue))
    sendMessage('{' + message + '}', retain)


def alert(message):
    print(message)


with open('config.json') as config_file:
    configuration = json.load(config_file)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(configuration["username"], configuration["password"])
client.connect(configuration["host"], configuration["port"])

sendCommand("sendHumidityLevel", True)

client.loop_forever()
