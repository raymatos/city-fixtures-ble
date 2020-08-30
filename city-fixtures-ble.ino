
#include "global.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include "src/SPIFFSEditor.h"
#include <Update.h>

#include "time.h"
#include "configweb.h"
#include "peripheral.h"
#include "ble_handler.h"

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setTimeZone(long offset, int daylight)
{
    char cst[17] = {0};
    char cdt[17] = "DST";
    char tz[33] = {0};

    if (offset % 3600)
    {
        sprintf(cst, "UTC%ld:%02u:%02u", offset / 3600, abs((offset % 3600) / 60), abs(offset % 60));
    }
    else
    {
        sprintf(cst, "UTC%ld", offset / 3600);
    }
    if (daylight != 3600)
    {
        long tz_dst = offset - daylight;
        if (tz_dst % 3600)
        {
            sprintf(cdt, "DST%ld:%02u:%02u", tz_dst / 3600, abs((tz_dst % 3600) / 60), abs(tz_dst % 60));
        }
        else
        {
            sprintf(cdt, "DST%ld", tz_dst / 3600);
        }
    }
    sprintf(tz, "%s%s", cst, cdt);
    setenv("TZ", tz, 1);
    tzset();
}

void setup()
{
    //
    Serial.begin(115200);

    // Get cunique chip ID of ESP32 core
    (void)readChipId();
    debugA("Unique ID of core is %s\r\n", chip_serial);

    // Initialize SPIFFS module
    while (!SPIFFS.begin())
    {
        debugE("Failed to mount SPIFFS ... \r\n");
        SPIFFS.format();
        delay(2500);
    }

    // Loading configuration from SPIFFS
    (void)loadFSConfig();

    WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    (void)initConfigWebService();

    // Initialize and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, "time.nist.gov", "0.pool.ntp.org", "pool.ntp.org");
    // setTimeZone(-gmtOffset_sec, daylightOffset_sec);

    // Init peripherals
    init_peripherals();

    // Init BLE handler
    init_ble_service();
}

void loop()
{
    //
    configWebProc();

    //
    ble_handler_loop();
}
