#include <Arduino.h>
#include <Utility.h>

void setup()
{
  Serial.begin(921600);
  TelnetStream.begin();
  loadConfigFile();
  setupOTA();
  wifiReconnectSchedule.storedMillis = 60000; // run immediately using "negative" unsigned long
  wifiReconnectSchedule.interval = 60000;
  broadcastSchedule.storedMillis = 0;
  broadcastSchedule.interval = 5000;
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
}

void loop()
{
  wifiReconnet();
  if (WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.handle(); // Always run when WiFi is connected

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
      /*DO SOME OTHER WIFI RELATED STUFF*/
      server.handleClient();
    }
  }

  /*DO SOME NON-WIFI RELATED STUFF*/
}