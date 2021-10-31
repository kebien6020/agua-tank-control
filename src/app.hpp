#pragma once

#include <Arduino.h>
#include <DirectIO.h>
#include <TaskSchedulerDeclarations.h>

#include "aqueduct.hpp"
#include "blink.hpp"
#include "chrono.hpp"
#include "config.hpp"
#include "io.hpp"
#include "serial-ui.hpp"

#include "mtc.hpp"

namespace kev {

class App {
public:
  App();
  auto loop() -> void;

private:
  Scheduler sch{};
  Io<> io;

  MainTankControl<> mtc{sch, io};
  AqueductControl<> aqueduct{sch, io};

  SerialUi<> tSerialUi{sch, mtc, aqueduct};
  ScreenUi<> tScreenUi{sch, mtc, aqueduct};

};

}
