#pragma once

#include <TaskSchedulerDeclarations.h>

#include "edge.hpp"
#include "main-tank-fsm.hpp"
#include "metrics.hpp"
#include "screen-ui.hpp"
#include "timer.hpp"

#undef FALLING
#undef RISING

namespace kev {

template<typename = void>
class MainTankControl : public Task {
  static constexpr chr::milliseconds period = 10_ms;
public:
  MainTankControl(Scheduler& sch, Io<>& io)
    : Task{period.count(), TASK_FOREVER, &sch, true}
    , fsm{MainTankFsm::fsm}
    , io{io}
    {}

  auto setUi(ScreenUi<>& ui) {
    MainTankFsm::init(io, ui);
  }

  auto Callback() -> bool {
    sensorHi1Edge.update();
    sensorHi2Edge.update();
    sensorLo3Edge.update();

    if (recirTimer.justFinished()) {
      fsm.trigger(MainTankFsm::E::RECIR_TIME);
    }

    fsm.run_machine();
    return true;
  }

  auto fill1() -> bool {
    if (io.sensorHi1 == config::sensorFull) {
      Serial.println("[mtc] Ignoring fill1 becuse sensorHi1 is full");
      return false;
    }
    if (io.sensorLo3 == config::sensorEmpty) {
      Serial.println("[mtc] Ignoring fill1 becuse sensorLo3 is empty");
      return false;
    }

    fsm.trigger(MainTankFsm::E::FILL_1);
    return true;
  }

  auto fill2() -> bool {
    if (io.sensorHi2 == config::sensorFull) {
      Serial.println("[mtc] Ignoring fill2 becuse sensorHi2 is full");
      return false;
    }
    if (io.sensorLo3 == config::sensorEmpty) {
      Serial.println("[mtc] Ignoring fill2 becuse sensorLo3 is empty");
      return false;
    }

    fsm.trigger(MainTankFsm::E::FILL_2);
    return true;
  }

  auto recir1Short() -> void {
    recir(RecirTank::Tank1, config::recirTimeShort);
  }

  auto recir1Long() -> void {
    recir(RecirTank::Tank1, config::recirTimeLong);
  }

  auto recir2Short() -> void {
    recir(RecirTank::Tank2, config::recirTimeShort);
  }

  auto recir2Long() -> void {
    recir(RecirTank::Tank2, config::recirTimeLong);
  }

  auto clearAlarm() -> void {
    fsm.trigger(MainTankFsm::E::CLEAR_ALARM);
  }

  auto stop() -> void {
    fsm.trigger(MainTankFsm::E::STOP);
  }

  auto stateStr() -> char const* {
    return fsm.current_state().name;
  }

  auto recirRemaining() -> chr::milliseconds {
    return recirTimer.remaining();
  }

private:

  auto recir(RecirTank tank, chr::milliseconds time) {
    MainTankFsm::setLastTime(time);
    recirTimer.start(time);

    auto const event = tank == RecirTank::Tank1
      ? MainTankFsm::E::RECIR_1
      : MainTankFsm::E::RECIR_2;
    fsm.trigger(event);

    auto const metricName = tank == RecirTank::Tank1
      ? "recir1.time"
      : "recir2.time";
    metrics.sendGaugeMetric(metricName, chr::seconds{time}.count());
  }

  Fsm& fsm;
  Io<>& io;
  ScreenUi<>* ui = nullptr;

  FsmEdge<SensorHi1, Fsm, Event> sensorHi1Edge{io.sensorHi1, fsm, MainTankFsm::E::SENSOR_HI_1};
  FsmEdge<SensorHi2, Fsm, Event> sensorHi2Edge{io.sensorHi2, fsm, MainTankFsm::E::SENSOR_HI_2};
  FsmEdge<SensorLo3, Fsm, Event> sensorLo3Edge{io.sensorLo3, fsm, MainTankFsm::E::SENSOR_LO_3, EdgeVariant::FALLING};

  Timer<> recirTimer;
};

}
