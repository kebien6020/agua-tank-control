#pragma once

#include <TaskSchedulerDeclarations.h>

#include "chrono.hpp"

namespace kev {


template<typename = void>
class Bridge : public Task {

  static constexpr auto period = 2_ms;

public:
  Bridge(Scheduler& sch): Task{period.count(), TASK_FOREVER, &sch, true} {}

  auto setup() {
    Serial1.begin(9600);
    Serial1.print("baud=115200\xFF\xFF\xFF");
    delay(20);
    Serial1.flush();
    Serial1.begin(115200);
    Serial1.print("sendme\xFF\xFF\xFF");
  }

  auto Callback() -> bool {
    while (Serial.available()) {
      Serial1.write(Serial.read());
    }
    while (Serial1.available()) {
      Serial.write(Serial1.read());
    }
    return true;
  }
};

}
