import paho.mqtt.client as mqtt
from datetime import datetime
import pytz
import json
from pony.orm import *
import sys

with open(sys.argv[1]) as config_file:
    configuration = json.load(config_file)

incomingTopic = configuration["incomingTopic"]
outgoingTopic = configuration["outgoingTopic"]

humidityThreshold = configuration["humidityThreshold"]
pumpDuration = configuraion["pumpSeconds"] * 1000
sleepDuration = configuration["sleepMinutes"] * 60000

db = Database()


class HumidityMeasure(db.Entity):
    timestamp = Required(datetime)
    value = Required(int)


class Event(db.Entity):
    timestamp = Required(datetime)
    kind = Required(str)


db.bind(provider='sqlite', filename=configuration["database"], create_db=True)
db.generate_mapping(create_tables=True)


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
        now = datetime.now(pytz.utc)
        with db_session:
            HumidityMeasure(value=humidity,
                            timestamp=now)
            alert("received humidity measure: {}".format(humidity))
            if humidity < humidityThreshold:
                alert("triggering pump")
                sendCommandAdv("feedWater", "duration", pumpDuration)
                Event(kind="pump", timestamp=now)
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


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(configuration["username"], configuration["password"])
client.connect(configuration["host"], configuration["port"])

sendCommand("sendHumidityLevel", True)

client.loop_forever()
