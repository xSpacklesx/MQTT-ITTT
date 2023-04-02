#include <Arduino.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

#include "config.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

MKRIoTCarrier carrier;

int period = 5000;
unsigned long time_now = 0;

void connectWiFi(){
  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Connecting to WiFi...");
    WiFi.begin(SSID, PASSWORD);
    delay(500);
  }

  Serial.println("Connected to WiFi!");
}

void reconnectMQTTClient(){
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");
    if (client.connect(CLIENT_NAME.c_str())){
      Serial.println("connected");
      client.subscribe(CLIENT_TELEMETRY_TOPIC.c_str()); // TO SUBCRIBE A TOPIC
    }
    else{
      Serial.print("Retrying in 5 seconds - failed, rc=");
      Serial.println(client.state());    
      delay(5000);
    }
  }
}

void clientCallback(char *topic, uint8_t *payload, unsigned int length){
  char buff[length + 1];
  for (int i = 0; i < length; i++){
    buff[i] = (char)payload[i];
  }
  
  buff[length] = '\0';

  Serial.print("Message received:");
  Serial.println(buff);
   
  DynamicJsonDocument doc(1024);

  deserializeJson(doc, buff);

  JsonObject obj = doc.as<JsonObject>();

  float tempReceived = obj["Temp"];


  Serial.println("REC: " + String(tempReceived));


  // String message = buff;
   
  // if (message == "TurnON")
  //   digitalWrite(LED_BUILTIN, HIGH);
  // else if (message == "TurnOFF")
  //   digitalWrite(LED_BUILTIN, LOW);

}

void createMQTTClient()
{
    client.setServer(BROKER.c_str(), 1883);
    client.setCallback(clientCallback);
    reconnectMQTTClient();
}


//////////////////////
//Arduino code below//
//////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  while(!Serial){

  }
  carrier.noCase();
  carrier.begin();

  connectWiFi();
  createMQTTClient();
}

void loop() {
  // put your main code here, to run repeatedly:
  reconnectMQTTClient();
  client.loop();
  float temp = carrier.Env.readTemperature();

  DynamicJsonDocument doc(1024);
  doc["Temp"] = temp;

  string telemetry;

  //JsonObject obj = doc.as<JsonObject()>;
  serializeJson(doc, telemetry);


  //String telemetry = String (temp);

  // if(millis() > time_now + period){
  //   //wait 5  
  //   Serial.println(millis());
  //   time_now = millis();
  // }  

  Serial.println("Sending JSON");
  Serial.println(telemetry.c_str());
  client.publish(CLIENT_TELEMETRY_TOPIC.c_str(), telemetry.c_str());
  telemetry.clear();
  delay(period);

}