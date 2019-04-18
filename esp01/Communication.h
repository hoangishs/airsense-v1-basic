#ifndef Communication_h
#define Communication_h

#include "./StructDefine.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPtimeESP.h>
#include <SoftwareSerial.h>

const char mqttServerAddress[15] = "188.68.48.86";
const uint16_t mqttServerPort = 1883;

void initMqttClient(char* _topic, char* _espID, PubSubClient& _mqttClient);

#endif
