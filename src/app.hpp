#pragma once

#include <Arduino.h>
#include <DirectIO.h>
#include <TaskSchedulerDeclarations.h>

#include "blink.hpp"
#include "bridge.hpp"
#include "chrono.hpp"
#include "config.hpp"

namespace kev {

class App {
public:
  App();
  auto setup() -> void;
  auto loop() -> void;

private:
  Scheduler sch{};

  OutputLow<relay1_pin> relay1{};
  OutputLow<relay2_pin> relay2{};
  OutputLow<relay3_pin> relay3{};

  Blink<OutputLow<relay1_pin>> tBlinkSlow{sch, relay1, 1000_ms};
  Blink<OutputLow<relay2_pin>> tBlinkFast{sch, relay2, 2000_ms};
  Blink<OutputLow<relay3_pin>> tBlinkFaster{sch, relay3, 500_ms};
  Bridge<> tBridge{sch};
};

}
