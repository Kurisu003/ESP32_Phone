#include "HttpsUtils.h"
#include "helpers.h"
#include "input.h"
#include "display.h"
#include "whatsapp_chat.h"
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>
#include <chrono>
#include <thread>

#include "variables.h"

// char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";
int selected_contact = 0;
std::vector<String> contacts;
std::vector<String> unread_contacts;

void init_contacts()
{
  display_simple_text("Get request for contacts sent");
  String response = get_whatsapp_info("get_contacts");
  contacts = parseJsonArray(response);

  response = "";
  response = get_whatsapp_info("get_unreads");
  unread_contacts = parseJsonArray(response);

  printf("Contacts jsoned\n");
  display_simple_text("Contacts Jsoned");
}

void fetch_and_disp_contacts()
{
  display_simple_text("Getting contacts...");

  init_contacts();
  printf("Contacts init\n");
  display_simple_text("Contacts Initialized");

  draw_contacts(contacts, unread_contacts, selected_contact);
}

void whatsapp_main()
{
  printf("Whatsapp Main\n");
  display_simple_text("Whatsapp Main");

  keypad_init();
  printf("Keypad init\n");
  display_simple_text("Keypad Init");

  fetch_and_disp_contacts();

  int last_contact = 0;

  while (1)
  {
    char key = keypad_get_key();

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
      display_simple_text("Getting messages...");
      whatsapp_chat_main(contacts[selected_contact].c_str());
      // after exiting from chat with person, it should be reinitialized
      fetch_and_disp_contacts();
    }
    else if (key == '*')
      return;

    selected_contact %= contacts.size();

    // only redraw if something happened
    if (last_contact != selected_contact)
    {
      draw_contacts(contacts, unread_contacts, selected_contact);
    }

    last_contact = selected_contact;
  }
}