#ifndef SETTINGS_H
#define SETTINGS_H
#include "variables.h"
#include "wifi_self.h"
#include "input.h"
#include "big_keyboard.h"
#include "storage.h"

#include <vector>

int scroll_height = 0;
String entered_password = "";
String selected_wifi = "";
bool return_flag = false;

//! Private
void handle_settings_input()
{
    char key = keypad_get_key();
    if (key == '\0')
        return;

    if (key == '2')
        scroll_height -= 3;
    else if (key == '8')
        scroll_height += 3;
    else if (key == '5')
    {
        entered_password = big_keyboard_main();

        fill_screen_black();
        display_simple_text("Trying to connect to wifi");
        // TODO: Wont work cause selected_wifi
        // TODO: has signal strength in it

        bool connection_successful = connect_to_wifi(selected_wifi, entered_password);
        if (connection_successful)
        {
            append_wifi(selected_wifi, entered_password);
        }
    }
    else if (key == '*')
        return_flag = true;
}

//! Public
void settings_main()
{
    scroll_height = 0;
    entered_password = "";
    selected_wifi = "";
    return_flag = false;
    fill_screen_black();
    display_simple_text("Searching wifi...");
    String wifi_networks = list_wifi_networks();

    fill_screen_black();
    while (true)
    {
        handle_settings_input();
        if (return_flag)
        {
            set_tft_rotation(2);
            return;
        }
        selected_wifi = scrollable_text_box(0, 0, 128, 80, wifi_networks, scroll_height, scroll_height);
        delay(100);
    }
}

#endif