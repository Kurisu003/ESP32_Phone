#ifndef BIG_KEYBOARD_H
#define BIG_KEYBOARD_H

#include "display.h"
#include "input.h"

int keyboard_selected_x = 0;
int keyboard_selected_y = 0;
int prev_x = -1;
int prev_y = -1;
int selected_keyboard = 0;
String text = "";
String prev_text = "0";
bool return_text = false;

void press_selected()
{
    char c = '\0';

    if (selected_keyboard == 0)
        c = *keyboard_keys_normal[keyboard_selected_y][keyboard_selected_x];

    if (selected_keyboard == 1)
        c = *keyboard_keys_caps[keyboard_selected_y][keyboard_selected_x];

    if (selected_keyboard == 2)
        c = *keyboard_keys_symbols[keyboard_selected_y][keyboard_selected_x];

    Serial.println(c);
    if (selected_keyboard == 0 && c == '^' && keyboard_selected_x == 0)
    {
        // prev set to reload index
        fill_screen_black();
        selected_keyboard = 1;
        prev_x = 0;
        prev_y = 0;
    }
    else if (selected_keyboard == 1 && c == '^' && keyboard_selected_x == 0)
    {
        // prev set to reload index
        fill_screen_black();
        selected_keyboard = 0;
        prev_x = 0;
        prev_y = 0;
    }
    else if (c == '!' && keyboard_selected_y != 0)
    {
        // prev set to reload index
        fill_screen_black();
        selected_keyboard = 2;
        prev_x = 0;
        prev_y = 0;
    }
    else if (c == 'A' && selected_keyboard == 2)
    {
        // prev set to reload index
        fill_screen_black();
        selected_keyboard = 0;
        prev_x = 0;
        prev_y = 0;
    }
    else
    {
        // string append char
        text += c;
    }
}

void handle_big_keyboard_input()
{
    // get physically pressed button
    char c = keypad_get_key();

    if (c == '5')
    {
        press_selected();
    }
    // Navigation
    else if (c == '8')
        keyboard_selected_x++;
    else if (c == '2')
        keyboard_selected_x--;
    else if (c == '6')
        keyboard_selected_y--;
    else if (c == '4')
        keyboard_selected_y++;
    // Deletion
    else if (c == '*' && text.length() > 0)
    {
        // delete last string char if present
        text.remove(text.length() - 1);
        blank_keyboard_text_area();
    }
    // Return
    else if (c == '1')
        return_text = true;

    if (keyboard_selected_x < 0)
        keyboard_selected_x = keyboard_matrix_x_amount - 1;
    if (keyboard_selected_y < 0)
        keyboard_selected_y = keyboard_matrix_y_amount - 1;

    keyboard_selected_x %= keyboard_matrix_x_amount;
    keyboard_selected_y %= keyboard_matrix_y_amount;
}

String big_keyboard_main()
{
    fill_screen_black();
    set_tft_rotation(3);

    keyboard_selected_x = 0;
    keyboard_selected_y = 0;
    prev_x = -1;
    prev_y = -1;
    selected_keyboard = 0;
    text = "Minecraft123!";
    prev_text = "0";
    return_text = false;

    while (1)
    {
        handle_big_keyboard_input();

        if (return_text)
        {
            return_text = false;
            return text;
        }

        if (prev_x == keyboard_selected_x && prev_y == keyboard_selected_y && prev_text == text)
            continue;

        draw_rect_matrix(
            keyboard_matrix_x_offset,
            keyboard_matrix_y_offset,
            keyboard_matrix_x_amount,
            keyboard_matrix_y_amount,
            keyboard_matrix_x_size,
            keyboard_matrix_y_size,
            keyboard_matrix_x_spacing,
            keyboard_matrix_y_spacing);

        draw_keyboard(selected_keyboard);
        draw_keyboard_selected(keyboard_selected_x, keyboard_selected_y);
        draw_keyboard_text(text);

        prev_x = keyboard_selected_x;
        prev_y = keyboard_selected_y;
        prev_text = text;
    };
}

#endif