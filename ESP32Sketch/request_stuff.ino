#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "HttpsUtils.h"

// ---------- Wi-Fi ----------
const char* ssid     = "Wi-Find You Cute 2.4 GHz";
const char* password = "Minecraft123!";

// ---------- HTTPS endpoint ----------
const char* serverUrl = "https://154.16.36.201:36596/api/get_contacts";   // <-- change path if needed
const char* getRequest = "https://154.16.36.201:36596/api/get_contacts";   // <-- change path if needed

// ---------- Root-CA certificate (the one you gave) ----------
// const char* root_ca =
// "-----BEGIN CERTIFICATE-----\n"
// "MIIDljCCAn6gAwIBAgIUKAaBJpqw2FeSRnMCEpkvbLVJQakwDQYJKoZIhvcNAQEL\n"
// "BQAwUjELMAkGA1UEBhMCVVMxDjAMBgNVBAgMBVN0YXRlMQ0wCwYDVQQHDARDaXR5\n"
// "MQwwCgYDVQQKDANPcmcxFjAUBgNVBAMMDTE1NC4xNi4zNi4yMDEwHhcNMjUxMTAx\n"
// "MjIzNzA5WhcNMjYxMTAxMjIzNzA5WjBSMQswCQYDVQQGEwJVUzEOMAwGA1UECAwF\n"
// "U3RhdGUxDTALBgNVBAcMBENpdHkxDDAKBgNVBAoMA09yZzEWMBQGA1UEAwwNMTU0\n"
// "LjE2LjM2LjIwMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN0gwPMv\n"
// "W8PO59k0jnNADZ9H7pqS8ro1wddWmB2O2yT3vUUc+gXK9NHRNTJnoWLxt49jlw07\n"
// "l9KGWEgsLRzOe8fBjeok0ykbRJpaWuYCcP7AB3dHBj1m6K3GNY9371bu+CGP5YLb\n"
// "B6P1CzkatysbE94Vb90thEgxBCbFjvXpPRtPhcgE7moYHexKVocooUSJura79iLi\n"
// "PCZmPPrtp0pU1Emj4wkVSP0a81J/hviAbDsMwNYnXy7b7wKG0YGdH3Ejt74FWFzC\n"
// "SPIWlrFQ45cZE+Y6Mp63JC1qMp6GuybKUMQJZYHgTA2nsOGcWYSqthockcIJxmz3\n"
// "f+eAoTRqM5nO0uECAwEAAaNkMGIwHQYDVR0OBBYEFHZxgpzob6s7lYW8+eAy+L40\n"
// "xnvvMB8GA1UdIwQYMBaAFHZxgpzob6s7lYW8+eAy+L40xnvvMA8GA1UdEwEB/wQF\n"
// "MAMBAf8wDwYDVR0RBAgwBocEmhAkyTANBgkqhkiG9w0BAQsFAAOCAQEAe4HPr/p2\n"
// "xcXvgWdz1PYrk1qMXH4nYkV5PYUzwQIwbWKSXF4izBC6A5v0Scz8eTSfmpVZiMOt\n"
// "qPlU9SmZfcm/oY5nM8Qu+IHCmAsbedvPLH6k685cFPM36cJm67ikVyoyRQblaexU\n"
// "OzknAz2K71Hndr7uc6BVMJM39N3lGyjeX7O5WQPVF55GT7hATr85lElg70CiAwyx\n"
// "G9z7b+CiHlbR8rVSzicYfNFFNmygKZHe8ZI6cnjRoMnhUIZLPeg3oLLgW6pBQVdA\n"
// "xAC3QjXD+RcgN8bHBo9CBj9xM+FAOI9KA8v7vHUsbM+FcWCaE/T43aFWXH9IEOy/\n"
// "atLBEGBzCzfY/A==\n"
// "-----END CERTIFICATE-----";

void setup() {
  Serial.begin(115200);
  // delay(1000);
  const char* encrypted_api_key = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";
  const String encrypted_api_key_str = "6Y/2RcOyPX3cgpPY3BFvfXs/amLBS9Mgue/9Os8=";

  // // ---- Connect to Wi-Fi ----
  // Serial.printf("\nConnecting to %s ", ssid);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("\nWiFi connected, IP: " + WiFi.localIP().toString());

  // // ---- Send HTTPS POST ----
  Serial.println("\nAnswer:" + sendHttpsGet(getRequest, encrypted_api_key));


  String CONTACT = "Test";
  String MESSAGE = "Hello from ESP32!";

  sendHttpsPost(CONTACT, MESSAGE, encrypted_api_key_str);
}

void loop() {
  // Uncomment to repeat every 10 s
  // sendHttpsPost();
  // delay(10000);
}

// String sendHttpsGet(const char* url, const char* api_key) {
//   String response = "";
//   if (WiFi.status() != WL_CONNECTED) {
//     Serial.println("WiFi not connected");
//     return response;
//   }

//   WiFiClientSecure client;
//   client.setCACert(root_ca);  // Your custom certificate

//   HTTPClient https;
//   Serial.printf("GET %s ... ", url);

//   if (https.begin(client, url)) {
//     // Add API key header
//     https.addHeader("X-API-Key", api_key);

//     int httpCode = https.GET();

//     if (httpCode > 0) {
//       Serial.printf("OK [%d]\n", httpCode);
//       if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//         response = https.getString();
//       }
//     } else {
//       Serial.printf("FAILED: %s\n", https.errorToString(httpCode).c_str());
//     }
//     https.end();
//   } else {
//     Serial.println("Unable to connect");
//   }

//   return response;
// }


// void sendHttpsPost(String contact, String message, String apiKey) {
//   if (WiFi.status() != WL_CONNECTED) {  // Fixed: WL_CONNECTED, not WL-connected
//     Serial.println("WiFi lost");
//     return;
//   }

//   WiFiClientSecure client;
//   client.setCACert(root_ca);  // Make sure root_ca is defined

//   HTTPClient https;
//   const char* serverUrl = "https://154.16.36.201:36596/api/send_message_to_contact";

//   if (!https.begin(client, serverUrl)) {
//     Serial.println("HTTPS begin failed");
//     return;
//   }

//   https.addHeader("Content-Type", "application/json");
//   https.addHeader("X-API-Key", apiKey);

//   // Optional: Generate timestamp dynamically (requires <TimeLib.h> or RTC)
//   char timestamp[25];
//   snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:00:00Z", 2025, 11, 4, 12); // Example

//   String payload = "{\"role\": \"user\", \"content\": {\"contact\": \"" + contact +
//                    "\", \"message\": \"" + message +
//                    "\"}, \"metadata\": {\"source\": \"arduino\", \"timestamp\": \"" +
//                    String(timestamp) + "\"}}";

//   int code = https.POST(payload);

//   if (code > 0) {
//     Serial.printf("HTTPS POST success, code: %d\n", code);
//     Serial.println("Response: " + https.getString());
//   } else {
//     Serial.printf("HTTPS POST failed: %s\n", https.errorToString(code).c_str());
//   }

//   https.end();
// }