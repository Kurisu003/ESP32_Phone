#include "HttpsUtils.h"
#include "helpers.h"
#include "input.h"
#include "whatsapp_chat.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>
#include <chrono>
#include <thread>

#include "variables.h"
using namespace std; // <-- So we can use vector<String>

extern TFT_eSPI tft;

// char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";
int selected_contact = 0;
vector<String> contacts;
vector<String> unread_contacts;

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

void fill_screen_black()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
}

void init_contacts()
{
  fill_screen_black();
  String response = get_whatsapp_info("get_contacts");
  contacts = parseJsonArray(response);

  response = "";
  response = get_whatsapp_info("get_unreads");
  unread_contacts = parseJsonArray(response);

  printf("Contacts jsoned\n");
  tft.fillScreen(TFT_RED);
}

void draw_contacts()
{
  Serial.println("Color changed");
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

void init_disp_contacts()
{
  init_contacts();
  printf("Contacts init\n");
  draw_contacts();
}

void whatsapp_main()
{
  printf("Whatsapp Main\n");

  keypadInit();
  printf("Keypad init\n");
  init_disp_contacts();

  int last_contact = 0;

  while (1)
  {
    char key = keypadGetKey();

    if (key == '2')
      selected_contact--;
    else if (key == 56)
      selected_contact++;
    else if (key == '6')
      selected_contact += 8;
    else if (key == '4')
      selected_contact -= 8;
    else if (key == '5')
    {
      whatsapp_chat(contacts[selected_contact].c_str());
      init_disp_contacts();
    }

    selected_contact %= contacts.size();

    // only redraw if something happened
    if (last_contact != selected_contact)
    {
      draw_contacts();
    }

    last_contact = selected_contact;
  }
}