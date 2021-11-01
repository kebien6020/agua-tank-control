#pragma once

#include <TaskSchedulerDeclarations.h>

#include "aqueduct-fsm.hpp"
#include "chrono.hpp"
#include "edge.hpp"
#include "io.hpp"

namespace kev {

template<typename = void>
class AqueductControl : public Task {
  static constexpr chr::milliseconds period = 10_ms;
public:
  AqueductControl(Scheduler& sch, Io<>& io)
    : Task{period.count(), TASK_FOREVER, &sch, true}
    , fsm{AqueductFsm::fsm}
    , io{io}
    {}

  auto setUi(ScreenUi<>& ui) {
    AqueductFsm::init(io, ui);
  }

  auto Callback() -> bool {
    sensorHi3Edge.update();
    fsm.run_machine();
    return true;
  }

  auto fill() -> bool {
    if (io.sensorHi3 == config::sensorFull) {
      Serial.println("[mtc] Ignoring auqeduct_fill becuse sensorHi3 is full");
      return false;
    }
    fsm.trigger(AqueductFsm::E::FILL);
    return true;
  }

  auto stop() {
    fsm.trigger(AqueductFsm::E::STOP);
  }

  auto togglePump() -> bool {
    if (AqueductFsm::isStopped() && io.valveIn3) {
      return fill();
    }

    if (AqueductFsm::isFilling()) {
      stop();
      return false;
    }

    return false;
  }

  auto setValve(bool active) {
    if (!active && isFilling()) {
      // Turn off the pump when turning off the valve
      stop();
    }
    io.valveIn3 = active;
  }

  auto getValve() -> bool {
    return io.valveIn3;
  }

  auto isFilling()-> bool {
    return AqueductFsm::isFilling();
  }

private:
  Fsm& fsm;
  Io<>& io;

  FsmEdge<SensorHi3, Fsm, Event> sensorHi3Edge{io.sensorHi3, fsm, AqueductFsm::E::SENSOR_HI_3};
};

}
