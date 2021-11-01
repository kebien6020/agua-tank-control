#include "aqueduct-fsm.hpp"

#include "screen-ui.hpp"

namespace kev {

namespace AqueductFsm {

  Io<>* io = nullptr;
  ScreenUi<>* ui = nullptr;

  auto onStopped() {
    io->aqueductPump = false;
    io->valveIn3 = false;
    ui->updateAqueductInterface();
  }

  auto onFilling() -> void {
    io->valveIn3 = true;
    io->aqueductPump = false;
  }

  auto onFillingPump() -> void {
    io->valveIn3 = true;
    io->aqueductPump = true;
  }

  auto onTransition(State& from, State& to, struct Event event) {
    Serial.print("[aqueduct-state-machine] state transition. from="); Serial.print(from.name);
    Serial.print(" to="); Serial.print(to.name);
    Serial.print(" event="); Serial.print(event.name);
    Serial.println();
  }

  State stopped{"STOPPED", onStopped};
  State filling{"FILLING", onFilling};
  State fillingPump{"FILLING_PUMP", onFillingPump};

  Fsm fsm{&stopped, onTransition};

  auto isStopped() -> bool {
    auto const& current = fsm.current_state();
    return &current == &stopped;
  }

  auto isFilling() -> bool {
    auto const& current = fsm.current_state();
    return &current == &filling;
  }

  auto isFillingPump() -> bool {
    auto const& current = fsm.current_state();
    return &current == &fillingPump;
  }

  auto init(Io<>& ioParam, ScreenUi<>& uiParam) -> void {
    io = &ioParam;
    ui = &uiParam;

    fsm.add_transition(&stopped,     &filling,     E::VALVE_ON);
    fsm.add_transition(&filling,     &stopped,     E::VALVE_OFF);
    fsm.add_transition(&stopped,     &fillingPump, E::PUMP_ON);
    fsm.add_transition(&fillingPump, &stopped,     E::VALVE_OFF);
    fsm.add_transition(&filling,     &fillingPump, E::PUMP_ON);
    fsm.add_transition(&fillingPump, &filling,     E::PUMP_OFF);

    fsm.add_transition(&filling,     &stopped, E::SENSOR_HI_3);
    fsm.add_transition(&fillingPump, &stopped, E::SENSOR_HI_3);


    Serial.print("[aqueduct-state-machine] initialized. initialState=");
    Serial.println(fsm.current_state().name);
  }

}

}
