#pragma once

#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <vector>  
#include <chrono>
#include <thread>

#include "variables.h"
using namespace std;  // <-- So we can use vector<String>

// Returns vector<String> — always safe (empty on error)
vector<String> parseJsonArray(const String& jsonString) {
  vector<String> result;

  // Safety: empty input
  if (jsonString.length() == 0) {
    Serial.println("Empty JSON string");
    return result;
  }

  // Estimate size: 10 items max for now
  const size_t capacity = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    Serial.print("JSON parse failed: ");
    Serial.println(error.c_str());
    return result;  // return empty
  }

  if (!doc.is<JsonArray>()) {
    Serial.println("JSON is not an array");
    return result;
  }

  JsonArray array = doc.as<JsonArray>();
  for (JsonVariant value : array) {
    result.push_back(value.as<String>());
  }

  Serial.printf("Parsed %d contacts\n", result.size());
  return result;
}