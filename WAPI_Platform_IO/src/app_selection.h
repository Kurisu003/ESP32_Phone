#ifndef APP_SELECTION_H
#define APP_SELECTION_H

#include "variables.h"
#include "display.h"
#include "image_data.h"
#include "calculator.h"
#include "settings.h"

#include "whatsapp_contacts.h"
#include "input.h"

RLE_Pixel *app_images[] = {
    settings_icon,
    whatsapp_icon,
    calc_icon};

int app_images_lengths[] = {
    settings_length,
    whatsapp_length,
    calc_length};

int num_app_images = sizeof(app_images) / sizeof(app_images[0]);

int selected_app = 0;

void start_app()
{
    switch (selected_app)
    {
    case 0:
        settings_main();
        selected_app = 0;
        break;
    case 1:
        whatsapp_main();
        selected_app = 0;
        break;
    case 2:
        calculator_main();
        selected_app = 1;
        break;
    }
}

void handle_app_selection_input()
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
    while (true)
    {
        delay(20);
        handle_app_selection_input();

        if (selected_app == previously_selected_app)
            continue;

        draw_apps(app_images, app_images_lengths, num_app_images);
        draw_selected_frame(selected_app);
        previously_selected_app = selected_app;
    }
}

#endif