#ifndef SETTINGS_H
#define SETTINGS_H

#include "input.h"
#include "display.h"
#include "settings_wifi.h"

#define WIFI "Wifi"
#define BLUETOOTH "Bluetooth"
#define WIFI_AUTO_CONNECT "Wifi auto connect"

#define SETTING_SCROLL_INCREMENT 1

String availible_settings = String(WIFI) + "\n" + String(BLUETOOTH) + "\n" + String(WIFI_AUTO_CONNECT);

int setting_scroll_height = 0;
int prev_setting_scroll_height = -1;
String selected_setting = "";
bool settings_return_flag = false;

//! Private
void launch_setting()
{

    if (selected_setting == WIFI)
    {

        settings_wifi_main();
        // Reset prev to update display when returning
        prev_setting_scroll_height = -1;
    }
    else if (selected_setting == BLUETOOTH)
    {

        // TODO:

        // Reset prev to update display when returning
        prev_setting_scroll_height = -1;
    }
    else if (selected_setting == WIFI_AUTO_CONNECT){
        bool wifi_connected = auto_connect_to_wifi();
        
    }
}

//! Private
void handle_settings_input()
{
    char key = keypad_get_key();
    if (key == '\0')
        return;

    if (key == '2')
        setting_scroll_height -= SETTING_SCROLL_INCREMENT;
    else if (key == '8')
        setting_scroll_height += SETTING_SCROLL_INCREMENT;

    else if (key == '5')
        launch_setting();

    else if (key == '*')
        settings_return_flag = true;
}

//! Public
void settings_main()
{
    setting_scroll_height = 0;
    prev_setting_scroll_height = -1;

    selected_setting = "";
    settings_return_flag = false;

    while (true)
    {
        delay(50);
        handle_settings_input();

        if (settings_return_flag)
            return;

        if (prev_setting_scroll_height == setting_scroll_height)
            continue;

        selected_setting = scrollable_text_box(0, 0, 128, 160, availible_settings, setting_scroll_height, setting_scroll_height);
        prev_setting_scroll_height = setting_scroll_height;
    }
}

#endif