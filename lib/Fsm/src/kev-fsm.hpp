// This file is part of arduino-fsm.
//
// arduino-fsm is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// arduino-fsm is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with arduino-fsm.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FSM_H
#define FSM_H


#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif


struct State
{
  char const* name;
  State(char const* name, void (*on_enter)(), void (*on_state)() = nullptr, void (*on_exit)() = nullptr);
  void (*on_enter)();
  void (*on_state)();
  void (*on_exit)();
};


struct Event
{
  int id;
  char const* name;
};

bool operator==(Event a, Event b);
bool operator!=(Event a, Event b);

using TransitionCallback = void (*)(State&,State&,Event);

class Fsm
{
public:
  Fsm(State* initial_state, TransitionCallback on_transition = nullptr);
  ~Fsm();

  void add_transition(State* state_from, State* state_to, Event event,
                      void (*on_transition)() = nullptr);

  void add_timed_transition(State* state_from, State* state_to,
                            unsigned long interval, void (*on_transition)() = nullptr);

  void check_timed_transitions();

  void trigger(Event event);
  void run_machine();

  State& current_state();

private:
  struct Transition
  {
    State* state_from;
    State* state_to;
    Event event;
    void (*on_transition)();

  };
  struct TimedTransition
  {
    Transition transition;
    unsigned long start;
    unsigned long interval;
  };

  static Transition create_transition(State* state_from, State* state_to,
                                      Event event, void (*on_transition)());

  void make_transition(Transition* transition);

private:
  State* m_current_state;
  Transition* m_transitions;
  int m_num_transitions;

  TimedTransition* m_timed_transitions;
  int m_num_timed_transitions;
  bool m_initialized;

  TransitionCallback m_on_transition;
};


#endif
