#include "screen-fsm.hpp"

#include <Nextion.h>
#include <estd/array.h>

#include "config.hpp"
#include "mtc.hpp"
#include "timer.hpp"

namespace kev {

namespace ScreenFsm {

  MainTankControl<>* mtc;

  NexPage pgHome{0, 0, "home"};
  NexButton fill1{1, 2, "llenar1"};
  NexButton fill2{1, 3, "llenar2"};
  NexButton recirTank1{3, 2, "recir1"};
  NexButton recirTank2{3, 3, "recir2"};
  NexButton timeShort{4, 2, "timeShort"};
  NexButton timeLong{4, 3, "timeLong"};

  NexTouch* menusTouchables[] = {
    &fill1,
    &fill2,
    &recirTank1,
    &recirTank2,
    &timeShort,
    &timeLong,
    nullptr,
  };

  NexButton fillingReset{2, 2, "b2"};

  NexTouch* fillingTouchables[] = {
    &fillingReset,
    nullptr,
  };

  NexButton recirReset{5, 3, "b2"};
  NexText minutesTxt{5, 4, "minutos"};
  NexText secondsTxt{5, 5, "segundos"};

  NexTouch* recirTouchables[] = {
    &recirReset,
    nullptr,
  };

  NexPage recirSummaryPg{6, 0, "recirSummary"};
  NexButton recirSummaryReset{6, 6, "b2"};
  NexText recirSummaryTankTxt{6, 3, "tanque"};
  NexText recirSummaryTimeTxt{6, 5, "tiempo"};

  NexTouch* recirSummaryTouchables[] = {
    &recirSummaryReset,
    nullptr,
  };

  auto recirTank = RecirTank::Tank1;

  auto lastRecirTank = RecirTank::Tank1;
  auto lastRecirTime = 0_ms;

  auto setLastRecirTank(RecirTank tank) -> void {
    lastRecirTank = tank;
  }

  auto setLastRecirTime(chr::milliseconds time) -> void {
    lastRecirTime = time;
  }

  auto onMenus() -> void {
    pgHome.show();
  }

  auto duringMenus() -> void {
    nexLoop(menusTouchables);
  }

  auto onFilling1() -> void {
    mtc->fill1();
  }

  auto onFilling2() -> void {
    mtc->fill2();
  }

  auto duringFilling() -> void {
    nexLoop(fillingTouchables);
  }

  auto onRecirShort() -> void {
    if (recirTank == RecirTank::Tank1) mtc->recir1Short();
    if (recirTank == RecirTank::Tank2) mtc->recir2Short();
  }

  auto onRecirLong() -> void {
    if (recirTank == RecirTank::Tank1) mtc->recir1Long();
    if (recirTank == RecirTank::Tank2) mtc->recir2Long();
  }

  auto updateTimer = Timer<>{};

  auto updateRemaining() -> void {
    auto const remaining = chr::seconds{mtc->recirRemaining()};
    auto const secs = long(remaining.count());

    auto const secStr = String(secs % 60l);
    auto const minStr = String(secs / 60l);

    secondsTxt.setText(secStr.c_str());
    minutesTxt.setText(minStr.c_str());
  }

  auto duringRecir() -> void {
    nexLoop(recirTouchables);
    if (updateTimer.justFinished()) {
      updateTimer.start();
      updateRemaining();
    }
  }

  auto onRecirSummary() -> void {
    recirSummaryPg.show();

    const auto tankTxt = String(int(lastRecirTank));
    const auto timeTxt = String(chr::minutes{lastRecirTime}.count()) + " min";

    recirSummaryTankTxt.setText(tankTxt.c_str());
    recirSummaryTimeTxt.setText(timeTxt.c_str());
  }

  auto duringRecirSummary() -> void {
    nexLoop(recirSummaryTouchables);
  }

  auto onReset() -> void {
    mtc->stop();
  }

  State menus{"MENUS", onMenus, duringMenus};
  State filling{"FILLING", nullptr, duringFilling};
  State recir{"RECIR", nullptr, duringRecir};
  State recirSummary{"RECIR_SUMMARY", onRecirSummary, duringRecirSummary};

  auto onTransition(State& from, State& to, struct Event event) {
    Serial.print("[ui-state-machine] state transition. from="); Serial.print(from.name);
    Serial.print(" to="); Serial.print(to.name);
    Serial.print(" event="); Serial.print(event.name);
    Serial.println();
  }

  Fsm fsm{&menus, onTransition};

  auto screenInit() {
    auto success = nexInit();
    if (success) {
      Serial.println("[ui-state-machine] screen found");
    } else {
      Serial.println("[ui-state-machine] screen not found");
    }

    fill1.attachPop     ([](void*){ fsm.trigger(E::FILL_1); });
    fill2.attachPop     ([](void*){ fsm.trigger(E::FILL_2); });
    recirTank1.attachPop([](void*){ recirTank = RecirTank::Tank1; });
    recirTank2.attachPop([](void*){ recirTank = RecirTank::Tank2; });
    timeShort.attachPop ([](void*){ fsm.trigger(E::RECIR_SHORT); });
    timeLong.attachPop  ([](void*){ fsm.trigger(E::RECIR_LONG); });

    fillingReset.attachPop     ([](void*){ fsm.trigger(E::RESET); });
    recirReset.attachPop       ([](void*){ fsm.trigger(E::RESET); });
    recirSummaryReset.attachPop([](void*){ fsm.trigger(E::EXIT); });
  }

  auto init(MainTankControl<>& mtcParam) -> void {
    mtc = &mtcParam;
    screenInit();
    updateTimer.start(1_s);

    fsm.add_transition(&menus,   &filling, E::FILL_1, onFilling1);
    fsm.add_transition(&menus,   &filling, E::FILL_2, onFilling2);
    fsm.add_transition(&filling, &menus,   E::EXIT);
    fsm.add_transition(&filling, &menus,   E::RESET,  onReset);

    fsm.add_transition(&menus, &recir,        E::RECIR_SHORT, onRecirShort);
    fsm.add_transition(&menus, &recir,        E::RECIR_LONG,  onRecirLong);
    fsm.add_transition(&recir, &menus,        E::RESET,       onReset);
    fsm.add_transition(&recir, &recirSummary, E::RECIR_END);

    fsm.add_transition(&recirSummary, &menus, E::EXIT);

    Serial.print("[ui-state-machine] initialized. initialState=");
    Serial.println(fsm.current_state().name);
  }

}

}
