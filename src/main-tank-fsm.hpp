#pragma once

#include <kev-fsm.hpp>

#include "aliases.hpp"
#include "config.hpp"
#include "io.hpp"
#include "screen-ui.hpp"

namespace kev {

namespace MainTankFsm {
  extern Fsm fsm;
  auto init(Io<>& ioParam, ScreenUi<>& uiParam) -> void;
  auto setLastTime(chr::milliseconds time) -> void;

  namespace E {
    constexpr auto FILL_1      = Event{1, "FILL_1"};
    constexpr auto FILL_2      = Event{2, "FILL_2"};
    constexpr auto SENSOR_HI_1 = Event{3, "SENSOR_HI_1"};
    constexpr auto SENSOR_HI_2 = Event{4, "SENSOR_HI_2"};
    constexpr auto SENSOR_LO_3 = Event{5, "SENSOR_LO_3"};
    constexpr auto CLEAR_ALARM = Event{6, "CLEAR_ALARM"};
    constexpr auto RECIR_1     = Event{7, "RECIR_1"};
    constexpr auto RECIR_2     = Event{8, "RECIR_2"};
    constexpr auto RECIR_TIME  = Event{9, "RECIR_TIME"};
    constexpr auto STOP        = Event{10, "STOP"};
  }
}

}
