#ifndef APP_SELECTION_H
#define APP_SELECTION_H

#include "variables.h"
#include "display.h"
#include "image_data.h"

#include "whatsapp_contacts.h"
// #include "input.h"

// Settings
// Calc
// Whatsapp

RLE_Pixel *app_images[] = {
    settings_icon,
    whatsapp_icon,
    whatsapp_icon,
    calc_icon};

int app_images_lengths[] = {
    settings_length,
    whatsapp_length,
    whatsapp_length,
    calc_length};

int num_app_images = sizeof(app_images) / sizeof(app_images[0]);

int selected_app = 3;

void start_app()
{
    switch (selected_app)
    {
    case 2:
        whatsapp_main();
        break;
    }
}

void handle_input()
{
    char key = keypad_get_key();
    if (key == '\0')
        return;

    if (key == '6')
        selected_app = (selected_app + 1) % num_app_images;

    if (key == '4')
        selected_app = (selected_app - 1) % num_app_images;

    if (key == '2')
        selected_app = (selected_app - 3) % num_app_images;

    if (key == '8')
        selected_app = (selected_app + 3) % num_app_images;

    if (selected_app < 0)
        selected_app = 0;

    if (key == '5')
        start_app();
}

void app_selection_main()
{
    int previously_selected_app = -1;
    // Later to be called only when something chagned
    while (true)
    {
        delay(20);
        handle_input();

        if (selected_app == previously_selected_app)
            continue;

        draw_apps(app_images, app_images_lengths, num_app_images);
        draw_selected_frame(selected_app);
        previously_selected_app = selected_app;
    }
}

#endif