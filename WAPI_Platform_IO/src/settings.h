#ifndef SETTINGS_H
#define SETTINGS_H

#include "input.h"
#include "settings_wifi.h"

#define WIFI "Wifi"
#define BLUETOOTH "Bluetooth"

String availible_settings[] = {WIFI, BLUETOOTH};
int num_settings = sizeof(availible_settings) / sizeof(availible_settings[0]);

int selected_setting = 0;
int prev_selected_setting = -1;
bool settings_return = false;

//! Private
void launch_setting()
{
    String current_setting = availible_settings[selected_setting];

    if (current_setting == WIFI)
    {

        settings_wifi_main();
        // Reset prev to update display when returning
        prev_selected_setting = -1;
    }
    else if (current_setting == BLUETOOTH)
    {

        // TODO:

        // Reset prev to update display when returning
        prev_selected_setting = -1;
    }
}

//! Private
void handle_settings_input()
{
    char c = keypad_get_key();
    if (c == '\0')
        return;
    if (c == '8')
        selected_setting = (selected_setting + 1) % num_settings;
    if (c == '2')
        selected_setting--;

    if (c == '5')
        launch_setting();

    if (selected_setting < 0)
        selected_setting = num_settings - 1;
}

//! Public
void settings_main()
{
    selected_setting = 0;
    prev_selected_setting = -1;
    settings_return = false;

    while (true)
    {
        delay(50);
        handle_settings_input();

        // stuff that happens always

        if (settings_return)
            return;

        if (prev_selected_setting == selected_setting)
            continue;

        // update display (runs if stuff changes)
    }
}

#endif