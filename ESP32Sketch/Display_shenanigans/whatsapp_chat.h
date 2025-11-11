#include "HttpsUtils.h"
#include "helpers.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>
using namespace std;

extern TFT_eSPI tft;
// char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";

#define PIN_UP    25
#define PIN_DOWN  33
#define DEBOUNCE  50  // ms

struct ChatLine {
    String text;
    uint16_t color;
};

std::vector<ChatLine> all_lines;
int scroll_pos = 0;
unsigned long last_up = 0, last_down = 0;

void init_chat(const char* contact) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);

    String url = "https://154.16.36.201:40837/api/messages_from_contact/" + String(contact);
    String response = sendHttpsGet(url.c_str(), encrypted_api_key);

    if (response.length() == 0) {
        tft.setCursor(0, 0);
        tft.print("No response");
        return;
    }

    StaticJsonDocument<6144> doc;
    DeserializationError error = deserializeJson(doc, response, DeserializationOption::NestingLimit(6));
    if (error) {
        tft.setCursor(0, 0);
        tft.print("JSON fail");
        return;
    }

    all_lines.clear();
    all_lines.reserve(80);

    for (JsonArray msg : doc.as<JsonArray>()) {
        const char* sender = msg[0] | "";
        const char* message = msg[1] | "";
        if (strlen(message) == 0) continue;

        uint16_t color = (String(sender) == "You") ? TFT_GREEN : TFT_CYAN;
        String full = String(message);
        int pos = 0;
        bool first = true;

        while (pos < full.length()) {
            int next = full.indexOf('\n', pos);
            if (next == -1) next = full.length();
            String line = full.substring(pos, next);
            String out = first ? String(sender) + ": " + line : "    " + line;
            all_lines.push_back({out, color});
            first = false;
            pos = next + 1;
        }
    }

    scroll_pos = max(0, (int)all_lines.size() - 20);  // Start at bottom
}

void render_screen() {
    tft.fillScreen(TFT_BLACK);
    int y = 0;
    const int line_h = 10;
    int max_lines = tft.height() / line_h;

    for (int i = 0; i < max_lines && scroll_pos + i < all_lines.size(); i++) {
        const auto& line = all_lines[scroll_pos + i];
        tft.setTextColor(line.color);
        tft.setCursor(0, y);
        tft.print(line.text);
        y += line_h;
    }
}

void whatsapp_chat(const char* contact) {
    pinMode(PIN_UP, INPUT_PULLDOWN);
    pinMode(PIN_DOWN, INPUT_PULLDOWN);

    init_chat(contact);

    if (all_lines.empty()) {
        tft.setCursor(0, 0);
        tft.print("Empty chat");
        while (1) delay(100);
    }

    render_screen();

    while (1) {
        unsigned long now = millis();

        // Scroll UP (show older messages)
        if (digitalRead(PIN_UP) == HIGH && now - last_up > DEBOUNCE) {
            last_up = now;
            if (scroll_pos > 0) {
                scroll_pos--;
                render_screen();
            }
        }

        // Scroll DOWN (show newer messages)
        if (digitalRead(PIN_DOWN) == HIGH && now - last_down > DEBOUNCE) {
            last_down = now;
            int max_scroll = max(0, (int)all_lines.size() - (tft.height() / 10));
            if (scroll_pos < max_scroll) {
                scroll_pos++;
                render_screen();
            }
        }

        delay(10);
    }
}