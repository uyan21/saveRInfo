#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "EEPROM.h"
#include "src/saveAPInfoManager.h"
void setup() 
{
  Serial.begin(115200);
  confirmEEPROM();
}

void loop() 
{
  wifiCon();
}
