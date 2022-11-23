#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define JsonConfigFile "/config.json"
#define OneWirePin 10
#define LedPin D2

ESP8266WiFiMulti WiFiMulti;
char espName[15];
int broadcastDeviceDetails = 1;
ESP8266WebServer server(80);
char apiUser[20];
char apiPassword[20];

class Schedule
{
public:
    unsigned long storedMillis;
    unsigned long interval;
    boolean checkMillis()
    {
        unsigned long millisNow = millis();
        // handled unsigned long overflow scenario
        if (millisNow - storedMillis >= interval)
        {
            storedMillis = millisNow;
            return true;
        }
        return false;
    }
};

Schedule wifiReconnectSchedule;
Schedule broadcastSchedule;

OneWire oneWire(OneWirePin);
// Pass oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
float previousTempC = 0;
float previousTempF = 0;
bool ledPinStatus = LOW;

bool blinkStatus = false;
Schedule blinkSchedule;
Schedule blinkStatusSchedule;

void serialAndTelnetPrint(__FlashStringHelper *message)
{
    Serial.print(message);
    TelnetStream.print(message);
}
void serialAndTelnetPrint(const char *message)
{
    Serial.print(message);
    TelnetStream.print(message);
}
void serialAndTelnetPrint(int message)
{
    Serial.print(message);
    TelnetStream.print(message);
}
void serialAndTelnetPrint(float message)
{
    Serial.print(message);
    TelnetStream.print(message);
}
void serialAndTelnetPrint(unsigned long message)
{
    Serial.print(message);
    TelnetStream.print(message);
}
void serialAndTelnetPrint(IPAddress message)
{
    Serial.print(message);
    TelnetStream.print(message);
}
void serialAndTelnetPrint(String message)
{
    Serial.print(message);
    TelnetStream.print(message);
}

void serialAndTelnetPrintln(__FlashStringHelper *message)
{
    Serial.println(message);
    TelnetStream.println(message);
}
void serialAndTelnetPrintln(const char *message)
{
    Serial.println(message);
    TelnetStream.println(message);
}
void serialAndTelnetPrintln(int message)
{
    Serial.println(message);
    TelnetStream.println(message);
}
void serialAndTelnetPrintln(float message)
{
    Serial.println(message);
    TelnetStream.println(message);
}
void serialAndTelnetPrintln(unsigned long message)
{
    Serial.println(message);
    TelnetStream.println(message);
}
void serialAndTelnetPrintln(IPAddress message)
{
    Serial.println(message);
    TelnetStream.println(message);
}
void serialAndTelnetPrintln(String message)
{
    Serial.println(message);
    TelnetStream.println(message);
}

void loadConfiguration()
{
    // Read configuration from FS json
    serialAndTelnetPrintln(F("Mounting FS"));

    // May need to make it begin(true) first time you are using SPIFFS
    if (LittleFS.begin())
    {
        // The file exists, reading and loading
        serialAndTelnetPrintln(F("Reading config"));
        File configFile = LittleFS.open(JsonConfigFile, "r");
        if (configFile)
        {
            serialAndTelnetPrintln(F("Opened config"));
            /*
            Size increase in config.json file or other files in the file system may
            fail json deserialization. Use https://arduinojson.org/v6/assistant/ to
            check json document size. Choose StaticJsonDocument for small documents
            (below 1KB) and switch to a DynamicJsonDocument if it’s too large to fit
            in the stack memory. Source: https://arduinojson.org/v6/api/staticjsondocument/
            */
            StaticJsonDocument<768> json;
            DeserializationError error = deserializeJson(json, configFile);
            if (!error)
            {
                serialAndTelnetPrintln(F("Parsing config"));
                strcpy(espName, json["deviceType"]);
                broadcastDeviceDetails = json["broadcastDeviceDetails"].as<int>();
                WiFiMulti.addAP(json["accessPoint"][0]["ssid"], json["accessPoint"][0]["password"]);
                WiFiMulti.addAP(json["accessPoint"][1]["ssid"], json["accessPoint"][1]["password"]);
                WiFiMulti.addAP(json["accessPoint"][2]["ssid"], json["accessPoint"][2]["password"]);
                IPAddress gateway(192, 168, 1, 1);
                IPAddress subnet(255, 255, 0, 0);
                IPAddress local_IP(json["ipAddress"][0].as<int>(), json["ipAddress"][1].as<int>(), json["ipAddress"][2].as<int>(), json["ipAddress"][3].as<int>());
                WiFi.config(local_IP, gateway, subnet);
                ArduinoOTA.setPassword(json["otaPassword"]);
                strcpy(apiUser, json["api"]["user"]);
                strcpy(apiPassword, json["api"]["password"]);
            }
            else
            {
                serialAndTelnetPrintln(F("Load config failed"));
            }
        }
    }
    else
    {
        serialAndTelnetPrintln(F("Mount FS failed"));
    }
}

void setupOTA()
{
    // Create unique ota host name by appending MAC Address
    uint16_t maxlen = strlen(espName) + 8;
    char *deviceName = new char[maxlen];
    uint8_t mac[6];
    WiFi.macAddress(mac);
    snprintf(deviceName, maxlen, "%s-%02x%02x%02x", espName, mac[3], mac[4], mac[5]);
    strcpy(espName, deviceName);
    ArduinoOTA.setHostname(deviceName);
    delete[] deviceName;

    ArduinoOTA.onStart([]()
                       {
                            serialAndTelnetPrint(F(""));
                            serialAndTelnetPrintln(F("Update start")); });

    ArduinoOTA.onEnd([]()
                     { serialAndTelnetPrintln(F("\nEnd")); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });

    ArduinoOTA.onError([](ota_error_t error)
                       {
                            Serial.printf("Error[%u]: ", error);
                            if (error == OTA_AUTH_ERROR) serialAndTelnetPrintln(F("\nAuth Failed"));
                            else if (error == OTA_BEGIN_ERROR) serialAndTelnetPrintln(F("\nBegin Failed"));
                            else if (error == OTA_CONNECT_ERROR) serialAndTelnetPrintln(F("\nConnect Failed"));
                            else if (error == OTA_RECEIVE_ERROR) serialAndTelnetPrintln(F("\nReceive Failed"));
                            else if (error == OTA_END_ERROR) serialAndTelnetPrintln(F("\nEnd Failed")); });

    ArduinoOTA.begin();
    serialAndTelnetPrintln(F("ESPOTA READY"));
}

void wifiReconnect()
{
    if ((WiFi.status() != WL_CONNECTED) && wifiReconnectSchedule.checkMillis())
    {
        WiFiMulti.run();
    }
}

float sensorDataCelsius()
{
    // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

    // Suppress no reading (-127)
    if (tempC == -127)
    {
        tempC = previousTempC;
    }
    else
    {
        previousTempC = tempC;
    }
    return tempC;
}

float sensorDataFahrenheit()
{
    // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
    sensors.requestTemperatures();
    float tempF = sensors.getTempFByIndex(0);

    // Suppress no reading (-196)
    if (tempF == -196)
    {
        tempF = previousTempF;
    }
    else
    {
        previousTempF = tempF;
    }
    return tempF;
}

void apiAuthentication()
{
    if (!server.authenticate(apiUser, apiPassword))
    {
        return server.requestAuthentication();
    }
}

void handleRoot()
{
    apiAuthentication();
    String output;
    StaticJsonDocument<64> doc;
    doc["message"] = "Welcome to the ESP8266 API Server";
    serializeJson(doc, output);
    server.send(200, "application/json", output);
}

void handleDeviceDetails()
{
    apiAuthentication();
    String output;
    StaticJsonDocument<192> doc;
    doc["deviceName"] = espName;
    doc["wifiConnection"] = WiFi.SSID();
    doc["macAddress"] = WiFi.macAddress();
    doc["ipAddress"] = WiFi.localIP();
    serializeJson(doc, output);
    server.send(200, "application/json", output);
}

void handleTemperatureCelcius()
{
    apiAuthentication();
    String output;
    StaticJsonDocument<16> doc;
    doc["temperature"] = sensorDataCelsius();
    serializeJson(doc, output);
    server.send(200, "application/json", output);
}

void handleTemperatureFahrenheit()
{
    apiAuthentication();
    String output;
    StaticJsonDocument<16> doc;
    doc["temperature"] = sensorDataFahrenheit();
    serializeJson(doc, output);
    server.send(200, "application/json", output);
}

void handleBlinkLed()
{
    apiAuthentication();
    String output;
    if (server.method() != HTTP_POST)
    {
        StaticJsonDocument<48> doc;
        doc["message"] = "Invalid HTTP method";
        serializeJson(doc, output);
        return server.send(400, "application/json", output);
    }

    String postBody = server.arg("plain");
    StaticJsonDocument<96> doc;
    DeserializationError error = deserializeJson(doc, postBody);
    if (error)
    {
        StaticJsonDocument<48> doc;
        doc["message"] = "Invalid JSON";
        serializeJson(doc, output);
        return server.send(400, "application/json", output);
    }
    else
    {
        if (!blinkStatus)
        {
            blinkStatus = true;
            blinkStatusSchedule.interval = doc["durationInMilliseconds"];
            blinkStatusSchedule.storedMillis = millis();
            blinkSchedule.interval = doc["intervalInMilliseconds"];
            blinkSchedule.storedMillis = doc["intervalInMilliseconds"];

            StaticJsonDocument<48> doc;
            doc["message"] = "Blink started";
            serializeJson(doc, output);
            return server.send(200, "application/json", output);
        }
        else
        {
            StaticJsonDocument<48> doc;
            doc["message"] = "Blink in-progress";
            serializeJson(doc, output);
            return server.send(200, "application/json", output);
        }
    }
}

void handleNotFound()
{
    apiAuthentication();
    String output;
    StaticJsonDocument<48> doc;
    doc["message"] = "Endpoint not found";
    serializeJson(doc, output);
    server.send(404, "application/json", output);
}