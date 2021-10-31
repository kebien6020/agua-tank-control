#pragma once

#include "chrono.hpp"

using clk = chr::arduino_clock;

namespace kev {

template<typename = void>
class Timer {
public:
  auto setDuration(chr::milliseconds dur) -> void {
    duration = dur;
  }

  auto start() -> void {
    startTime = clk::now();
    started = true;
  }

  auto start(chr::milliseconds dur) -> void {
    setDuration(dur);
    start();
  }

  auto finished() -> bool {
    return elapsed() >= duration;
  }

  auto justFinished() -> bool {
    if (!started) return false;
    if (finished()) {
      started = false;
      return true;
    }

    return false;
  }

  auto elapsed() -> chr::milliseconds {
    return clk::now() - startTime;
  }

  auto remaining() -> chr::milliseconds {
    return duration - elapsed();
  }

private:
  chr::milliseconds duration;
  clk::time_point startTime;

  bool started = false;
};

}
