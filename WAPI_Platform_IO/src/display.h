#ifndef DISPLAY_H
#define DISPLAY_H

#include "User_Setup.h"
#include "variables.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <vector>

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

//! Public
void screen_init()
{
    // Use this initializer if you're using a 1.8" TFT

    // tft.height(), etc, wroks
    // fill versions also availible
    // x, y, w, h
    // tft.drawRect(0, 0, 10, 10, TFT_WHITE);
    // tft.drawLine(10, 10, 20, 20, TFT_WHITE);

    // tft.setCursor(40, 40);
    // tft.setTextColor(TFT_WHITE);
    // tft.setTextWrap(true);

    // x, y
    // tft.setCursor(0, 30);
    // tft.print("Penis");

    // tft.drawCircle(60, 60, 10, TFT_WHITE);

    // tft.drawTriangle(x_1, y_1, x_2, y_2, x_3, y_3, color);
    // tft.fillTriangle(0, 0, 100, 0, 50, 50, TFT_BLUE);

    // tft.drawRoundRect(x, y, w, h, 5, color);
    tft.init(); // initialize a ST7735S chip
    tft.invertDisplay(0);

    tft.fillScreen(TFT_BLACK);
}

//! Public
void drawApp(String name, int x, int index, bool is_selected)
{
    int y = (index * 20) + 5;
    if (is_selected)
    {
        tft.drawRect(x, y, 70, 18, TFT_BLUE);
    }
    else
    {
        tft.drawRect(x, y, 70, 18, TFT_WHITE);
    }

    tft.setCursor(x + 5, y + 5);
    tft.print(name);
}

//! Public
void fill_screen_black()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
}

//! Private
void display_contact(int x, int index, const char *contact, int selected_contact, int page, bool is_unread)
{
    // index used so the display draws correctly
    int index_for_draw = index - page * 8;
    int y = (index_for_draw * 20);
    if (index == selected_contact)
    {
        tft.drawRect(x, y, 70, 18, TFT_BLUE);
    }
    else
    {
        tft.drawRect(x, y, 70, 18, TFT_WHITE);
    }
    if (is_unread)
    {
        tft.fillCircle(x + 70, y, 3, TFT_BLUE);
    }

    char buffer[11]; // 10 chars + null terminator
    strncpy(buffer, contact, 10);
    buffer[10] = '\0'; // Force null-terminate

    tft.setCursor(x + 5, y + 5);
    tft.print(buffer);
}

//! Public
void display_simple_text(String text)
{
    fill_screen_black();
    tft.print(text);
}

//! Public
void draw_contacts(std::vector<String> contacts, std::vector<String> unread_contacts, int selected_contact)
{
    fill_screen_black();

    if (contacts.empty())
    {
        tft.print("No contacts found");
        return;
    }

    // Displays 8 contacts per page
    int page = selected_contact / 8;
    for (size_t i = page * 8; i < min(contacts.size(), (size_t)8 + page * 8); i++)
    {
        bool is_unread = std::find(unread_contacts.begin(), unread_contacts.end(), contacts[i]) != unread_contacts.end();
        display_contact(5, i, contacts[i].c_str(), selected_contact, page, is_unread);
        // display_contact(5, i, contacts[i].c_str(), selected_contact, page);
    }

    tft.setCursor(100, 5);
    tft.print(page + 1);
    tft.print("/");
    tft.print(int(ceil(contacts.size() / 8.0)));
}

//! Public
void render_whatsapp_chat_screen(int scroll_pos, std::vector<ChatLine> all_lines)
{

    fill_screen_black();
    int y = 0;
    const int line_h = 10;
    int max_lines = (tft.height() - 30) / line_h;

    for (int i = 0; i < max_lines && scroll_pos + i < all_lines.size(); i++)
    {
        const auto &line = all_lines[scroll_pos + i];
        tft.setTextColor(line.color);
        tft.setCursor(0, y);
        tft.print(line.text);
        y += line_h;
    }
}

//! Public
void render_whatsapp_type_mode_rect(bool type_mode)
{
    tft.fillRect(0, 130, 128, 30, TFT_BLACK);
    if (type_mode)
        tft.drawRect(0, 130, 128, 30, TFT_BLUE);
    else
        tft.drawRect(0, 130, 128, 30, TFT_WHITE);
}

//! Public
int get_tft_height()
{
    return tft.height();
}

//! Public
int get_tft_width()
{
    return tft.width();
}

//! Public
void render_whatsapp_typed_text(String current_text)
{
    tft.setCursor(2, 132);
    tft.print(current_text);
}

#endif