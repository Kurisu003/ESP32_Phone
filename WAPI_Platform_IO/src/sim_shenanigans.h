#include <Arduino.h>
#include "variables.h"
#define RX_PIN 17
#define TX_PIN 16
#define HTTP_DELAY 200
bool sim_is_initialized = false;
HardwareSerial modem(2);

//! PRIVATE
String waitForURC(String token, unsigned long timeoutMs = 10000)
{
    unsigned long start = millis();
    String line;

    while (millis() - start < timeoutMs)
    {
        if (modem.available())
        {
            line = modem.readStringUntil('\n');
            line.trim();

            if (line.length())
                Serial.println("URC: " + line);

            if (line.startsWith(token))
                return line;
        }
    }

    Serial.println("Timeout waiting for URC: " + String(token));
    return "";
}

//! PRIVATE
String read_module()
{
    String resp = "";
    while (modem.available())
    {
        resp += modem.readStringUntil('\n');
        resp.trim();
        if (resp.length() > 0)
        {
            Serial.println("Modem: " + resp);
        }
    }
    return resp;
}

//! PRIVATE
void send_command(String command)
{
    command.trim();
    if (command.length() > 0)
    {
        modem.print(command + "\r"); // Use \r for modem commands
        Serial.println("Sent: " + command);
    }
}

//! PRIVATE
String send_and_wait(String command, String token_to_wait_for)
{
    send_command(command);
    return waitForURC(token_to_wait_for);
}

//! PRIVATE
bool wait_for_ip()
{
    String ip = "0.0.0.0";
    unsigned long start = millis();

    while (ip == "0.0.0.0")
    {
        String resp = send_and_wait("AT+CGPADDR=1", "+CGPADDR");

        if (resp.length() > 0)
            ip = resp;

        // Timeout after 30 seconds
        if (millis() - start > 30000)
        {
            Serial.println("Timeout waiting for IP address");
            return false;
        }

        delay(500); // optional: avoid hammering the modem
    }

    Serial.println("IP acquired: " + ip);
    return true;
}

//! PRIVATE
bool sim_init()
{
    Serial.begin(115200);

    delay(1000);
    Serial.println("Serial connection ready");

    modem.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    send_and_wait("AT", "OK");
    send_and_wait("ATE0", "OK");
    bool has_ip = wait_for_ip();
    sim_is_initialized = has_ip;
    return has_ip;
}

//! PRIVATE
String http_read_content()
{
    // Request the full payload
    modem.print("AT+HTTPREAD=0,99999\r");

    String content = "";
    bool inPayload = false;
    unsigned long start = millis();

    while (millis() - start < 2000) // 5 s timeout
    {
        if (!modem.available())
            continue;

        String line = modem.readStringUntil('\n');
        line.trim();

        // Skip empty noise
        if (line.length() == 0)
            continue;

        // Detect header: +HTTPREAD: <number>
        if (line.startsWith("+HTTPREAD:"))
        {
            inPayload = true;
            continue; // skip header line
        }

        // Detect footer: +HTTPREAD: <number>  or OK / ERROR
        if (inPayload && line.startsWith("+HTTPREAD:"))
        {
            break; // second header/footer → end of payload
        }

        // Store actual payload
        if (inPayload)
        {
            content += line;
            content += "\n";
        }
    }

    return content;
}

//! PUBLIC
String send_http_sim(String address)
{
    if (!sim_is_initialized)
    {
        bool could_init_sim = sim_init();
        if (!could_init_sim)
            return "FAILED";
    }
    // flush output
    read_module();
    delay(HTTP_DELAY);

    send_command("AT+HTTPTERM"); // Might return error, but just to make sure, waits becaus if it does return ok, it will overwrite other command
    read_module();
    delay(HTTP_DELAY);

    send_and_wait("AT+HTTPINIT", "OK");
    delay(HTTP_DELAY);

    String command = "AT+HTTPPARA=\"URL\",\"" + address + "\"";
    send_and_wait(command, "OK");
    delay(HTTP_DELAY);

    String header =
        String("AT+HTTPPARA=\"USERDATA\",\"X-API-Key: ") +
        String(encrypted_api_key) +
        "\\r\\n\"";
    send_and_wait(header, "OK");
    delay(HTTP_DELAY);

    send_and_wait("AT+HTTPACTION=0", "+HTTPACTION");
    delay(HTTP_DELAY);

    String response = http_read_content();
    // Serial.println(response);

    send_and_wait("AT+HTTPTERM", "OK");
    delay(HTTP_DELAY);
    // read_module();

    return response;
}

//! PUBLIC
String send_http_post_sim(String address, String post_data)
{
    if (!sim_is_initialized)
    {
        bool could_init_sim = sim_init();
        if (!could_init_sim)
            return "FAILED";
    }

    // flush output
    read_module();
    delay(HTTP_DELAY);

    // Make sure HTTP session is terminated
    send_command("AT+HTTPTERM"); // Might return error, but just to make sure, waits becaus if it does return ok, it will overwrite other command
    read_module();

    // Initialize HTTP session
    send_and_wait("AT+HTTPINIT", "OK");
    delay(HTTP_DELAY);

    // Set URL
    String command = "AT+HTTPPARA=\"URL\",\"" + address + "\"";
    send_and_wait(command, "OK");
    delay(HTTP_DELAY);

    // Set headers
    String header = String("AT+HTTPPARA=\"USERDATA\",\"X-API-Key: ") +
                    String(encrypted_api_key) +
                    "\\r\\n\"";
    send_and_wait(header, "OK");
    delay(HTTP_DELAY);

    // Prepare POST data
    command = "AT+HTTPDATA=" + String(post_data.length()) + ",10000"; // 10000 = timeout in ms
    if (send_and_wait(command, "DOWNLOAD"))
    {
        // Send actual POST data
        send_and_wait(post_data, "OK");
    }
    delay(HTTP_DELAY);

    // Send POST request
    send_and_wait("AT+HTTPACTION=1", "+HTTPACTION");
    delay(HTTP_DELAY);
    delay(HTTP_DELAY);
    delay(HTTP_DELAY);

    // Read response
    String response = http_read_content();
    Serial.println(response);

    send_and_wait("AT+HTTPTERM", "OK");
    delay(HTTP_DELAY);
    return response;
}
