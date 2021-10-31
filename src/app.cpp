#include "app.hpp"

#include <HardwareSerial.h>

namespace kev {

App::App() {
  mtc.setUi(tScreenUi);
  aqueduct.setUi(tScreenUi);
  Serial.println("[app] initialized");
}

auto App::loop() -> void {
  sch.execute();
}

}
