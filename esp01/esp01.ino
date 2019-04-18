#include "./Communication.h"
#include "./Device.h"

uint8_t dataBuffer[PACKET_SIZE] = {0};
uint8_t dataByteCount = 0;

bool isGetTime = true;
bool isPublish = false;

uint32_t lastGetTime = 0;
uint32_t lastGetData = 0;
uint32_t lastMqttReconnect = 0;

char topic[25];
char espID[10];

WiFiClient espClient;
PubSubClient mqttClient(espClient);

NTPtime NTPch("ch.pool.ntp.org");///???
strDateTime dateTime;

void setup()
{
  DEBUG.begin(9600);
  DEBUG.setDebugOutput(true);
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT);
  WiFi.mode(WIFI_STA);
  initMqttClient(topic, espID, mqttClient);
  DEBUG.println(" - setup done");
}

void loop()
{
  if (longPressButton())
  {
    DEBUG.println(" - long press!");
    if (WiFi.beginSmartConfig())
    {
      DEBUG.println(" - start config wifi");
    }
  }
  if (Serial.available() > 0)
  {
    dataBuffer[dataByteCount] = Serial.read();
    if (dataBuffer[0] == 66)
      dataByteCount++;
    if (dataByteCount == PACKET_SIZE)
    {
      dataByteCount = 0;
      if (dataBuffer[1] == 77)
      {
        uint16_t check = 0;
        for (uint8_t i = 0; i < PACKET_SIZE - 2; i++)
        {
          check += dataBuffer[i];
        }
        uint16_t check2 = dataBuffer[PACKET_SIZE - 2] * 256 + dataBuffer[PACKET_SIZE - 1];
        if (check == check2)
        {
          lastGetData = millis();
          isPublish = true;
        }
      }
    }
  }
  else if (WiFi.status() == WL_CONNECTED)
  {
    if (isGetTime)
    {
      dateTime = NTPch.getNTPtime(7.0, 0);
      if (dateTime.valid)
      {
        //if got time
        lastGetTime = millis();
        isGetTime = false;
      }
    }
    else if (isPublish)
    {
      //if queue is not empty, publish data to server
      if (mqttClient.connected())
      {
        DEBUG.print(" - publish:.. ");
        uint32_t epochTime = 0;
        if (lastGetData > lastGetTime)
          epochTime = dateTime.epochTime + ((lastGetData - lastGetTime) / 1000);
        else
          epochTime = dateTime.epochTime - ((lastGetTime - lastGetData) / 1000);
        char mes[256] = {0};
        sprintf(mes, "{\"data\":{\"tem\":\"%d\",\"humi\":\"%d\",\"pm1\":\"%d\",\"pm2p5\":\"%d\",\"pm10\":\"%d\",\"time\":\"%d\"}}", dataBuffer[2], dataBuffer[3], ((dataBuffer[4] << 8) + dataBuffer[5]), ((dataBuffer[6] << 8) + dataBuffer[7]), ((dataBuffer[8] << 8) + dataBuffer[9]), epochTime);
        if (mqttClient.publish(topic, mes, true))
        {
          DEBUG.println(mes);
          isPublish = false;
        }
        mqttClient.loop();
      }
      else if (millis() - lastMqttReconnect > 1000)
      {
        lastMqttReconnect = millis();
        DEBUG.println(" - mqtt reconnect ");
        mqttClient.connect(espID);
      }
    }
  }
}
