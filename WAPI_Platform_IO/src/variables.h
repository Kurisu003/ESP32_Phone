#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

constexpr const char *BASE_IP = "154.16.36.201";
constexpr const char *BASE_PORT = "34641";

struct ChatLine
{
    String text;
    uint16_t color;
};

#endif