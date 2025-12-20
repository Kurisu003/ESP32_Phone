#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

#define keyboard_matrix_x_offset 2
#define keyboard_matrix_y_offset 70
#define keyboard_matrix_x_amount 10
#define keyboard_matrix_y_amount 4
#define keyboard_matrix_x_size 12
#define keyboard_matrix_y_size 12
#define keyboard_matrix_x_spacing 3
#define keyboard_matrix_y_spacing 3

#define calc_matrix_x_offset 2
#define calc_matrix_y_offset 50
#define calc_matrix_x_amount 4
#define calc_matrix_y_amount 4
#define calc_matrix_x_size 25
#define calc_matrix_y_size 25
#define calc_matrix_x_spacing 8
#define calc_matrix_y_spacing 3

#define WIFI_CONNECTION_TIMEOUT 15000

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

// One row can be deleted
const char *keyboard_keys_normal[4][10] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", " "},
    {"^", "z", "x", "c", "v", "b", "n", "m", " ", "!"},
};

const char *keyboard_keys_caps[4][10] = {
    {"!", "@", "#", "$", "%", "^", "&", "*", "(", ")"},
    {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
    {"A", "S", "D", "F", "G", "H", "J", "K", "L", " "},
    {"^", "Z", "X", "C", "V", "B", "N", "M", " ", "!"},
};

const char *keyboard_keys_symbols[4][10] = {
    {"!", "@", "#", "$", "%", "^", "&", "*", "(", ")"},
    {"~", "`", "-", "_", "=", "+", "{", "}", "[", "]"},
    {",", ".", "/", "?", ";", ":", "'", "\"", "\\", "|"},
    {"<", ">", " ", " ", " ", " ", " ", " ", " ", "A"},
};

#endif