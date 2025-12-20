#ifndef SETTINGS_WIFI_H
#define SETTINGS_WIFI_H

#include "variables.h"
#include "wifi_self.h"
#include "input.h"
#include "big_keyboard.h"
#include "storage.h"

#include <vector>

#define WIFI_SETTING_SCROLL_INCREMENT 3

int wifi_setting_scroll_height = 0;
int prev_wifi_setting_scroll_height = -1;
String selected_wifi = "";
String entered_password = "";
bool settings_wifi_return_flag = false;

//! Private
void handle_settings_wifi_input()
{
    char key = keypad_get_key();
    if (key == '\0')
        return;

    if (key == '2')
        wifi_setting_scroll_height -= WIFI_SETTING_SCROLL_INCREMENT;
    else if (key == '8')
        wifi_setting_scroll_height += WIFI_SETTING_SCROLL_INCREMENT;
    else if (key == '5')
    {
        entered_password = big_keyboard_main();

        fill_screen_black();
        display_simple_text("Trying to connect to wifi");

        bool connection_successful = connect_to_wifi(selected_wifi, entered_password);
        if (connection_successful)
        {
            append_wifi(selected_wifi, entered_password);
        }
    }
    else if (key == '*')
        settings_wifi_return_flag = true;
}

//! Public
void settings_wifi_main()
{
    wifi_setting_scroll_height = 0;
    entered_password = "";
    selected_wifi = "";
    settings_wifi_return_flag = false;
    fill_screen_black();
    display_simple_text("Searching wifi...");
    String wifi_networks = list_wifi_networks();

    fill_screen_black();
    while (true)
    {
        delay(50);
        handle_settings_wifi_input();
        if (settings_wifi_return_flag)
        {
            set_tft_rotation(2);
            return;
        }

        if (prev_wifi_setting_scroll_height == wifi_setting_scroll_height)
            continue;

            selected_wifi = scrollable_text_box(0, 0, 128, 80, wifi_networks, wifi_setting_scroll_height, wifi_setting_scroll_height);
        delay(100);
    }
}

#endif
