#ifndef WIFI_H
#define WIFI_H
#include <display.h>
#include <WiFi.cpp>

//! Public
String list_wifi_networks()
{
    int n = WiFi.scanNetworks();
    String result = "";

    if (n == 0)
    {
        result = "No networks found";
    }
    else
    {
        for (int i = 0; i < n; ++i)
        {
            String ssid = WiFi.SSID(i);
            int rssi = WiFi.RSSI(i);
            result += ssid + "\n(" + String(rssi) + " dBm)";
            if (i < n - 1)
            {
                result += "\n\n"; // Separate entries with newline
            }
        }
    }

    return result;
}

//! Public
void connect_to_wifi(String ssid, String password)
{
    Serial.println("Connecting to:");
    Serial.println(ssid);
    Serial.println(password);
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