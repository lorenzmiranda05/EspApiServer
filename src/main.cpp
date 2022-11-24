#include <Arduino.h>
#include <Utility.h>

void setup()
{
  Serial.begin(921600);
  TelnetStream.begin();
  loadConfiguration();
  setupOTA();
  wifiReconnectSchedule.storedMillis = 60000; // run immediately using "negative" unsigned long
  wifiReconnectSchedule.interval = 60000;
  broadcastSchedule.storedMillis = 0;
  broadcastSchedule.interval = 5000;
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, LOW);
  lcd.noBacklight();
  lcd.begin(Columns, Rows, LCD_5x8DOTS, I2CDataPin, I2CClockPin);
  server.on("/", handleRoot);
  server.on("/deviceDetails", HTTP_GET, handleDeviceDetails);
  server.on("/temperature/celcius", HTTP_GET, handleTemperatureCelcius);
  server.on("/temperature/fahrenheit", HTTP_GET, handleTemperatureFahrenheit);
  server.on("/blinkLed", HTTP_POST, handleBlinkLed);
  server.on("/displayTextToLcd", HTTP_POST, handleDisplayTextToLcd);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop()
{
  wifiReconnect();
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
  if (blinkStatus)
  {
    if (blinkSchedule.checkMillis())
    {
      digitalWrite(LedPin, !digitalRead(LedPin));
    }
  }

  if (blinkStatusSchedule.checkMillis())
  {
    if (blinkStatus)
    {
      blinkStatus = false;
    }
  }

  if (lcdDisplayRefreshSchedule.checkMillis())
  {
    lcd.scrollDisplayLeft();
  }

  if (lcdDisplayStatusSchedule.checkMillis())
  {
    lcd.clear();
    lcd.noBacklight();
    lcdDisplayStatus = false;
  }
}