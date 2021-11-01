#include "screen-fsm.hpp"

#include <Nextion.h>
#include <estd/array.h>

#include "aqueduct.hpp"
#include "config.hpp"
#include "mtc.hpp"
#include "timer.hpp"

namespace kev {

namespace ScreenFsm {

  MainTankControl<>* mtc;
  AqueductControl<>* aqueduct;

  NexPage pgHome{0, 0, "home"};
  NexButton aqueductMenu{0, 5, "aqueduct"};
  NexButton fill1{1, 2, "llenar1"};
  NexButton fill2{1, 3, "llenar2"};
  NexButton recirTank1{3, 2, "recir1"};
  NexButton recirTank2{3, 3, "recir2"};
  NexButton timeShort{4, 2, "timeShort"};
  NexButton timeLong{4, 3, "timeLong"};

  NexTouch* menusTouchables[] = {
    &aqueductMenu,
    &fill1,
    &fill2,
    &recirTank1,
    &recirTank2,
    &timeShort,
    &timeLong,
    nullptr,
  };

  NexDSButton aqueductValve{8, 3, "valve"};
  NexDSButton aqueductPump{8, 4, "pump"};
  NexButton aqueductBack{8, 2, "back"};

  NexTouch* aqueductTouchables[] = {
    &aqueductValve,
    &aqueductPump,
    &aqueductBack,
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

  NexPage alarmPg{7, 0, "alarm"};
  NexText alarmMsgTxt{7, 2, "text"};
  NexPage alarmReset{7, 3, "b2"};
  NexTouch* alarmTouchables[] = {
    &alarmReset,
    nullptr,
  };

  auto recirTank = RecirTank::Tank1;

  auto lastRecirTank = RecirTank::Tank1;
  auto lastRecirTime = 0_ms;

  auto alarmMsg = String{};

  auto setLastRecirTank(RecirTank tank) -> void {
    lastRecirTank = tank;
  }

  auto setLastRecirTime(chr::milliseconds time) -> void {
    lastRecirTime = time;
  }

  auto setAlarmMsg(String const& msg) -> void {
    alarmMsg = msg;
  }

  auto onMenus() -> void {
    pgHome.show();
  }

  auto duringMenus() -> void {
    nexLoop(menusTouchables);
  }

  auto onFilling1() -> void {
    auto const success = mtc->fill1();
    if (!success) fsm.trigger(E::EXIT);
  }

  auto onFilling2() -> void {
    auto const success = mtc->fill2();
    if (!success) fsm.trigger(E::EXIT);
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

  auto onAlarm() {
    alarmPg.show();
    alarmMsgTxt.setText(alarmMsg.c_str());
  }

  auto duringAlarm() {
    nexLoop(alarmTouchables);
  }

  auto onAlarmExit() {
    mtc->clearAlarm();
  }

  auto onAqueduct() {
    aqueductValve.setValue(aqueduct->getValve());
    aqueductPump.setValue(aqueduct->isFilling());
    sendCommand("tsw pump,valve.val"); // Enable pump if valve is enabled
  }

  auto duringAqueduct() {
    nexLoop(aqueductTouchables);
  }

  auto onAqueductValve() {
    uint32_t value = -1; aqueductValve.getValue(&value);
    aqueduct->setValve(value == 1);
    aqueductPump.setValue(aqueduct->isFilling()); // Setting the valve might update the pump
  }

  auto onAqueductPump() {
    auto const startedFilling = aqueduct->togglePump();
    aqueductPump.setValue(startedFilling);
  }

  State menus{"MENUS", onMenus, duringMenus};
  State filling{"FILLING", nullptr, duringFilling};
  State recir{"RECIR", nullptr, duringRecir};
  State recirSummary{"RECIR_SUMMARY", onRecirSummary, duringRecirSummary};
  State alarm{"ALARM", onAlarm, duringAlarm, onAlarmExit};
  State aqueductSt{"AQUEDUCT", onAqueduct, duringAqueduct};

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

    fill1.attachPop      ([](void*){ fsm.trigger(E::FILL_1); });
    fill2.attachPop      ([](void*){ fsm.trigger(E::FILL_2); });
    recirTank1.attachPop ([](void*){ recirTank = RecirTank::Tank1; });
    recirTank2.attachPop ([](void*){ recirTank = RecirTank::Tank2; });
    timeShort.attachPop  ([](void*){ fsm.trigger(E::RECIR_SHORT); });
    timeLong.attachPop   ([](void*){ fsm.trigger(E::RECIR_LONG); });

    aqueductMenu.attachPop ([](void*){ fsm.trigger(E::AQUEDUCT_MENU); });
    aqueductValve.attachPop([](void*){ onAqueductValve(); });
    aqueductPump.attachPop ([](void*){ onAqueductPump(); });
    aqueductBack.attachPop ([](void*){ fsm.trigger(E::AQUEDUCT_EXIT); });

    fillingReset.attachPop     ([](void*){ fsm.trigger(E::RESET); });
    recirReset.attachPop       ([](void*){ fsm.trigger(E::RESET); });
    recirSummaryReset.attachPop([](void*){ fsm.trigger(E::EXIT); });
    alarmReset.attachPop       ([](void*){ fsm.trigger(E::EXIT); });
  }

  auto init(MainTankControl<>& mtcParam, AqueductControl<>& aqueductParam) -> void {
    mtc = &mtcParam;
    aqueduct = &aqueductParam;

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

    fsm.add_transition(&filling,      &alarm, E::ALARM);
    fsm.add_transition(&recir,        &alarm, E::ALARM);
    fsm.add_transition(&recirSummary, &alarm, E::ALARM);
    fsm.add_transition(&menus,        &alarm, E::ALARM);
    fsm.add_transition(&alarm,        &menus, E::EXIT);

    fsm.add_transition(&menus,      &aqueductSt, E::AQUEDUCT_MENU);
    fsm.add_transition(&aqueductSt, &menus,      E::AQUEDUCT_EXIT);

    Serial.print("[ui-state-machine] initialized. initialState=");
    Serial.println(fsm.current_state().name);
  }

}

}
