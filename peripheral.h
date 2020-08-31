
/**
 * 
 * FILE       : peripheral.h
 * PROJECT    : 
 * AUTHOR     : 
 * DESCRITION : 
 *
 */

#pragma once
#include <Arduino.h>
#include "src/SerialDebug/SerialDebug.h"

/******************************************************************/

#define Relay1_Pin 33
#define Relay2_Pin 32
#define Relay3_Pin 25
#define Relay4_Pin 26
#define Relay5_Pin 15
#define Relay6_Pin 5
#define Relay7_Pin 18
#define Relay8_Pin 19

//
void init_peripherals(uint8_t device_type, uint8_t relay_count);
bool driveRelay(uint8_t channel, bool signal);
bool readRelay(uint8_t channel);
