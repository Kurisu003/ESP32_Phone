// HttpsUtils.h
#ifndef HTTPS_UTILS_H
#define HTTPS_UTILS_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "sim_shenanigans.h"

#include "variables.h"

// ---------------------------------------------------------------------------
// Put your server’s root CA certificate here (PEM format, keep the \n)
// ---------------------------------------------------------------------------
const char *root_ca = R"(-----BEGIN CERTIFICATE-----
MIIDljCCAn6gAwIBAgIUKAaBJpqw2FeSRnMCEpkvbLVJQakwDQYJKoZIhvcNAQEL
BQAwUjELMAkGA1UEBhMCVVMxDjAMBgNVBAgMBVN0YXRlMQ0wCwYDVQQHDARDaXR5
MQwwCgYDVQQKDANPcmcxFjAUBgNVBAMMDTE1NC4xNi4zNi4yMDEwHhcNMjUxMTAx
MjIzNzA5WhcNMjYxMTAxMjIzNzA5WjBSMQswCQYDVQQGEwJVUzEOMAwGA1UECAwF
U3RhdGUxDTALBgNVBAcMBENpdHkxDDAKBgNVBAoMA09yZzEWMBQGA1UEAwwNMTU0
LjE2LjM2LjIwMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN0gwPMv
W8PO59k0jnNADZ9H7pqS8ro1wddWmB2O2yT3vUUc+gXK9NHRNTJnoWLxt49jlw07
l9KGWEgsLRzOe8fBjeok0ykbRJpaWuYCcP7AB3dHBj1m6K3GNY9371bu+CGP5YLb
B6P1CzkatysbE94Vb90thEgxBCbFjvXpPRtPhcgE7moYHexKVocooUSJura79iLi
PCZmPPrtp0pU1Emj4wkVSP0a81J/hviAbDsMwNYnXy7b7wKG0YGdH3Ejt74FWFzC
SPIWlrFQ45cZE+Y6Mp63JC1qMp6GuybKUMQJZYHgTA2nsOGcWYSqthockcIJxmz3
f+eAoTRqM5nO0uECAwEAAaNkMGIwHQYDVR0OBBYEFHZxgpzob6s7lYW8+eAy+L40
xnvvMB8GA1UdIwQYMBaAFHZxgpzob6s7lYW8+eAy+L40xnvvMA8GA1UdEwEB/wQF
MAMBAf8wDwYDVR0RBAgwBocEmhAkyTANBgkqhkiG9w0BAQsFAAOCAQEAe4HPr/p2
xcXvgWdz1PYrk1qMXH4nYkV5PYUzwQIwbWKSXF4izBC6A5v0Scz8eTSfmpVZiMOt
qPlU9SmZfcm/oY5nM8Qu+IHCmAsbedvPLH6k685cFPM36cJm67ikVyoyRQblaexU
OzknAz2K71Hndr7uc6BVMJM39N3lGyjeX7O5WQPVF55GT7hATr85lElg70CiAwyx
G9z7b+CiHlbR8rVSzicYfNFFNmygKZHe8ZI6cnjRoMnhUIZLPeg3oLLgW6pBQVdA
xAC3QjXD+RcgN8bHBo9CBj9xM+FAOI9KA8v7vHUsbM+FcWCaE/T43aFWXH9IEOy/
atLBEGBzCzfY/A==
-----END CERTIFICATE-----)";

// ---------------------------------------------------------------------------
// GET request with custom X-API-Key header
// ---------------------------------------------------------------------------
String sendHttpsGet_wifi(const char *url)
{

    String response = "";
    WiFiClientSecure client;
    client.setInsecure();
    // client.setCACert(root_ca);
    HTTPClient https;

    Serial.printf("GET %s ... ", url);
    if (https.begin(client, url))
    {
        int httpCode = https.GET();

        if (httpCode > 0)
        {
            Serial.printf("OK [%d]\n", httpCode);
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {
                response = https.getString();
            }
        }
        else
        {
            Serial.printf("FAILED: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
    }
    else
    {
        Serial.println("Unable to connect");
    }
    return response;
}

//! Public
String sendHttpsGet(String url)
{
    //! Hardcoded sim for now
    // if (WiFi.status() != WL_CONNECTED)
    if (true)
    {
        return (send_http_sim(url));
    }

    return (sendHttpsGet_wifi(url.c_str()));
}

//! Public
String get_whatsapp_info(String option)
{
    String url = "https://" + String(BASE_IP) + ":" + String(BASE_PORT) + "/api/" + option;
    String response = sendHttpsGet(url);
    Serial.println(response);
    return response;
}

// ---------------------------------------------------------------------------
// POST JSON payload with contact/message and X-API-Key
// ---------------------------------------------------------------------------

String send_http_post_wifi(String url, String payload)
{
    WiFiClientSecure client;
    client.setInsecure();
    // client.setCACert(root_ca);
    HTTPClient https;

    const char *serverUrl = url.c_str();

    if (!https.begin(client, serverUrl))
    {
        return "HTTPS begin failed";
    }

    // https.addHeader("Content-Type", "application/json");
    // https.addHeader("X-API-Key", apiKey);

    int code = https.POST(payload);
    if (code > 0)
    {
        Serial.printf("HTTPS POST success, code: %d\n", code);
        return https.getString();
    }

    return ("HTTPS POST failed: %s\n", https.errorToString(code).c_str());
    // https.end();
    // how to end before returning?
}

String send_message_to_contact(const String &contact, const String &message)
{
    Serial.println(contact);
    Serial.println(message);
    String url = "https://" + String(BASE_IP) + ":" + String(BASE_PORT) + "/api/send_message_to_contact";

    String payload = "{\"content\":{\"contact\":\"" + contact + "\",\"message\":\"" + message + "\"}}";

    // if (WiFi.status() != WL_CONNECTED)
    //! Hardcoded to always use sim for now
    if (true)
        return send_http_post_sim(url, payload);

    return send_http_post_wifi(url, payload);
}

#endif // HTTPS_UTILS_H