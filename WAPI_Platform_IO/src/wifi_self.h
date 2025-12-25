#ifndef WIFI_H
#define WIFI_H
#include <display.h>
#include <WiFi.cpp>
#include "storage.h"
#include "variables.h"

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
        display_simple_text("Connecting to WiFi: " + ssid);

        if (millis() - startTime >= WIFI_CONNECTION_TIMEOUT)
        {
            Serial.println("WiFi connection failed.");
            return false; // connection failed
        }

        delay(250); // small delay to avoid busy looping
    }

    display_simple_text("WiFi Connected :)");
    Serial.println("WiFi connected successfully!");
    return true; // connection succeeded
}

//! Public
bool auto_connect_to_wifi()
{
    std::vector<std::pair<String, String>> wifiList = read_all_wifi();

    for (auto &entry : wifiList)
    {
        Serial.println("----");
        Serial.print("SSID: ");
        Serial.println(entry.first);
        Serial.print("Password: ");
        Serial.println(entry.second);
        Serial.println("----");
        display_simple_text("Trying to connect to: " + entry.first);
        bool connection_successful = connect_to_wifi(entry.first, entry.second);
        if (connection_successful)
        {
            return true;
        }
    }
    return false;
}

#endif