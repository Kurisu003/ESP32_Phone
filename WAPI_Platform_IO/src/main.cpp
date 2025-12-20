#include "User_Setup.h"

#include "display.h"
#include "variables.h"
#include "wifi_self.h"
#include "app_selection.h"
#include "storage.h"
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
  storage_init();

  // char *ssid = "Wi-Find You Cute 2.4 GHz";
  // char *password = "Minecraft123!";
  // connect_to_wifi(ssid, password);

  // app_selection_main();
  append_to_file("/test.txt", "Hallo Welt!");
  append_to_file("/test.txt", "Hallo Welt!");
  Serial.println(read_from_file("/test.txt"));
}

void loop()
{
}
