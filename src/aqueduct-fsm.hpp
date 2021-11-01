#pragma once

#include <kev-fsm.hpp>

#include "io.hpp"
#include "screen-ui.hpp"

namespace kev {

namespace AqueductFsm {
  extern Fsm fsm;
  auto init(Io<>& ioParam, ScreenUi<>& uiParam) -> void;
  auto isStopped() -> bool;
  auto isFilling() -> bool;
  auto isFillingPump() -> bool;

  namespace E {
    constexpr auto VALVE_ON    = Event{1, "VALVE_ON"};
    constexpr auto VALVE_OFF   = Event{2, "VALVE_OFF"};
    constexpr auto PUMP_ON     = Event{3, "PUMP_ON"};
    constexpr auto PUMP_OFF    = Event{4, "PUMP_OFF"};
    constexpr auto SENSOR_HI_3 = Event{5, "SENSOR_HI_3"};
  }
}

}
