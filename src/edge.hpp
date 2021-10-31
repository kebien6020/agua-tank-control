#pragma once

#undef FALLING
#undef RISING

#include <HardwareSerial.h>

namespace kev {

template <class Reader>
class Edge {
public:
  Edge(Reader& reader, bool prev = false, bool curr = false)
    : reader{reader}
    , prev{prev}
    , curr{curr}
    {}

  auto update() -> void {
    prev = curr;
    curr = reader.read();
  }

  auto risingEdge() -> bool {
    auto const retval =  changed() && curr;
    return retval;
  }

  auto fallingEdge() -> bool {
    auto const retval =  changed() && !curr;
    return retval;
  }

  auto changed() -> bool {
    return prev != curr;
  }

private:
  bool prev;
  bool curr;
  Reader& reader;
};

enum class EdgeVariant { FALLING, RISING };

template <class Reader, class Fsm, class Event>
class FsmEdge {
public:

  FsmEdge(Reader& reader, Fsm& fsm, Event event, EdgeVariant ev = EdgeVariant::RISING, bool prev = false, bool curr = false)
    : edge{reader, prev, curr}
    , fsm{fsm}
    , event{event}
    , ev{ev}
    {}

  auto update() -> void {
    edge.update();

    if (
        (ev == EdgeVariant::RISING && edge.risingEdge()) ||
        (ev == EdgeVariant::FALLING && edge.fallingEdge())
    ) {
      Serial.print("[edge] triggering event. edge="); Serial.print(edge.risingEdge() ? "rising" : "falling");
      Serial.print(" event="); Serial.print(event.name);
      Serial.println();
      fsm.trigger(event);
    }

  }
private:
  Edge<Reader> edge;
  Event event;
  EdgeVariant ev;
  Fsm& fsm;
};

}
