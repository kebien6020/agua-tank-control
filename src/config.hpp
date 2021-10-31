#pragma once

#include "chrono.hpp"

namespace kev {

namespace config {
  constexpr auto baudrate = 115200;
  constexpr auto sensorEmpty = false;

  constexpr chr::milliseconds preFillTime    = 3_s;
  constexpr chr::milliseconds recirTimeShort = chr::seconds{3};
  // constexpr chr::milliseconds recirTimeShort = chr::minutes{5};
  constexpr chr::milliseconds recirTimeLong  = chr::minutes{40};
  constexpr chr::milliseconds fillSafetyTime = chr::minutes{27};


  constexpr auto sensorFull = !sensorEmpty;

}

enum class RecirTank {
  Tank1,
  Tank2,
};

}
