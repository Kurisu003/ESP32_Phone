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

  // app_selection_main();
  // TODO: Look through all saved passwords and try to connect
  // TODO: Use wifi for https if availible
  append_wifi("Wi-Find You Cute 2.4 GHz\nMinecraft123!");

  Serial.println("Stored WiFi credentials:");

  
}

void loop()
{
}
