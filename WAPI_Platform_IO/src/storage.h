#ifndef STORAGE_H
#define STORAGE_H

#include <FS.h>
#include <SPIFFS.h>

#include <vector>

void storage_init()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

// Write a string to a file (overwrite)
void write_to_file(const String &file_name, const String &text)
{
    fs::File file = SPIFFS.open(file_name, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    file.print(text); // use print instead of println if you don't want extra newline
    file.close();
}

// Append a string to a file
void append_to_file(const String &file_name, const String &text)
{
    fs::File file = SPIFFS.open(file_name, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }

    file.print(text); // append without adding newline automatically
    file.close();
}

// Read the entire contents of a file into a string
String read_from_file(const String &file_name)
{
    fs::File file = SPIFFS.open(file_name, FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return "";
    }

    String result = file.readString();
    file.close();
    return result;
}

void append_wifi(const String &entry, const String &file_name = "/wifi.txt")
{
    // Split incoming entry into ssid and password
    int sepIndex = entry.indexOf('\n');
    if (sepIndex == -1)
    {
        Serial.println("Invalid entry, must be 'ssid\\npassword'");
        return;
    }
    String new_ssid = entry.substring(0, sepIndex);
    String new_pwd = entry.substring(sepIndex + 1);

    // Read existing file contents
    String fileContents = "";
    if (SPIFFS.exists(file_name))
    {
        fs::File file = SPIFFS.open(file_name, FILE_READ);
        if (file)
        {
            fileContents = file.readString();
            file.close();
        }
    }

    // Split existing contents into entries
    std::vector<String> entries;
    int start = 0;
    while (true)
    {
        int doubleNewline = fileContents.indexOf("\n\n", start);
        if (doubleNewline == -1)
            break;

        String entryStr = fileContents.substring(start, doubleNewline);
        entries.push_back(entryStr);
        start = doubleNewline + 2; // skip the \n\n
    }

    bool updated = false;

    // Check for existing SSID
    for (size_t i = 0; i < entries.size(); i++)
    {
        int idx = entries[i].indexOf('\n');
        if (idx != -1)
        {
            String ssid = entries[i].substring(0, idx);
            if (ssid == new_ssid)
            {
                entries[i] = new_ssid + "\n" + new_pwd; // overwrite password
                updated = true;
                break;
            }
        }
    }

    // If not found, append new entry
    if (!updated)
    {
        entries.push_back(new_ssid + "\n" + new_pwd);
    }

    // Rebuild file content
    String newFileContents = "";
    for (size_t i = 0; i < entries.size(); i++)
    {
        newFileContents += entries[i];
        newFileContents += "\n\n"; // double newline separator
    }

    // Write back to file
    fs::File file = SPIFFS.open(file_name, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    file.print(newFileContents);
    file.close();
}

std::vector<std::pair<String, String>> read_all_wifi(const String &file_name = "/wifi.txt")
{
    std::vector<std::pair<String, String>> result;

    if (!SPIFFS.exists(file_name))
    {
        // File does not exist, return empty vector
        return result;
    }

    fs::File file = SPIFFS.open(file_name, FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open WiFi file for reading");
        return result;
    }

    String fileContents = file.readString();
    file.close();

    int start = 0;
    while (true)
    {
        int doubleNewline = fileContents.indexOf("\n\n", start);
        String entry;
        if (doubleNewline == -1)
        {
            // Last entry
            entry = fileContents.substring(start);
            if (entry.length() == 0)
                break; // empty
        }
        else
        {
            entry = fileContents.substring(start, doubleNewline);
        }

        int sepIndex = entry.indexOf('\n');
        if (sepIndex != -1)
        {
            String ssid = entry.substring(0, sepIndex);
            String password = entry.substring(sepIndex + 1);
            result.push_back(std::make_pair(ssid, password));
        }

        if (doubleNewline == -1)
            break;
        start = doubleNewline + 2; // skip \n\n
    }

    return result;
}

#endif
