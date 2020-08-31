/**
 * 
 * FILE       : peripheral.cpp
 * PROJECT    : 
 * AUTHOR     : 
 * DESCRITION : 
 *
 */

#include "global.h"
#include "peripheral.h"
#include "src/SerialDebug/SerialDebug.h"

uint8_t base_relay_pins[] = {
    Relay1_Pin, Relay2_Pin, Relay3_Pin, Relay4_Pin, Relay5_Pin, Relay6_Pin, Relay7_Pin, Relay8_Pin};

void init_peripherals()
{
    debugI("Initializing peripherals: %s, %d relays", config.mode == FLOOR ? "Floor" : "Elevator", config.relay_count);
    for (uint8_t idx = 0; idx < 8; idx++)
    {
        pinMode(base_relay_pins[idx], OUTPUT);
        digitalWrite(base_relay_pins[idx], LOW);
    }
}

bool forceRelay(uint8_t channel, bool signal)
{
    if (channel > 0 && channel < 9)
    {
        digitalWrite(base_relay_pins[channel], signal);
        return true;
    }
    return false;
}

bool readRelay(uint8_t channel)
{
    if (channel > 0 && channel < 9)
    {
        return digitalRead(base_relay_pins[channel]);
    }
    return false;
}