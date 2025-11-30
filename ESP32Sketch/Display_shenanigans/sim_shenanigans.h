#include <Arduino.h>
#include "variables.h"
#define RX_PIN 17
#define TX_PIN 16
HardwareSerial modem(2);


void sim_init() {
 
  delay(1000);
  Serial.println("Serial connection ready");
 
  modem.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  modem.println("AT");
  delay(1000);

  modem.println("AT+HTTPINIT"); // HTTP-Service des Modems starten
}
 
String sim_sendHttpsGet(String url) {

    // Set the URL
    modem.println("AT+HTTPPARA=\"URL\",\"" + url + "\"");

    // Set custom header
    modem.printf("AT+HTTPPARA=\"USERDATA\",\"X-API-Key: %s\"\r\n", encrypted_api_key);

    // Start the GET request
    modem.println("AT+HTTPACTION=0"); // 0 = GET

    // Wait for completion
    String response = "";
    bool done = false;

    while (!done) {
        if (modem.available()) {
            String line = modem.readStringUntil('\n');
            line.trim();
            if (line.startsWith("+HTTPACTION:")) {
                int firstComma = line.indexOf(',');
                int secondComma = line.indexOf(',', firstComma + 1);
                int status = line.substring(firstComma + 1, secondComma).toInt();
                int dataLen = line.substring(secondComma + 1).toInt();

                Serial.println("HTTP status: " + String(status));
                Serial.println("Data length: " + String(dataLen));

                done = true;

                if (dataLen > 0) {
                    modem.println("AT+HTTPREAD=0," + String(dataLen));
                    unsigned long timeout = millis() + 5000;
                    while (millis() < timeout) {
                        if (modem.available()) {
                            response += modem.readString();
                        }
                    }
                }
            }
        }
    }

    return response;
}

String sim_sendHttpsPost(const char* url, const char* json) {

    // Initialize HTTP
    modem.println("AT+HTTPINIT");
    modem.printf("AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
    modem.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

    // Set custom header
    modem.printf("AT+HTTPPARA=\"USERDATA\",\"X-API-Key: %s\"\r\n", encrypted_api_key);

    // Send data length and wait for DOWNLOAD prompt
    modem.printf("AT+HTTPDATA=%d,5000\r\n", strlen(json));
    unsigned long start = millis();
    while (millis() - start < 5000) {
        if (modem.available()) {
            String line = modem.readStringUntil('\n');
            line.trim();
            if (line == "DOWNLOAD") {
                modem.print(json);  // Send JSON payload
                break;
            }
        }
    }

    // Wait for OK after sending JSON
    start = millis();
    bool okReceived = false;
    while (millis() - start < 5000) {
        if (modem.available()) {
            String line = modem.readStringUntil('\n');
            line.trim();
            if (line == "OK") {
                okReceived = true;
                break;
            }
        }
    }
    if (!okReceived) return "ERROR";

    // Trigger POST
    modem.println("AT+HTTPACTION=1"); // 1 = POST

    // Wait for +HTTPACTION URC
    start = millis();
    while (millis() - start < 15000) {
        if (modem.available()) {
            String line = modem.readStringUntil('\n');
            line.trim();
            if (line.startsWith("+HTTPACTION:")) {
                return line;  // Returns "+HTTPACTION: 1,200,45"
            }
        }
    }

    return "ERROR";  // Timeout or failure
}

// void loop() {
//   while (modem.available()) {
//     String resp = modem.readStringUntil('\n');
//     resp.trim();
//     if (resp.length() > 0) {
//       Serial.println("Modem: " + resp);
//     }
//   }
 
//   static unsigned long lastRun = millis() - 18000;
//   if (millis() - lastRun > 20000) {
//     lastRun = millis();
 
//     modem.println("AT+HTTPINIT"); // HTTP-Service des Modems starten

//     String body = sendHttpsGet("https://45.88.97.219:21814/api/get_contacts");
//     Serial.println("Response body:\n" + body);


//     String result = sendHttpsPost("https://45.88.97.219:21814/api/send_message_to_contact", "{\"content\":{\"contact\":\"Elmar\",\"message\":\"Hallo\"}}");
//     Serial.println("POST result: " + result);

//     modem.println("AT+HTTPREAD=99999"); // "Unsaubere Implementierung": Eigentlich AT+HTTPREAD? nutzen um Länge der Antwort zu ermitteln
//     modem.println("AT+HTTPTERM"); // HTTP-Service nach Verwendung stoppen
//   }
// }