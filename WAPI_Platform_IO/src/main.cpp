#include "User_Setup.h"

#include "display.h"
#include "variables.h"
#include "wifi_self.h"
#include "app_selection.h"
#include "storage.h"
#include <SPI.h>

void setup(void)
{
  Serial.begin(115200);
  screen_init();
  keypad_init();
  storage_init();

  // Clear wifi:
  write_to_file("/wifi.txt", "");

  append_wifi("Wi-Find You Cute 2.4 GHz\nMinecraft123!");
  append_wifi("UwU\n121345678");
  append_wifi("FRITZ!Box 4020 YR\nBraunhofer51252");


  // Serial.println(wifi_connected);
  // Serial.println("Stored WiFi credentials:");
  app_selection_main();
}

void loop()
{
}
