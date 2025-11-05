#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "whatsapp_contacts.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

int selected_app = 0;

const String my_apps[] = {
  "WhatsApp",
  "Calculator",
  "Hotspot",
  "Settings",
};

int my_apps_length = sizeof(my_apps)/sizeof(my_apps[0]);


void setup(void) {
  Serial.begin(9600);

  const char* ssid     = "Wi-Find You Cute 2.4 GHz";
  const char* password = "Minecraft123!";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.setCursor(0, 0);
    tft.print("Connecting to WiFi...");
  }
  

  // Use this initializer if you're using a 1.8" TFT
  tft.init();   // initialize a ST7735S chip
  tft.invertDisplay(0);

  tft.fillScreen(TFT_BLACK);
  // tft.height(), etc, wroks
  // fill versions also availible
  // x, y, w, h
  // tft.drawRect(0, 0, 10, 10, TFT_WHITE);
  // tft.drawLine(10, 10, 20, 20, TFT_WHITE);

  // tft.setCursor(40, 40);
  // tft.setTextColor(TFT_WHITE);
  // tft.setTextWrap(true);
  
  // x, y
  // tft.setCursor(0, 30);
  // tft.print("Penis");

  // tft.drawCircle(60, 60, 10, TFT_WHITE);

  // tft.drawTriangle(x_1, y_1, x_2, y_2, x_3, y_3, color);
  // tft.fillTriangle(0, 0, 100, 0, 50, 50, TFT_BLUE);

  // tft.drawRoundRect(x, y, w, h, 5, color);
  whatsapp_main();
}

void drawApp(String name, int x, int index, bool is_selected){
  int y = (index*20)+5;
  if (is_selected){
    tft.drawRect(x, y, 70, 18, TFT_BLUE);
  }
  else{
    tft.drawRect(x, y, 70, 18, TFT_WHITE);
  }

  tft.setCursor(x+5,y+5);
  tft.print(name);
}

void draw_apps_loop(){
  for(int i = 0; i < my_apps_length; i++){
    drawApp(my_apps[i], 5, i, i == selected_app);
  }
}

void loop() {
  // selected_app += 1;
  // selected_app %= my_apps_length;
  // draw_apps_loop();
  // delay(500);
}

