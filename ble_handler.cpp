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
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println();
            Serial.println("*********");
        }
    }

    void onRead(BLECharacteristic *pCharacteristic)
    {
        // struct timeval tv;
        // gettimeofday(&tv, nullptr);
        // std::ostringstream os;
        // os << "Time: " << tv.tv_sec;
        pCharacteristic->setValue("1234567890");
    }
};

class ForceRelayCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0)
        {
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println();
            Serial.println("*********");
        }
    }
};

void init_ble_service()
{
    // Create the BLE Device
    BLEDevice::init("ESP32");

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

    pForceRelayCharacteristic = pService->createCharacteristic(
        FORCERELAY_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    pForceRelayCharacteristic->setCallbacks(new ForceRelayCallback());

    pSystemTimeCharacteristic = pService->createCharacteristic(
        FORCERELAY_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY);
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
            static uint16_t value = 10;
            value++;
            notify_time = millis();
            pPeripheralNotifyCharacteristic->setValue((uint8_t *)&value, sizeof(value));
            pPeripheralNotifyCharacteristic->notify();
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
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}