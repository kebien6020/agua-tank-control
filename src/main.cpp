//#define BRIDGE
#ifdef BRIDGE

#include "bridge.main.hpp"

#else

#include <TaskScheduler.h>
#include "app.hpp"

void setup() {
  Serial.begin(kev::config::baudrate);

  kev::App app{};

  while (true) {
    app.loop();
  }
}
void loop() { }

#endif
