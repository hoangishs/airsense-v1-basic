#include "./Device.h"

#include <ESP8266WiFi.h>

uint32_t lastPressButton = 0;

bool longPressButton()
{
  if (millis() - lastPressButton > 3000 && digitalRead(PIN_BUTTON) == 0)
  {
    return true;
  }
  else if (digitalRead(PIN_BUTTON) == 1)
  {
    lastPressButton = millis();
  }
  return false;
}
