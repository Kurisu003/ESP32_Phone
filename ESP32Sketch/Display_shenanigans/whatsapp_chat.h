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

  String url = String("https://154.16.36.201:36596/api/messages_from_contact/") + contact;
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



void whatsapp_chat(const char* contact) {
 
  init_chat(contact);

  const unsigned long DRAW_INTERVAL = 250;
  unsigned long lastDrawTime = 0;

  bool lastState = LOW;
  bool currentState;


  String sender = parseJsonArray(chat_messages[0])[0];
  String actual_message = parseJsonArray(chat_messages[0])[1];
  tft.setCursor(0, 0);
  tft.print(sender);
  tft.setCursor(0, 10);
  tft.print(actual_message);


  while(1) {
    
  }
}