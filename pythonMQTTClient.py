import time
import paho.mqtt.client as mqtt
import datetime
import json

from ifttt_webhook import IftttWebhook


IFTTT_KEY = "bysprj-hq0bggQ0mnF_KRG"
ifttt = IftttWebhook(IFTTT_KEY)

id = 'Spack'

client_telemetry_topic = 'champ/iot/spack'
server_command_topic = client_telemetry_topic
client_name = id + 'led'

mqtt_client = mqtt.Client(client_name)
mqtt_client.connect('broker.hivemq.com') #    test.mosquitto.org

mqtt_client.loop_start()

def handle_telemetry(client, userdata, message):
    payload = message.payload.decode()
    #print(payload)
    recTime = datetime.datetime.now()

    jsonData = make_json(payload)

    print("Message received at: ", recTime)
    print("Message data: " + str(jsonData))
    write_to_file(jsonData, recTime)

    if jsonData["Temp"] > 10:
        ifttt.trigger("temp_over10C", value1 ="temp_over10C")

def write_to_file(data, timeRecived):
    file = open("temperature.txt", "a")
    file.write(str(timeRecived) + "," + str(data["Temp"]) + "\n")
    file.close()

def make_json(dataToDecode):
    #print(type(dataToDecode))
    #print(dataToDecode)
    return json.loads(dataToDecode)

mqtt_client.subscribe(client_telemetry_topic)
mqtt_client.on_message = handle_telemetry

while True:
    time.sleep(1)