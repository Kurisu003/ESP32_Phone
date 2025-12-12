#include "HttpsUtils.h"
#include "helpers.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>

#include "variables.h"
using namespace std;

extern TFT_eSPI tft;
// char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";

#define PIN_UP 25
#define PIN_DOWN 33
#define DEBOUNCE 50 // ms

struct ChatLine
{
    String text;
    uint16_t color;
};

std::vector<ChatLine> all_lines;
int scroll_pos = 0;
unsigned long last_up = 0, last_down = 0;
unsigned int refetch_message_counter = 0;

void init_chat(const char *contact)
{

    // String url = "https://" + String(BASE_IP) + ":" + String(BASE_PORT) + "/api/messages_from_contact/" + String(contact);

    // String response = sendHttpsGet(url.c_str(), encrypted_api_key);
    String response = get_whatsapp_info("messages_from_contact/" + String(contact));

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);

    if (response.length() == 0)
    {
        tft.setCursor(0, 0);
        tft.print("No response");
        return;
    }

    StaticJsonDocument<6144> doc;
    DeserializationError error = deserializeJson(doc, response, DeserializationOption::NestingLimit(6));
    if (error)
    {
        tft.setCursor(0, 0);
        tft.print("JSON fail");
        return;
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
        String full = String(message);
        int pos = 0;
        bool first = true;

        while (pos < full.length())
        {
            int next = full.indexOf('\n', pos);
            if (next == -1)
                next = full.length();
            String line = full.substring(pos, next);
            String out = first ? String(sender) + ": " + line : "    " + line;
            all_lines.push_back({out, color});
            first = false;
            pos = next + 1;
        }
    }

    // scroll_pos = max(0, (int)all_lines.size() - 20);  // Start at bottom
    int visible_lines = (tft.height() - 30) / 10;
    scroll_pos = max(0, (int)all_lines.size() - visible_lines);
}

void render_screen()
{
    tft.fillScreen(TFT_BLACK);
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

void fresh_chat_init(const char *contact)
{
    init_chat(contact);
    render_screen();
}

void whatsapp_chat_main(const char *contact)
{
    bool type_mode = false;

    static char lastKey = 0;
    static unsigned long lastPressTime = 0;
    static int pressCount = 0;
    static String currentText = "";
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

    init_chat(contact);
    String current_user_message;

    if (all_lines.empty())
    {
        tft.setCursor(0, 0);
        tft.print("Empty chat");
        while (1)
            delay(100);
    }

    render_screen();

    while (1)
    {
        char key = keypadGetKey();

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
                    render_screen();
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
                    render_screen();
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
                        currentText[currentText.length() - 1] = letters[pressCount % numLetters];
                    }
                }
                else
                {
                    // New key or timeout → commit previous letter and start a new one
                    pressCount = 0;
                    const char *letters = t9map[key - '0'];
                    if (strlen(letters) > 0)
                        currentText += letters[0];
                }

                lastPressTime = now;
                lastKey = key;

                // render_text(currentText); // Your function to update display
            }
            if (key == '*')
            {
                if (currentText.length() > 0)
                {
                    currentText.remove(currentText.length() - 1);
                    // render_text(currentText);
                }
                lastKey = 0; // reset so it doesn't interfere with cycling
                pressCount = 0;
            }

            if (key == '0')
            {
                currentText += ' ';
                lastKey = 0;
                pressCount = 0;
            }

            // Send message
            if (key == '1')
            {
                sendHttpsPost(contact, currentText);
                fresh_chat_init(contact);
                currentText = "";
            }

            // Commit last character automatically after timeout
            if (lastKey && (millis() - lastPressTime) > multiTapTimeout)
            {
                lastKey = 0;
                pressCount = 0;
            }
        }

        tft.fillRect(0, 130, 128, 30, TFT_BLACK);
        if (type_mode)
            tft.drawRect(0, 130, 128, 30, TFT_BLUE);
        else
            tft.drawRect(0, 130, 128, 30, TFT_WHITE);

        tft.setCursor(2, 132);
        tft.print(currentText);

        delay(10);
        refetch_message_counter++;
        if (refetch_message_counter % 10000 == 0)
        {
            refetch_message_counter = 0;
            fresh_chat_init(contact);
        }
    }
}