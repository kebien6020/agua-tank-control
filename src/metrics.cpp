#include "metrics.hpp"

#include <HardwareSerial.h>
#include <ArduinoJson.h>

namespace kev {

Metrics metrics;

using namespace ArduinoJson;

auto& mSerial = Serial2;

StaticJsonDocument<1024> doc;

Metrics::Metrics() {
  mSerial.begin(9600);
}

auto Metrics::sendGaugeMetric(czstring name, long value) -> void {
  doc.clear();
  doc["action"] = "metric";
  doc["type"] = "gauge";
  doc["name"] = name;
  doc["value"] = value;

  serializeJson(doc, mSerial);
  mSerial.print('\0');

  Serial.print("[metrics] sending metric. json=");
  serializeJson(doc, Serial);
  Serial.println();
}

auto Metrics::sendCountMetric(czstring name, long value) -> void {
  doc.clear();
  doc["action"] = "metric";
  doc["type"] = "count";
  doc["name"] = name;
  doc["value"] = value;
  serializeJson(doc, mSerial);
  mSerial.print('\0');

  Serial.print("[metrics] sending metric. json=");
  serializeJson(doc, Serial);
  Serial.println();
}

}
