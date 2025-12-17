#include <Arduino.h>
#include "variables.h"

/* =========================
   Pin definitions (ESP32)
   ========================= */

#define Pin_1 33
#define Pin_2 25
#define Pin_3 26
#define Pin_4 27
#define Pin_5 14
#define Pin_6 12
#define Pin_7 13

const int ROW3 = Pin_1;
const int ROW2 = Pin_2;
const int COL1 = Pin_3;
const int ROW1 = Pin_4;
const int COL3 = Pin_5;
const int ROW4 = Pin_6;
const int COL2 = Pin_7;

/* =========================
   Keypad configuration
   ========================= */

static const int COL_PINS[3] = {COL1, COL2, COL3};
static const int ROW_PINS[4] = {ROW1, ROW2, ROW3, ROW4};

static const char KEYMAP[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

/* =========================
   Initialization
   ========================= */

void keypad_init()
{
  /* Columns: outputs, idle HIGH */
  for (int c = 0; c < 3; c++)
  {
    pinMode(COL_PINS[c], OUTPUT);
    digitalWrite(COL_PINS[c], HIGH);
  }

  /* Rows: inputs with pull-ups */
  for (int r = 0; r < 4; r++)
  {
    pinMode(ROW_PINS[r], INPUT_PULLUP);
  }
}

/* =========================
   Key scan
   ========================= */

char keypad_get_key()
{
  static uint32_t lastScanTime = 0;
  static uint32_t lastSendTime = 0;
  static char lastSentKey = '\0';

  /* Scan debounce */
  if (millis() - lastScanTime < 30)
  {
    return '\0';
  }

  lastScanTime = millis();

  for (int c = 0; c < 3; c++)
  {
    /* Activate column */
    digitalWrite(COL_PINS[c], LOW);

    for (int r = 0; r < 4; r++)
    {
      if (digitalRead(ROW_PINS[r]) == LOW)
      {
        char key = KEYMAP[r][c];
        uint32_t now = millis();

        /* Suppress same key within 200 ms */
        if (key == lastSentKey && (now - lastSendTime) < 200)
        {
          digitalWrite(COL_PINS[c], HIGH);
          return '\0';
        }

        /* Accept key */
        lastSentKey = key;
        lastSendTime = now;

        digitalWrite(COL_PINS[c], HIGH);
        return key;
      }
    }

    /* Deactivate column */
    digitalWrite(COL_PINS[c], HIGH);
  }

  return '\0';
}
