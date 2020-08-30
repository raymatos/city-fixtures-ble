
/**
 * 
 * FILE       : ble_handler.h
 * PROJECT    : 
 * AUTHOR     : 
 * 
 *
 */

#pragma once
#include <Arduino.h>

#include "src/BLE/BLEDevice.h"
#include "src/BLE/BLEServer.h"
#include "src/BLE/BLEUtils.h"
#include "src/BLE/BLE2902.h"

#define FIXTURE_SERVICE_UUID "0f7e0001-3e26-454e-9669-1a8b67b52161"                // Fixture BLE service UUID
#define PERIPHERALS_CHARACTERISTIC_UUID "0f7e0002-b5a3-f393-e0a9-e50e24dcca9e"     //
#define CONFIGURATION_CHARACTERISTIC_UUID "0f7e0003-b5a3-f393-e0a9-e50e24dcca9e"   //
#define FORCERELAY_CHARACTERISTIC_UUID "0f7e0004-b5a3-f393-e0a9-e50e24dcca9e"      //
#define SELECT_SCHEDULE_CHARACTERISTIC_UUID "0d7e0005-b5a3-f393-e0a9-e50e24dcca9e" //
#define SYSTEMTIME_CHARACTERISTIC_UUID "0d7e0006-b5a3-f393-e0a9-e50e24dcca9e"      //

void init_ble_service();
void ble_handler_loop();