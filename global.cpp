/**
 * 
 * FILE       : global.cpp
 * PROJECT    : 
 * AUTHOR     : 
 * DESCRITION : 
 *
 */

#include "global.h"
#include "peripheral.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "src/SerialDebug/SerialDebug.h"

// Unique id of ESP32 core
char chip_serial[30];
const int32_t gmtOffset_sec = 3600 * 10;
const int32_t daylightOffset_sec = 0;
struct Config config;

void readChipId()
{
    // Get unique chip id of ESP32 core
    uint8_t bytes[8] = {0};
    uint64_t id = ESP.getEfuseMac();
    uint8_t *p = (uint8_t *)&id;
    for (uint8_t idx = 0; idx < 8; idx++)
        bytes[idx] = p[idx];
    sprintf(chip_serial, "%02x%02x%02x%02x%02x%02x",
            bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
}

void loadFSConfig()
{
    if (SPIFFS.exists("/config.json"))
    {
        // File exists, reading and loading
        File configFile = SPIFFS.open("/config.json", "r");
        debugA("Loading system configuration ...");

        if (configFile)
        {
            //
            DynamicJsonDocument doc(2048);
            // Deserialize the JSON document
            DeserializationError error = deserializeJson(doc, configFile);
            if (error)
                debugE("Failed to read configuration file, using default configuration");

            config.mode = doc["mode"] | FLOOR;
            if (config.mode == FLOOR)
            {
                config.relay_count = 2;
            }
            else
            {
                config.relay_count = doc["relay_count"] | 8;
            }

            const char *identifier = doc["name"] | (config.mode == FLOOR ? DEFAULT_FLOOR_NAME : DEFAULT_ELEVATOR_NAME);
            strcpy(config.identifier, identifier);

            JsonArray relays = doc["relays"].as<JsonArray>();
            uint8_t index = 0;
            for (JsonObject obj : relays)
            {
                const char *description = obj["desc"];
                uint8_t channel = obj["channel"];
                strcpy(config.relays[index].description, description);
                config.relays[index].channel = obj["channel"];
                index++;
            }
            configFile.close();
        }
        else
        {
            debugE("Fatal error found to read configuration");
        }
    }
    else
    {
        debugE("No found config file in SPIFFS, will save default config");

        config.mode = FLOOR;
        config.relay_count = 2;
        strcpy(config.identifier, DEFAULT_FLOOR_NAME);
        for (uint8_t idx = 0; idx < sizeof(config.relays) / sizeof(FloorRelay); idx++)
        {
            strcpy(config.relays[idx].description, "floor");
            config.relays[idx].channel = idx;
        }
        saveFSConfig();
    }
}

void saveFSConfig()
{
    DynamicJsonDocument doc(2048);
    doc["mode"] = config.mode;
    doc["name"] = config.identifier;
    doc["relay_count"] = config.relay_count;

    JsonArray relays = doc.createNestedArray("relays");
    for (uint8_t idx = 0; idx < sizeof(config.relays) / sizeof(FloorRelay); idx++)
    {
        JsonObject relay = relays.createNestedObject();
        relay["desc"] = config.relays[idx].description;
        relay["channel"] = config.relays[idx].channel;
    }

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        debugE("Failed to open config file for writing \r\n");
    }
    else
    {
        debugA("Successfully saved current configuration into SPIFFS ...\r\n");
    }
    serializeJson(doc, configFile);
    configFile.close();
}

String getSplitString(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

bool parseWriteConfig(const char *payload)
{
    bool shouldReboot = false;
    char json[512];
    strcpy(json, payload);
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        debugE("Failed to parse configuration write payload");
        return false;
    }
    if (doc.containsKey("mode"))
    {
        config.mode = doc["mode"];
        debugA("Mode changed to %s", config.mode ? "Elevator" : "Floor");
        if (config.mode == FLOOR)
        {
            config.relay_count = 2;
        }
        shouldReboot = true;
    }

    if (doc.containsKey("name"))
    {
<<<<<<< HEAD
        strcpy(config.identifier, doc["name"]);
=======
       strcpy(config.identifier, doc["name"]);
>>>>>>> 5ce1c1211c3f08df74bad423e04acc268c63fcba
        debugA("BLE device name has changed to %s", config.identifier);
        shouldReboot = true;
    }

    if (doc.containsKey("relay_count"))
    {
        config.relay_count = doc["relay_count"];
        shouldReboot = true;
    }

    if (doc.containsKey("relays"))
    {
        JsonArray relays = doc["relays"].as<JsonArray>();
        for (JsonObject obj : relays)
        {
            const char *description = obj["desc"];
            uint8_t channel = obj["channel"];
            strcpy(config.relays[channel].description, description);
        }
    }

    saveFSConfig();
    return shouldReboot;
}

void parseForceRelay(const char *payload)
{
    char json[256];
    strcpy(json, payload);
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        debugE("Failed to parse forcerelay write payload");
        return;
    }

    JsonArray relays = doc.to<JsonArray>();
    for (JsonObject obj : relays)
    {
        const char *description = obj["desc"];
        const char *command = obj["command"];
        String command_str = String(command);
        uint8_t channel = obj["channel"];
        forceRelay(channel, command_str == "on");
    }
}
