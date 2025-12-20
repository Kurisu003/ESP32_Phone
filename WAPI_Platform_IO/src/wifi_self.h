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
bool connect_to_wifi(const String &ssid, const String &password)
{
    Serial.println("Connecting to:");
    Serial.println(ssid);
    Serial.println(password);

    WiFi.begin(ssid.c_str(), password.c_str()); // ensure const char* compatibility
    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        display_simple_text("Connecting to WiFi...");

        // Timeout after 10 seconds
        if (millis() - startTime >= 10000)
        {
            display_simple_text("WiFi connection timed out!");
            Serial.println("WiFi connection failed.");
            return false; // connection failed
        }

        delay(250); // small delay to avoid busy looping
    }

    display_simple_text("WiFi Connected :)");
    Serial.println("WiFi connected successfully!");
    return true; // connection succeeded
}

#endif