#include "HttpsUtils.h"
#include "helpers.h"
#include "whatsapp_chat.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>  
#include <chrono>
#include <thread>
using namespace std;  // <-- So we can use vector<String>

extern TFT_eSPI tft;

// char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";
int selected_contact = 0;
vector<String> contacts;

void display_contact(int x, int index, const char* contact, int selected_contact, int page){
  // index used so the display draws correctly
  int index_for_draw = index-page*8;
  int y = (index_for_draw*20);
  if (index == selected_contact){
    tft.drawRect(x, y, 70, 18, TFT_BLUE);
  }
  else{
    tft.drawRect(x, y, 70, 18, TFT_WHITE);
  }

  char buffer[11];  // 10 chars + null terminator
  strncpy(buffer, contact, 10);
  buffer[10] = '\0';  // Force null-terminate

  tft.setCursor(x+5,y+5);
  tft.print(buffer);
}

void init_contacts(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);

  String response = sendHttpsGet("https://154.16.36.201:29793/api/get_contacts", encrypted_api_key);

  if (response.length() == 0) {
    tft.fillScreen(TFT_BLACK);

    tft.setCursor(0, 0);
    tft.print("No response");
    while(1){}
  }

  // vector<String> contacts = parseJsonArray(response);
  contacts = parseJsonArray(response);

  tft.fillScreen(TFT_BLACK);
}


void draw_contacts(){
  tft.fillScreen(TFT_BLACK);


  tft.setCursor(0, 0);

  if (contacts.empty()) {
    tft.print("No contacts found");
    return;
  }

  // Displays 8 contacts per page
  int page = selected_contact/8;
  for (size_t i = page * 8; i < min(contacts.size(), (size_t)8 + page * 8) ; i++) {
    display_contact(5, i, contacts[i].c_str(), selected_contact, page);
  }
}

void whatsapp_main() {
 
  init_contacts();
  draw_contacts();

  bool lastState = LOW;
  bool currentState;

  while(1) {
    currentState = digitalRead(13);

    // only redraw if something happened
    if (lastState == LOW && currentState == HIGH) {
      selected_contact = (selected_contact + 1) % contacts.size();
      draw_contacts();
    }
    lastState = currentState;

    if(digitalRead(27)){
      whatsapp_chat(contacts[selected_contact].c_str());
    }
  }
}