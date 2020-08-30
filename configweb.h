
/**
 * 
 * FILE       : configweb.h
 * PROJECT    : 
 * AUTHOR     : 
 * 
 *
 */

#pragma once
#include <Arduino.h>

#include "src/ESPAsyncWiFiManager.h"
#include "src/ESPAsyncWebServer.h"

/***************************************************/
#define SOFTAP_PASSWORD "password2020"
#define SOFTAP_SSID_PREFIX "fixture"

#define HTTP_EDIT_USER "admin"
#define HTTP_EDIT_PASS "admin123"

/**************************************************/

extern AsyncWebServer server;
extern AsyncWiFiManager wifiManager;

/**************************************************/

void initConfigWebService();
void configWebProc();
/**************************************************/