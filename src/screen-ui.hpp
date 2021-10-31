#pragma once

#include <TaskSchedulerDeclarations.h>

#include "config.hpp"
#include "chrono.hpp"
#include "screen-fsm.hpp"

namespace kev {

template<typename = void>
class ScreenUi : public Task {
  static constexpr chr::milliseconds period = 100_ms;
public:
  ScreenUi(Scheduler& sch, MainTankControl<void>& mtc, AqueductControl<void>& aqueduct)
    : Task{period.count(), TASK_FOREVER, &sch, true}
    , fsm{ScreenFsm::fsm}
  {
    ScreenFsm::init(mtc, aqueduct);
  }

  auto Callback() -> bool {
    fsm.run_machine();
    return true;
  }

  auto exit() -> void {
    fsm.trigger(ScreenFsm::E::EXIT);
  }

  auto recirEnd(RecirTank tank, chr::milliseconds time) {
    ScreenFsm::setLastRecirTank(tank);
    ScreenFsm::setLastRecirTime(time);
    fsm.trigger(ScreenFsm::E::RECIR_END);
  }

private:
  Fsm& fsm;
};

}
