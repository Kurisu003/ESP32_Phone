#ifndef STORAGE_H
#define STORAGE_H

#include <FS.h>
#include <SPIFFS.h>

inline void storage_init()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

// Write a string to a file (overwrite)
inline void write_to_file(const String &file_name, const String &text)
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
inline void append_to_file(const String &file_name, const String &text)
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
inline String read_from_file(const String &file_name)
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

#endif
