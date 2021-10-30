#pragma once

#include <TaskSchedulerDeclarations.h>
#include <estd/chrono.h>

namespace chr = estd::chrono;

namespace kev {

template<class Output>
class Blink : public Task {
public:

  Blink(Scheduler& sch, Output& relay, chr::milliseconds period)
    : Task{static_cast<unsigned long>(period.count()), TASK_FOREVER, &sch, true}
    , sch{sch}
    , relay{relay}
    {}

  auto Callback() -> bool {
    relay.toggle();
    return true;
  }

private:
  Scheduler& sch;
  Output& relay;
};

}
