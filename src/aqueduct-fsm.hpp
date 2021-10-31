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

  namespace E {
    constexpr auto FILL        = Event{1, "FILL"};
    constexpr auto SENSOR_HI_3 = Event{2, "SENSOR_HI_3"};
    constexpr auto STOP        = Event{3, "STOP"};
  }
}

}
