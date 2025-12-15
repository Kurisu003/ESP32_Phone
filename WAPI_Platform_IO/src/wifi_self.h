#ifndef WIFI_H
#define WIFI_H
#include <display.h>
#include <WiFi.cpp>

//! Public
void conenct_to_wifi(char *ssid, char *password)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        tft.setCursor(0, 0);
        tft.print("Connecting to WiFi...");
    }
}

#endif