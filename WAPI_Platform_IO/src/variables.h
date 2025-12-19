#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

constexpr const char *BASE_IP = "154.16.36.201";
constexpr const char *BASE_PORT = "38823";

struct ChatLine
{
    String text;
    uint16_t color;
};

struct RLE_Pixel
{
    uint16_t count;
    uint16_t color; // RGB565
};

const char *calculator_keys[4][4] = {
    {"7", "8", "9", "/"},
    {"4", "5", "6", "*"},
    {"1", "2", "3", "-"},
    {"0", ".", "=", "+"}};

#endif