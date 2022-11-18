### **ESP8266 API Server**
Will be used as reference for the development of future projects:
* ESP Home Bare-bones
* PID Controlled Sous Vide Machine

###### **Used ESP8266 Lite Template**


---

<br  />

**Bread Board Setup**

1. ESP-01
    <br  />
    <br  />
    ![github](https://raw.githubusercontent.com/lorenzmiranda05/Esp8266LiteTemplate/main/Assets/Image/Schematic_Esp8266LiteTemplate.png)

<br  />

**data>config.json**
1. The config.json file looks like this:
    ```
    {
        "deviceType" : "ESP12F",
        "broadcastDeviceDetails" : 0,
        "ipAddress" : [192, 168, 1, 72],
        "otaPassword" : "12345",
        "accessPoint" : [
            {
                "ssid": "WiFi1",
                "password": "12345"
            },
            {
                "ssid": "WiFi2",
                "password": "12345"
            },
            {
                "ssid": "WiFi3",
                "password": "12345"
            }
        ],
        "api" : {
            "user": "admin",
            "password": "12345"
        }
    }
    ```

<br  />

**Dependency Graph**

|-- ArduinoJson @ 6.19.4
<br  />
|-- TelnetStream @ 1.2.2
<br  />
|&nbsp; &nbsp; &nbsp;|-- ESP8266WiFi @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|-- Ethernet @ 2.0.0
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- SPI @ 1.0
<br  />
|-- Utility
<br  />
|&nbsp; &nbsp; &nbsp;|-- ArduinoJson @ 6.19.4
<br  />
|&nbsp; &nbsp; &nbsp;|-- ArduinoOTA @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- ESP8266WiFi @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- ESP8266mDNS @ 1.2
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- ESP8266WiFi @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|-- ESP8266WebServer @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- ESP8266WiFi @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|-- ESP8266WiFi @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|-- LittleFS @ 0.1.0
<br  />
|&nbsp; &nbsp; &nbsp;|-- TelnetStream @ 1.2.2
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- ESP8266WiFi @ 1.0
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- Ethernet @ 2.0.0
<br  />
|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|&nbsp; &nbsp; &nbsp;|-- SPI @ 1.0
<br  />

<br  />

**Tasks**
* [x] Connect to WiFi using ESP8266WiFiMulti.h
* [x] Use WiFi credentials from File System
* [x] Update embedded program using ArduinoOTA.h
* [x] Print to serial and telnet streams
* [x] Add option to stop broadcasting device details
* [x] Use a static IP
* [x] Load static IP from config.json file
* [x] Use OTA Password
* [x] Load OTA Password from config.json file
* [x] Replace delay function with a non-blocking code