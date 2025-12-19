#include "HttpsUtils.h"
#include "helpers.h"
#include "variables.h"

#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>

#include <vector>

#include "variables.h"

#define PIN_UP 25
#define PIN_DOWN 33
#define DEBOUNCE 50 // ms
#define MESSAGE_CUTOFF 20

std::vector<ChatLine> all_lines;
int scroll_pos = 0;
unsigned long last_up = 0, last_down = 0;
unsigned int refetch_message_counter = 0;

#include <vector>
#include <Arduino.h>

std::vector<String> get_line_segmented_by_amount(const String &text, int amount)
{
    std::vector<String> segments;
    if (amount <= 0)
        return segments; // invalid amount

    int pos = 0;
    int text_len = text.length();

    while (pos < text_len)
    {
        // Determine the maximum end position for this segment
        int end = pos + amount;
        if (end >= text_len)
        {
            // Last segment, take remaining text
            segments.push_back(text.substring(pos));
            break;
        }

        // Look for the last space within this segment to keep whole words
        int last_space = -1;
        for (int i = pos; i < end; i++)
        {
            if (text[i] == ' ')
                last_space = i;
        }

        if (last_space != -1 && last_space > pos)
        {
            // Break at the last space
            segments.push_back(text.substring(pos, last_space));
            pos = last_space + 1; // skip the space
        }
        else
        {
            // No space found, force split
            segments.push_back(text.substring(pos, end));
            pos = end;
        }
    }

    return segments;
}

void init_chat_and_calc_scroll_pos(const char *contact)
{
    String response = get_whatsapp_info("messages_from_contact/" + String(contact));

    if (response.length() == 0)
    {
        display_simple_text("no response");
        while (1)
        {
        }
    }

    // StaticJsonDocument<6144> doc;
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, response);
    if (error)
    {
        display_simple_text(error.f_str());
        while (1)
        {
        }
    }

    all_lines.clear();
    all_lines.reserve(80);

    for (JsonArray msg : doc.as<JsonArray>())
    {
        const char *sender = msg[0] | "";
        const char *message = msg[1] | "";
        if (strlen(message) == 0)
            continue;

        uint16_t color = (String(sender) == "You") ? TFT_GREEN : TFT_CYAN;

        // Prepend sender name
        String full_message = String(sender) + ": " + String(message);

        // Add to the all_lines vector
        std::vector<String> segments = get_line_segmented_by_amount(full_message, MESSAGE_CUTOFF);

        bool first = true;
        for (const String &seg : segments)
        {
            String line = first ? seg : " " + seg;
            all_lines.push_back({line, color});
            first = false;
        }

        // Add an additional empty line after each message for spacing
        all_lines.push_back({"", color});
    }

    // Adjust scroll position to show the last messages
    int visible_lines = (get_tft_height() - 30) / 10; // adjust line height as needed
    scroll_pos = max(0, (int)all_lines.size() - visible_lines);
}

void fresh_chat_init(const char *contact)
{
    init_chat_and_calc_scroll_pos(contact);
    render_whatsapp_chat_screen(scroll_pos, all_lines);
}

void whatsapp_chat_main(const char *contact)
{
    bool type_mode = false;

    static char lastKey = 0;
    static unsigned long lastPressTime = 0;
    static int pressCount = 0;
    static String current_text = "";
    const unsigned long multiTapTimeout = 800; // ms before committing a letter

    // Mapping for T9 keys
    const char *t9map[10] = {
        "",      // 0
        "",      // 1 (unused)
        "abc01", // 2
        "def23", // 3
        "ghi4",  // 4
        "jkl5",  // 5
        "mno6",  // 6
        "pqrs7", // 7
        "tuv8",  // 8
        "wxyz9"  // 9
    };

    init_chat_and_calc_scroll_pos(contact);
    String current_user_message;

    if (all_lines.empty())
    {
        display_simple_text("Empty");
        while (1)
            delay(100);
    }

    render_whatsapp_chat_screen(scroll_pos, all_lines);

    while (1)
    {
        char key = keypad_get_key();

        if (key == '#')
            type_mode = !type_mode;

        // Scroll UP (show older messages)
        if (!type_mode)
        {
            if (key == '*')
                return;
            if (key == '2')
            {
                if (scroll_pos > 0)
                {
                    scroll_pos--;
                    render_whatsapp_chat_screen(scroll_pos, all_lines);
                }
            }

            // Scroll DOWN (show newer messages)
            if (key == '8')
            {
                int visible_lines = (tft.height() - 30) / 10;
                int max_scroll = max(0, (int)all_lines.size() - visible_lines);
                if (scroll_pos < max_scroll)
                {
                    scroll_pos++;
                    render_whatsapp_chat_screen(scroll_pos, all_lines);
                }
            }
        }
        else
        {
            if (key >= '2' && key <= '9')
            {
                unsigned long now = millis();

                if (key == lastKey && (now - lastPressTime) < multiTapTimeout)
                {
                    // Cycle to next letter on same key
                    pressCount++;
                    const char *letters = t9map[key - '0'];
                    int numLetters = strlen(letters);
                    if (numLetters > 0)
                    {
                        current_text[current_text.length() - 1] = letters[pressCount % numLetters];
                    }
                }
                else
                {
                    // New key or timeout → commit previous letter and start a new one
                    pressCount = 0;
                    const char *letters = t9map[key - '0'];
                    if (strlen(letters) > 0)
                        current_text += letters[0];
                }

                lastPressTime = now;
                lastKey = key;

                // render_text(currentText); // Your function to update display
            }
            // either delete or refetch text
            if (key == '*')
            {
                if (current_text.length() > 0)
                {
                    current_text.remove(current_text.length() - 1);
                    // render_text(currentText);
                }
                else
                {
                    fresh_chat_init(contact);
                }
                lastKey = 0; // reset so it doesn't interfere with cycling
                pressCount = 0;
            }

            if (key == '0')
            {
                current_text += ' ';
                lastKey = 0;
                pressCount = 0;
            }

            // Send message
            if (key == '1')
            {
                send_message_to_contact(contact, current_text);
                fresh_chat_init(contact);
                current_text = "";
            }

            // Commit last character automatically after timeout
            if (lastKey && (millis() - lastPressTime) > multiTapTimeout)
            {
                lastKey = 0;
                pressCount = 0;
            }
        }

        render_whatsapp_type_mode_rect(type_mode);

        render_whatsapp_typed_text(current_text);

        delay(10);
        refetch_message_counter++;
        if (refetch_message_counter % 10000 == 0)
        {
            refetch_message_counter = 0;
            fresh_chat_init(contact);
        }
    }
}