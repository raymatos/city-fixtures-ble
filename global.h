
/**
 * 
 * FILE       : global.h
 * PROJECT    : 
 * AUTHOR     : 
 * DESCRITION : 
 *
 */

#pragma once
#include <Arduino.h>
#include "src/SerialDebug/SerialDebug.h"
#include "src/ESPAsyncWiFiManager.h"
#include "src/ESPAsyncWebServer.h"
#include "src/FSM.h"

enum DeviceType
{
    FLOOR,
    ELEVATOR,
};

struct Config
{
    uint8_t mode;
};

extern Config config;

// Unique id of ESP32 core
extern char chip_serial[30];
extern const int32_t gmtOffset_sec;
extern const int32_t daylightOffset_sec;

void readChipId();
void loadFSConfig();
void saveFSConfig();
