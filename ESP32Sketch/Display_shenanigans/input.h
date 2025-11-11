#include <Arduino.h>

// Pin definitions for ESP32
const int COL2 = 13;  // D13 -> Column 2
const int ROW1 = 12;  // D12 -> Row 1
const int COL1 = 14;  // D14 -> Column 1
const int ROW4 = 27;  // D27 -> Row 4
const int COL3 = 26;  // D26 -> Column 3
const int ROW3 = 25;  // D25 -> Row 3
const int ROW2 = 33;  // D33 -> Row 2

// Array of column pins (to drive low one at a time)
const int COL_PINS[3] = {COL1, COL2, COL3};

// Array of row pins (to read input)
const int ROW_PINS[4] = {ROW1, ROW2, ROW3, ROW4};

// Keymap: 4 rows x 3 columns = 12 keys
// Customize the characters as needed
const char KEYMAP[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

void keypadInit() {
  // Columns: output, default HIGH (inactive)
  pinMode(COL1, OUTPUT);
  pinMode(COL2, OUTPUT);
  pinMode(COL3, OUTPUT);
  digitalWrite(COL1, HIGH);
  digitalWrite(COL2, HIGH);
  digitalWrite(COL3, HIGH);

  // Rows: input with internal pull-up
  pinMode(ROW1, INPUT_PULLUP);
  pinMode(ROW2, INPUT_PULLUP);
  pinMode(ROW3, INPUT_PULLUP);
  pinMode(ROW4, INPUT_PULLUP);
}

char keypadGetKey() {
  // Configure columns as input with pull-up
  for (int c = 0; c < 3; c++) {
    pinMode(COL_PINS[c], INPUT_PULLUP);
  }

  char key = '\0';

  // Scan rows
  for (int r = 0; r < 4; r++) {
    // Set all rows to HIGH (inactive)
    for (int i = 0; i < 4; i++) {
      pinMode(ROW_PINS[i], OUTPUT);
      digitalWrite(ROW_PINS[i], HIGH);
    }

    // Drive only the current row LOW
    digitalWrite(ROW_PINS[r], LOW);

    // Check each column
    for (int c = 0; c < 3; c++) {
      if (digitalRead(COL_PINS[c]) == LOW) {
        delay(20); // debounce
        while (digitalRead(COL_PINS[c]) == LOW); // wait for release
        key = KEYMAP[r][c];
        break;
      }
    }
    if (key != '\0') break;
  }

  // Optional: reset pins to safe state
  for (int r = 0; r < 4; r++) {
    pinMode(ROW_PINS[r], INPUT_PULLUP);
  }
  for (int c = 0; c < 3; c++) {
    pinMode(COL_PINS[c], INPUT_PULLUP);
  }

  return key;
}