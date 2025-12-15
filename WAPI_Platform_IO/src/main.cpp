#include "User_Setup.h"

#include "display.h"
#include "whatsapp_contacts.h"
#include "variables.h"
#include "wifi_self.h"
#include <SPI.h>

int selected_app = 0;

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

  char *ssid = "Wi-Find You Cute 2.4 GHz";
  char *password = "Minecraft123!";
  conenct_to_wifi(ssid, password);

  // whatsapp_main();
  whatsapp_chat_main("Elmar");
}

void draw_apps_loop()
{
  for (int i = 0; i < my_apps_length; i++)
  {
    drawApp(my_apps[i], 5, i, i == selected_app);
  }
}

void loop()
{
  // selected_app += 1;
  // selected_app %= my_apps_length;
  // draw_apps_loop();
  // delay(500);
}
