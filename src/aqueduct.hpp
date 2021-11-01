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

  auto setPump(bool active) {
    if (active && io.sensorHi3 == config::sensorFull) {
      Serial.println("[mtc] Ignoring aqueduct_pump_on becuse sensorHi3 is full");
      return false;
    }
    fsm.trigger(active ? AqueductFsm::E::PUMP_ON : AqueductFsm::E::PUMP_OFF);
  }

  auto setValve(bool active) {
    if (active && io.sensorHi3 == config::sensorFull) {
      Serial.println("[mtc] Ignoring aqueduct_valve_on becuse sensorHi3 is full");
      return false;
    }
    fsm.trigger(active ? AqueductFsm::E::VALVE_ON : AqueductFsm::E::VALVE_OFF);
  }

  auto getValve() -> bool {
    return io.valveIn3;
  }

  auto getPump()-> bool {
    return io.aqueductPump;
  }

private:
  Fsm& fsm;
  Io<>& io;

  FsmEdge<SensorHi3, Fsm, Event> sensorHi3Edge{io.sensorHi3, fsm, AqueductFsm::E::SENSOR_HI_3};
};

}
