#pragma once

#include <kev-fsm.hpp>

#include "config.hpp"
#include "chrono.hpp"

namespace kev {

// Forward declare to avoid circular dep
template<typename>
class MainTankControl;

namespace ScreenFsm {

  extern Fsm fsm;
  auto init(MainTankControl<void>& mtc) -> void;
  auto setLastRecirTank(RecirTank tank) -> void;
  auto setLastRecirTime(chr::milliseconds time) -> void;

  namespace E {
    constexpr auto FILL_1      = Event{1, "FILL_1"};
    constexpr auto FILL_2      = Event{2, "FILL_2"};
    constexpr auto RECIR_SHORT = Event{3, "RECIR_SHORT"};
    constexpr auto RECIR_LONG  = Event{4, "RECIR_LONG"};
    constexpr auto RECIR_END   = Event{5, "RECIR_END"};
    constexpr auto EXIT        = Event{6, "EXIT"};
    constexpr auto RESET       = Event{7, "RESET"};
  }

}

}