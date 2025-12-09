#include <Arduino.h>
#include "variables.h"
#define RX_PIN 17
#define TX_PIN 16
HardwareSerial modem(2);

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

void send_command(String command)
{
    command.trim();
    if (command.length() > 0)
    {
        modem.print(command + "\r"); // Use \r for modem commands
        Serial.println("Sent: " + command);
    }
}

String send_and_wait(String command, String token_to_wait_for)
{
    send_command(command);
    return waitForURC(token_to_wait_for);
}

void wait_for_ip()
{
    String ip = "0.0.0.0";
    while (ip == "0.0.0.0")
    {
        String resp = send_and_wait("AT+CGPADDR=1", "+CGPADDR");
        ip = resp != "" ? resp : "0.0.0.0";
    }
    Serial.println("IP acquired: " + ip);
}

void sim_init()
{
    Serial.begin(115200);

    delay(1000);
    Serial.println("Serial connection ready");

    modem.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    send_and_wait("AT", "OK");
    send_and_wait("ATE0", "OK");
}

String http_read_content()
{
    // Request the full payload
    modem.print("AT+HTTPREAD=0,9999\r");

    String content = "";
    bool inPayload = false;
    unsigned long start = millis();

    while (millis() - start < 5000) // 5 s timeout
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

String send_http(String address)
{
    // flush output
    read_module();
    delay(5000);

    send_and_wait("AT+HTTPTERM", "OK"); // Might return error, but just to make sure, waits becaus if it does return ok, it will overwrite other command
    delay(5000);

    send_and_wait("AT+HTTPINIT", "OK");
    delay(5000);

    String command = "AT+HTTPPARA=\"URL\",\"" + address + "\"";
    send_and_wait(command, "OK");
    delay(5000);
    send_and_wait("AT+HTTPACTION=0", "+HTTPACTION");
    delay(5000);

    Serial.println(http_read_content());
    // read_module();
    return "";
}