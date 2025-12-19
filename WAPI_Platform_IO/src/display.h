#ifndef DISPLAY_H
#define DISPLAY_H

#include "User_Setup.h"
#include "variables.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <vector>

#define IMG_W 32
#define IMG_H 32

bool display_initialized = false;

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

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
//! Public
void screen_init()
{
    if (!display_initialized)
    {
        tft.setRotation(2);

        tft.init(); // initialize a ST7735S chip
        tft.invertDisplay(0);

        tft.fillScreen(TFT_BLACK);
        display_initialized = true;
    }
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

//! Private
void set_pixel_color(int x, int y, int r, int g, int b)
{
    uint16_t color = tft.color565(r, g, b);
    tft.drawPixel(x, y, color);
}

//! Private
static inline void rgb565_to_rgb888(
    uint16_t c,
    uint8_t *r,
    uint8_t *g,
    uint8_t *b)
{
    *r = ((c >> 11) & 0x1F) << 3;
    *g = ((c >> 5) & 0x3F) << 2;
    *b = (c & 0x1F) << 3;
}

//! Private
void decode_rle_32x32(
    const RLE_Pixel *data,
    uint32_t data_len,
    uint8_t rgb[IMG_H][IMG_W][3])
{
    uint32_t pixel_index = 0;
    const uint32_t max_pixels = IMG_W * IMG_H;

    for (uint32_t i = 0; i < data_len && pixel_index < max_pixels; i++)
    {
        uint16_t run = data[i].count;
        uint16_t color = data[i].color;

        uint8_t r, g, b;
        rgb565_to_rgb888(color, &r, &g, &b);

        for (uint16_t j = 0; j < run && pixel_index < max_pixels; j++)
        {
            uint16_t x = pixel_index % IMG_W;
            uint16_t y = pixel_index / IMG_W;

            rgb[y][x][0] = r;
            rgb[y][x][1] = g;
            rgb[y][x][2] = b;

            pixel_index++;
        }
    }
}

//! Public
void draw_image_at_position(int x_offset, int y_offset, RLE_Pixel *image_data, int image_data_len)
{
    uint8_t image_rgb[IMG_H][IMG_W][3];
    decode_rle_32x32(image_data, image_data_len, image_rgb);

    for (uint16_t y = 0; y < IMG_H; y++)
    {
        for (uint16_t x = 0; x < IMG_W; x++)
        {
            int r = image_rgb[y][x][0];
            int g = image_rgb[y][x][1];
            int b = image_rgb[y][x][2];
            set_pixel_color(x + x_offset, y + y_offset, r, g, b);
        }
        Serial.println();
    }
}

//! Public
void draw_apps(RLE_Pixel **app_images, int *app_images_lengths, int num_app_images)
{
    fill_screen_black();
    int x = 0;                    // starting x position
    int y = 0;                    // starting y position
    const int spacing_x = 48;     // horizontal spacing between images
    const int spacing_y = 42;     // vertical spacing when wrapping to next row
    const int images_per_row = 3; // number of images per row

    for (int i = 0; i < num_app_images; i++)
    {
        RLE_Pixel *current_image = app_images[i];
        int length = app_images_lengths[i];

        // Draw the image at the current position
        draw_image_at_position(x, y, current_image, length);

        // Move to the next horizontal position
        x += spacing_x;

        // After every 3 images, move to next row
        if ((i + 1) % images_per_row == 0)
        {
            x = 0;          // reset x to start
            y += spacing_y; // move down
        }
    }
}

void draw_selected_frame(int selected)
{
    int x_index = selected % 3;
    int y_index = selected / 3;
    tft.drawRect(x_index * 48, y_index * 42, 32, 32, TFT_BLUE);
}

#endif