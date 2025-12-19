#include "User_Setup.h"

#include "display.h"
#include "variables.h"
#include "wifi_self.h"
#include "app_selection.h"
#include <SPI.h>

const String my_apps[] = {
    "WhatsApp",
    "Calculator",
    "Hotspot",
    "Settings",
};

int my_apps_length = sizeof(my_apps) / sizeof(my_apps[0]);

void setup(void)
{
  Serial.begin(115200);
  screen_init();
  keypad_init();

  char *ssid = "Wi-Find You Cute 2.4 GHz";
  char *password = "Minecraft123!";
  connect_to_wifi(ssid, password);

  // whatsapp_main();
  // keypadInit();
  app_selection_main();

  // whatsapp_chat_main("Elmar");
}

void loop()
{
  selected_app += 1;
  selected_app %= my_apps_length;
  // draw_all_apps();
  delay(500);
}
