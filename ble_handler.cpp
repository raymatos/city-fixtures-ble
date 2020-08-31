/**
 * 
 * FILE       : ble_handler.cpp
 * PROJECT    : 
 * AUTHOR     : 
 * 
 */

#include "ble_handler.h"
#include "src/SerialDebug/SerialDebug.h"
#include "global.h"
#include <ArduinoJson.h>

BLEServer *pServer = NULL;
BLECharacteristic *pPeripheralNotifyCharacteristic;
BLECharacteristic *pConfigurationCharacteristic;
BLECharacteristic *pForceRelayCharacteristic;
BLECharacteristic *pSystemTimeCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

class FixtureServerCallback : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class ConfigurationCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0)
        {
            if (parseWriteConfig(value.c_str()))
            {
                debugI("Will reboot in order to apply modified configuaration");
                ESP.reset();
            }
        }
    }

    void onRead(BLECharacteristic *pCharacteristic)
    {
        DynamicJsonDocument doc(1024);
        doc["mode"] = config.mode;
        doc["name"] = config.identifier;
        doc["relay_count"] = config.relay_count;

        JsonArray relays = doc.createNestedArray("relays");
        for (uint8_t idx = 0; idx < config.relay_count; idx++)
        {
            JsonObject relay = relays.createNestedObject();
            relay["desc"] = config.relays[idx].description;
            relay["channel"] = config.relays[idx].channel;
        }
        char configJson[1024];
        serializeJson(doc, configJson);
        pCharacteristic->setValue((uint8_t *)configJson, strlen(configJson));
    }
};

class ForceRelayCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0)
        {
                }
    }
};

class SystemTimeCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0)
        {
            String formattedDate = String(value.c_str());

            // Extract date
            int splitT = formattedDate.indexOf("T");
            String dateStamp = formattedDate.substring(0, splitT);

            // Extract time
            String timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);

            struct tm tm;
            tm.tm_year = getSplitString(dateStamp, '-', 0).toInt() - 1900;
            tm.tm_mon = getSplitString(dateStamp, '-', 1).toInt();
            tm.tm_mday = getSplitString(dateStamp, '-', 2).toInt();
            tm.tm_hour = getSplitString(timeStamp, ':', 0).toInt();
            tm.tm_min = getSplitString(timeStamp, ':', 1).toInt();
            tm.tm_sec = 0;
            time_t t = mktime(&tm);
            struct timeval now = {.tv_sec = t};
            settimeofday(&now, NULL);
        }
    }
};

void init_ble_service()
{
    // Create the BLE Device
    BLEDevice::init(config.identifier);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new FixtureServerCallback());

    // Create the BLE Service
    BLEService *pService = pServer->createService(FIXTURE_SERVICE_UUID);

    // Create a BLE Characteristic
    pPeripheralNotifyCharacteristic = pService->createCharacteristic(
        PERIPHERALS_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);

    // Create a BLE Descriptor
    pPeripheralNotifyCharacteristic->addDescriptor(new BLE2902());

    pConfigurationCharacteristic = pService->createCharacteristic(
        CONFIGURATION_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE);
    pConfigurationCharacteristic->setCallbacks(new ConfigurationCallback());

    pForceRelayCharacteristic = pService->createCharacteristic(
        FORCERELAY_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    pForceRelayCharacteristic->setCallbacks(new ForceRelayCallback());

    pSystemTimeCharacteristic = pService->createCharacteristic(
        FORCERELAY_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);
    pSystemTimeCharacteristic->setCallbacks(new SystemTimeCallback());

    // Create a BLE Descriptor
    pSystemTimeCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(FIXTURE_SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");
}

void ble_handler_loop()
{
    if (deviceConnected)
    {
        static uint32_t notify_time = millis();
        if (millis() - notify_time >= 500)
        {
            notify_time = millis();

            StaticJsonDocument<1024> doc;
            JsonArray relays = doc.to<JsonArray>();
            uint32_t value = 0x00;
            for (uint8_t idx = 0; idx < config.relay_count; idx++)
            {
                JsonObject relay = relays.createNestedObject();
                relay["desc"] = config.relays[idx].description;
                relay["status"] = "on";
            }
            char payload[512];
            serializeJson(doc, payload);
            pPeripheralNotifyCharacteristic->setValue((uint8_t *)payload, strlen(payload));
            pPeripheralNotifyCharacteristic->notify();
        }

        static uint32_t datetime_notify_time = millis();
        if (millis() - datetime_notify_time >= 5000)
        {
            struct tm tmstruct;
            getLocalTime(&tmstruct);
            char payload[20];
            sprintf(payload, "%d-%02d-%02dT%02d:%02d", tmstruct.tm_year + 1900, tmstruct.tm_mon + 1, tmstruct.tm_mday,
                    tmstruct.tm_hour, tmstruct.tm_min);
            pSystemTimeCharacteristic->setValue((uint8_t *)payload, strlen(payload));
            pSystemTimeCharacteristic->notify();
            datetime_notify_time = millis();
        }
    }

    // // disconnecting
    // if (!deviceConnected && oldDeviceConnected)
    // {
    //     delay(500);                  // give the bluetooth stack the chance to get things ready
    //     pServer->startAdvertising(); // restart advertising
    //     Serial.println("start advertising");
    //     oldDeviceConnected = deviceConnected;
    // }

    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
