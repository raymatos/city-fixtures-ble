/**
 * 
 * FILE       : configweb.cpp
 * PROJECT    : 
 * AUTHOR     : 
 * 
 */

#include "configweb.h"
#include "src/SerialDebug/SerialDebug.h"
#include "global.h"

#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include "src/SPIFFSEditor.h"
#include <Update.h>
#include <ArduinoJson.h>
#include "peripheral.h"

/*****************************************************************************/
AsyncWebServer server(80);
DNSServer dnsServer;
AsyncWiFiManager wifiManager(&server, &dnsServer);
static uint32_t requestedTimeMs = millis();
const uint32_t NoRequestTimeMs = 300000;
// Flag to use from web update to reboot the ESP
bool shouldReboot = false;
/*****************************************************************************/

void initConfigWebService()
{
    char ap_ssid[64];
    sprintf(ap_ssid, "%s-%s", SOFTAP_SSID_PREFIX, chip_serial);
    wifiManager.startConfigPortalModeless(ap_ssid, SOFTAP_PASSWORD);

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");
    server.serveStatic("/monitor", SPIFFS, "/index.htm");
    server.serveStatic("/upload", SPIFFS, "/upload.htm");

    server.on(
        "/firmware", HTTP_POST, [](AsyncWebServerRequest *request) {
        shouldReboot = !Update.hasError();
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"Firmware has updated successfully, will be rebooted!":"FAIL!");
        response->addHeader("Connection", "close");
        request->send(response);
        ESP.restart(); }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if(!index){
            Serial.printf("Firmware updating started: %s\n", filename.c_str());
            // Update.runAsync(true);
            // if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
            if(!Update.begin()){
                Update.printError(Serial);
            }
        }
        if(!Update.hasError()){
            if(Update.write(data, len) != len){
                Update.printError(Serial);
            }
        }
        if(final){
            if(Update.end(true)){
                Serial.printf("Firmware updating success: %uB\n", index+len);
            } else {
                Update.printError(Serial);
            }
        } });

    server.on(
        "/spiffs", HTTP_POST, [](AsyncWebServerRequest *request) {
        shouldReboot = !Update.hasError();
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "Data has uploaded successfully, will be rebooted!":"FAIL!");
        response->addHeader("Connection", "close");
        request->send(response);
        ESP.restart(); }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if(!index){
            Serial.printf("Data uploading started: %s\n", filename.c_str());
            if(!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)){
                Update.printError(Serial);
            }
        }
        if(!Update.hasError()){
            if(Update.write(data, len) != len){
                Update.printError(Serial);
            }
        }
        if(final){
            if(Update.end(true)){
                Serial.printf("Data uploading success: %uB\n", index+len);
            } else {
                Update.printError(Serial);
            }
        } });

    server.on("/readconf", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<1024> doc;
        // JsonArray segments = doc.createNestedArray("weekOfDays");
        // for (uint8_t idx = 0; idx < sizeof(config.segments) / sizeof(TimeSegment); idx++)
        // {
        //     JsonObject segment = segments.createNestedObject();
        //     segment["wakehour"] = config.segments[idx].wakehour;
        //     segment["wakemin"] = config.segments[idx].wakemin;
        //     segment["wakesec"] = config.segments[idx].wakesec;
        //     segment["sleephour"] = config.segments[idx].sleephour;
        //     segment["sleepmin"] = config.segments[idx].sleepmin;
        //     segment["sleepsec"] = config.segments[idx].sleepsec;
        // }
        char response[1024];
        serializeJson(doc, response);
        request->send(200, "text/plain", response);
    });

    server.on("/mode", HTTP_POST, [](AsyncWebServerRequest *request) {
        int args = request->args();
        for (int i = 0; i < args; i++)
        {
            Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
        }

        AsyncWebParameter *mode = request->getParam("mode", true);
        config.mode = mode->value().toInt();
        saveFSConfig();
        request->redirect("/monitor");
    });

    server.on("/synctime", HTTP_POST, [](AsyncWebServerRequest *request) {
        debugA("Setting RTC time ...\r\n");
        int args = request->args();
        for (int i = 0; i < args; i++)
        {
            Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
        }
        AsyncWebParameter *timeArg = request->getParam("time", true);
        uint32_t timeFromEpoch = timeArg->value().toInt();
        time_t rtc = timeFromEpoch;
        timeval tv = {rtc, 0};
        settimeofday(&tv, nullptr);
        request->send(200, "text/plain", "");
    });

    server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index)
            Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.printf("%s", (const char *)data);
        if (final)
            Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    });

    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (!index)
            Serial.printf("BodyStart: %u\n", total);
        Serial.printf("%s", (const char *)data);
        if (index + len == total)
            Serial.printf("BodyEnd: %u\n", total);
    });

    server.addHandler(new SPIFFSEditor(SPIFFS, HTTP_EDIT_USER, HTTP_EDIT_PASS));
}

void configWebProc()
{
    wifiManager.loop();
}
