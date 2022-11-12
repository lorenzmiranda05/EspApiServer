#include <Arduino.h>
#include <Utility.h>

void setup()
{
  Serial.begin(921600);
  TelnetStream.begin();
  loadConfigFile();
  setupOTA();
  wifiReconnectSchedule.storedMillis = 0;
  wifiReconnectSchedule.interval = 60000;
  broadcastSchedule.storedMillis = 0;
  broadcastSchedule.interval = 5000;
}

void loop()
{
  wifiReconnet();
  if (WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.handle();
    if (broadcastDeviceDetails == 1)
    {
      if (broadcastSchedule.checkMillis())
      {
        serialAndTelnetPrintln("");
        serialAndTelnetPrint("Device Name: ");
        serialAndTelnetPrintln(espName);
        serialAndTelnetPrint("WiFi Connection: ");
        serialAndTelnetPrintln(WiFi.SSID());
        serialAndTelnetPrint("MAC Address: ");
        serialAndTelnetPrintln(WiFi.macAddress());
        serialAndTelnetPrint("IP Address: ");
        serialAndTelnetPrintln(WiFi.localIP());
      }
    }
    else
    {
      /*DO SOME WIFI RELATED STUFF*/
    }
  }

  /*DO SOME NON-WIFI RELATED STUFF*/
}