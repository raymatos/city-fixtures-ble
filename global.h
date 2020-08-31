
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
#include "string.h"
#include "src/SerialDebug/SerialDebug.h"
#include "src/ESPAsyncWiFiManager.h"
#include "src/ESPAsyncWebServer.h"
#include "src/FSM.h"

#define DEFAULT_FLOOR_NAME "Floor"
#define DEFAULT_ELEVATOR_NAME "Elevator"
#define MAX_REALY_COUNT 32

enum DeviceMode
{
    FLOOR,
    ELEVATOR,
};

typedef struct
{
    char description[16];
    uint8_t channel;
} FloorRelay;

struct Config
{
    char identifier[32]; // BLE device name
    uint8_t mode;        // Device type

    uint8_t relay_count;
    FloorRelay relays[32];
};

extern Config config;

// Unique id of ESP32 core
extern char chip_serial[30];
extern const int32_t gmtOffset_sec;
extern const int32_t daylightOffset_sec;

void readChipId();
void loadFSConfig();
void saveFSConfig();

String getSplitString(String data, char separator, int index);