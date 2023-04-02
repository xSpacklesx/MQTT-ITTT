#pragma once

#include <string>

using namespace std;

// WiFi credentials
const char * SSID = "ChamplainPSK";
const char * PASSWORD = "letusdare";

// MQTT settings
const string ID = "Spack";
const string BROKER = "broker.hivemq.com";
const string CLIENT_NAME = ID + "remotelight_client";

const string CLIENT_TELEMETRY_TOPIC =  "champ/iot/spack";
