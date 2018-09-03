import paho.mqtt.client as mqtt
import datetime
import json

targetfile=""

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    if rc!=0:
        print("Connection failed with result code "+str(rc))
        exit(1)

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("waterIt/sensor/#")

def store_data(sensor, timestamp, value):
    file=open(targetfile, "a")
    file.write("{};{};{}\n".format(sensor, timestamp.strftime("%Y%m%d%H%M%S"), value))
    file.close()

def on_message(client, userdata, msg):
    payload=str(msg.payload)
    payload_parts=payload.split(":", 1)
    c=payload_parts[0]
    ts=datetime.datetime.utcnow()
    v=payload_parts[1]
    store_data(c, ts, v)

with open('config.json') as config_file:
    configuration = json.load(config_file)
targetfile=configuration["targetfile"]

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(configuration["username"], configuration["password"])
client.connect(configuration["host"], configuration["port"])

client.loop_forever()
