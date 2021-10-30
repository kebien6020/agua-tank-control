#include "app.hpp"

#include <HardwareSerial.h>

namespace kev {

App::App() {
}

auto App::setup() -> void {
  Serial.begin(baudrate);
  // Serial.println("Initialized");

  tBridge.setup();
}

auto App::loop() -> void {
  sch.execute();
}

}
