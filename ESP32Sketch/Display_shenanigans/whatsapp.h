#include "HttpsUtils.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>  // <-- REQUIRED
using namespace std;  // <-- So we can use vector<String>

extern TFT_eSPI tft;

const char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";

// Returns vector<String> — always safe (empty on error)
vector<String> parseJsonArray(const String& jsonString) {
  vector<String> result;

  // Safety: empty input
  if (jsonString.length() == 0) {
    Serial.println("Empty JSON string");
    return result;
  }

  // Estimate size: 10 items max for now
  const size_t capacity = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    Serial.print("JSON parse failed: ");
    Serial.println(error.c_str());
    return result;  // return empty
  }

  if (!doc.is<JsonArray>()) {
    Serial.println("JSON is not an array");
    return result;
  }

  JsonArray array = doc.as<JsonArray>();
  for (JsonVariant value : array) {
    result.push_back(value.as<String>());
  }

  Serial.printf("Parsed %d contacts\n", result.size());
  return result;
}

void display_contact(int x, int index, const char* contact){
  int y = (index*20);
  tft.drawRect(x, y, 70, 18, TFT_WHITE);

  char buffer[11];  // 10 chars + null terminator
  strncpy(buffer, contact, 10);
  buffer[10] = '\0';  // Force null-terminate

  tft.setCursor(x+5,y+5);
  tft.print(buffer);
}

void whatsapp_main() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);

  String response = sendHttpsGet("https://154.16.36.201:36596/api/get_contacts", encrypted_api_key);

  if (response.length() == 0) {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.print("No response");
    return;
  }

  vector<String> contacts = parseJsonArray(response);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);

  if (contacts.empty()) {
    tft.print("No contacts found");
    return;
  }

  // Displays 8 contacts per page
  // TODO: Implement selected contact
  int page = 1;
  for (size_t i = page * 8; i < min(contacts.size(), (size_t)8 + page * 8) ; i++) {
    display_contact(5, i-page*8, contacts[i].c_str());
    // tft.printf("%d: %s\n", i + 1, contacts[i].c_str());
  }
}