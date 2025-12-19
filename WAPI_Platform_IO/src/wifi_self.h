#ifndef WIFI_H
#define WIFI_H
#include <display.h>
#include <WiFi.cpp>

//! Public
void connect_to_wifi(char *ssid, char *password)
{
    WiFi.begin(ssid, password);
    unsigned long startTime = millis(); // Record the start time

    while (WiFi.status() != WL_CONNECTED)
    {
        display_simple_text("Connecting to WiFi...");

        // Check if 10 seconds have passed
        if (millis() - startTime >= 10000)
        {
            display_simple_text("WiFi connection timed out!");
            return;
        }
    }

    display_simple_text("WiFi Connected :)");
}

#endif