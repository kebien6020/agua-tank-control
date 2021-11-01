#include "main-tank-fsm.hpp"

#include <HardwareSerial.h>

#include "aliases.hpp"
#include "screen-ui.hpp"
#include "metrics.hpp"

namespace kev {

namespace MainTankFsm {

  Io<>* io = nullptr;
  ScreenUi<>* ui = nullptr;

  chr::milliseconds lastRecirTime;
  RecirTank lastRecirTank;

  auto setLastTime(chr::milliseconds time) -> void {
    lastRecirTime = time;
  }

  auto setLastRecirTank(RecirTank tank) -> void {
    lastRecirTank = tank;
  }

  auto onStopped() {
    io->fillPump = false;
    io->recirPump1 = false;
    io->recirPump2 = false;

    io->energy = false;

    io->valveIn1 = false;
    io->valveIn2 = false;
  }

  auto onPreFilling1() -> void {
    io->valveIn1 = true;
  }

  auto onPreFilling2() -> void {
    io->valveIn2 = true;
  }

  auto onFilling1() -> void {
    io->fillPump = true;
    io->energy = true;
  }

  auto onFilling2() -> void {
    io->fillPump = true;
    io->energy = true;
  }

  auto onRecir1() -> void {
    io->recirPump1 = true;
    io->energy = true;
    setLastRecirTank(RecirTank::Tank1);
  }

  auto onRecir2() -> void {
    io->recirPump2 = true;
    io->energy = true;
    setLastRecirTank(RecirTank::Tank2);
  }

  auto onRecirEnd() -> void {
    ui->recirEnd(lastRecirTank, lastRecirTime);

    auto const metricName = lastRecirTank == RecirTank::Tank1
        ? "recir1.completed"
        : "recir2.completed";
    metrics.sendCountMetric(metricName);
  }

  auto onAlarm() -> void {
    onStopped();
    auto const msg =
      String{"Tiempo de seguridad de llenado superado ("} +
      String{chr::minutes{config::fillSafetyTime}.count()} +
      " min).";
    ui->alarm(msg);

    metrics.sendCountMetric("alarm");
  }

  auto fill1Exit() {
    ui->exit();
    metrics.sendCountMetric("fill1.completed");
  }

  auto fill2Exit() {
    ui->exit();
    metrics.sendCountMetric("fill2.completed");
  }

  auto fill1Abort() {
    ui->exit();
    metrics.sendCountMetric("fill1.aborted");
  }

  auto fill2Abort() {
    ui->exit();
    metrics.sendCountMetric("fill2.aborted");
  }

  auto recir1Abort() {
    metrics.sendCountMetric("recir1.aborted");
  }

  auto recir2Abort() {
    metrics.sendCountMetric("recir2.aborted");
  }

  auto uiExit() -> void {
    ui->exit();
  }

  auto onTransition(State& from, State& to, struct Event event) {
    Serial.print("[main-state-machine] state transition. from="); Serial.print(from.name);
    Serial.print(" to="); Serial.print(to.name);
    Serial.print(" event="); Serial.print(event.name);
    Serial.println();
  }

  State stopped{"STOPPED", onStopped};
  State preFilling1{"PRE_FILLING1", onPreFilling1};
  State preFilling2{"PRE_FILLING2", onPreFilling2};
  State filling1{"FILLING1", onFilling1};
  State filling2{"FILLING2", onFilling2};
  State alarm{"ALARM", onAlarm};
  State recir1{"RECIR1", onRecir1};
  State recir2{"RECIR2", onRecir2};

  Fsm fsm{&stopped, onTransition};

  auto init(Io<>& ioParam, ScreenUi<>& uiParam) -> void {
    io = &ioParam;
    ui = &uiParam;

    fsm.add_transition      (&stopped,     &preFilling1, E::FILL_1);
    fsm.add_timed_transition(&preFilling1, &filling1,    config::preFillTime.count());
    fsm.add_transition      (&preFilling1, &stopped,     E::SENSOR_HI_1, uiExit);
    fsm.add_transition      (&preFilling1, &stopped,     E::SENSOR_LO_3, uiExit);
    fsm.add_transition      (&filling1,    &stopped,     E::SENSOR_HI_1, fill1Exit);
    fsm.add_transition      (&filling1,    &stopped,     E::SENSOR_LO_3, fill1Exit);
    fsm.add_timed_transition(&filling1,    &alarm,       config::fillSafetyTime.count());

    fsm.add_transition      (&stopped,     &preFilling2, E::FILL_2);
    fsm.add_timed_transition(&preFilling2, &filling2,    config::preFillTime.count());
    fsm.add_transition      (&preFilling2, &stopped,     E::SENSOR_HI_2, uiExit);
    fsm.add_transition      (&preFilling2, &stopped,     E::SENSOR_LO_3, uiExit);
    fsm.add_transition      (&filling2,    &stopped,     E::SENSOR_HI_2, fill2Exit);
    fsm.add_transition      (&filling2,    &stopped,     E::SENSOR_LO_3, fill2Exit);
    fsm.add_timed_transition(&filling2,    &alarm,       config::fillSafetyTime.count());

    fsm.add_transition(&stopped, &recir1, E::RECIR_1);
    fsm.add_transition(&recir1, &stopped, E::RECIR_TIME, onRecirEnd);

    fsm.add_transition(&stopped, &recir2, E::RECIR_2);
    fsm.add_transition(&recir2, &stopped, E::RECIR_TIME, onRecirEnd);

    fsm.add_transition(&alarm,       &stopped, E::CLEAR_ALARM);
    fsm.add_transition(&preFilling1, &stopped, E::STOP, fill1Abort);
    fsm.add_transition(&filling1,    &stopped, E::STOP, fill1Abort);
    fsm.add_transition(&preFilling2, &stopped, E::STOP, fill2Abort);
    fsm.add_transition(&filling2,    &stopped, E::STOP, fill2Abort);
    fsm.add_transition(&recir1,      &stopped, E::STOP, recir1Abort);
    fsm.add_transition(&recir2,      &stopped, E::STOP, recir2Abort);

    Serial.print("[main-state-machine] initialized. initialState=");
    Serial.println(fsm.current_state().name);
  }

}

}
