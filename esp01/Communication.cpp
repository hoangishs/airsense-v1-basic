#include "./Communication.h"

void initMqttClient(char* _topic, char* _espID, PubSubClient& _mqttClient)
{
  uint8_t espMacAddress[6];
  WiFi.macAddress(espMacAddress);
  uint32_t macAddressDecimal = (espMacAddress[3] << 16) + (espMacAddress[4] << 8) + espMacAddress[5];
  sprintf(_topic, "/airsense/ESP_%08d/", macAddressDecimal);
  sprintf(_espID, "%08d", macAddressDecimal);
  _mqttClient.setServer(mqttServerAddress, mqttServerPort);
  _mqttClient.connect(_espID);
  DEBUG.println(_topic);
}
