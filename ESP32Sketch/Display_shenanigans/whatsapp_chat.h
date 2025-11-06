#include "HttpsUtils.h"
#include "helpers.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>  
#include <chrono>
#include <thread>
using namespace std;  // <-- So we can use vector<String>

extern TFT_eSPI tft;

// char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";
vector<String> chat_messages;

void init_chat(const char* contact){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);

  String url = String("https://154.16.36.201:29793/api/messages_from_contact/") + contact;
  String response = sendHttpsGet(url.c_str(), encrypted_api_key);
  chat_messages = parseJsonArray(response);

  if (response.length() == 0) {
    tft.fillScreen(TFT_BLACK);

    tft.setCursor(0, 0);
    tft.print("No response");
    return;
  }

  tft.setCursor(5,5);

  // vector<String> contacts = parseJsonArray(response);
  // contacts = parseJsonArray(response);

  // tft.fillScreen(TFT_BLACK);
}

String wrapText(String text, int maxWidth) {
  if (text.length() == 0) return text;

  String result = "";
  String line = "";
  int spaceLeft = maxWidth;

  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);

    if (c == ' ' || c == '\n') {
      // Look ahead to find the full word
      int wordEnd = i + 1;
      while (wordEnd < text.length() && text.charAt(wordEnd) != ' ' && text.charAt(wordEnd) != '\n') {
        wordEnd++;
      }
      String word = text.substring(i + 1, wordEnd);

      int wordLen = word.length();
      if (wordLen > maxWidth) {
        // Word too long: force split
        if (line.length() > 0) {
          result += line + "\n";
          line = "";
          spaceLeft = maxWidth;
        }
        result += word + "\n";
        i = wordEnd - 1;
        spaceLeft = maxWidth;
      } else if (wordLen > spaceLeft) {
        // Start new line
        if (line.length() > 0) {
          result += line + "\n";
          line = word;
          spaceLeft = maxWidth - wordLen;
        } else {
          line = word;
          spaceLeft = maxWidth - wordLen;
        }
      } else {
        // Add word to current line
        if (line.length() > 0) {
          line += " ";
          spaceLeft--;
        }
        line += word;
        spaceLeft -= wordLen;
      }
      i = wordEnd - 1;
    }
  }

  if (line.length() > 0) {
    result += line;
  }

  return result;
}

void whatsapp_chat(const char* contact) {
 
  init_chat(contact);

  const unsigned long DRAW_INTERVAL = 250;
  unsigned long lastDrawTime = 0;

  bool lastState = LOW;
  bool currentState;
  int max_char_width = 16;


  int i = chat_messages.size()-5;
  String sender = parseJsonArray(chat_messages[i])[0];
  String actual_message = parseJsonArray(chat_messages[i])[1];
  String wrapped_text = wrapText(actual_message, max_char_width);
  String temp = wrapped_text;  // Make a copy

  int newlineCount = 0;
  for (size_t i = 0; i < temp.length(); ++i) {
      if (temp[i] == '\n') {
          newlineCount++;
      }
  }
  int lineCount = newlineCount + 1;

  tft.setCursor(0, 150-7*lineCount);
  tft.print(wrapped_text);

  
  while(1) {
    
  }
}