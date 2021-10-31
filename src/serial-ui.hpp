#pragma once

#include <HardwareSerial.h>
#include <TaskSchedulerDeclarations.h>

#include "aqueduct.hpp"
#include "chrono.hpp"
#include "mtc.hpp"

namespace kev {


template<typename = void>
class SerialUi : public Task {
  static constexpr chr::milliseconds period = 100_ms;
public:
  SerialUi(Scheduler& sch, MainTankControl<>& mtc, AqueductControl<>& aqueduct)
    : Task{period.count(), TASK_FOREVER, &sch, true}
    , mtc{mtc}
    , aqueduct{aqueduct}
    {}

  auto Callback() -> bool {
    if (Serial.available()) {
      auto const cmd = Serial.readStringUntil('\r');
      Serial.read(); // discard \n
      processCmd(cmd);
    }

    return true;
  }

  auto processCmd(String const& cmd) {
    Serial.print("[ui] cmd="); Serial.println(cmd);

    if (cmd.startsWith("nex")) {
      while (Serial1.available()) {
        Serial.print(Serial1.read(), 16);
      }
      Serial.println();
      auto const nexCmd = cmd.substring(3);
      Serial1.print(nexCmd);
      Serial1.print("\xFF\xFF\xFF");
    }
    if (cmd == "fill1") mtc.fill1();
    if (cmd == "fill2") mtc.fill2();
    if (cmd == "recir1_short") mtc.recir1Short();
    if (cmd == "recir1_long") mtc.recir1Long();
    if (cmd == "recir2_short") mtc.recir2Short();
    if (cmd == "recir2_long") mtc.recir2Long();
    if (cmd == "stop") mtc.stop();
    if (cmd == "clear_alarm") mtc.clearAlarm();
    if (cmd == "aqueduct_fill") aqueduct.fill();
    if (cmd == "aqueduct_stop") aqueduct.stop();
  }
private:
  MainTankControl<>& mtc;
  AqueductControl<>& aqueduct;
};

}
