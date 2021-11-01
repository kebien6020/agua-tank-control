#pragma once

#include "aliases.hpp"

namespace  kev {

class Metrics {
public:
  Metrics();
  auto sendGaugeMetric(czstring name, long value) -> void;
  auto sendCountMetric(czstring name, long value = 1) -> void;
};

extern Metrics metrics;

}
