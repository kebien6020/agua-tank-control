#pragma once

#include <DirectIO.h>

namespace kev {

using ValveIn1 = OutputLow<23>;
using ValveIn2 = OutputLow<25>;
using RecirPump1 = OutputLow<29>;
using RecirPump2 = OutputLow<31>;
using AqueductPump = OutputLow<33>;
using FillPump = OutputLow<35>;
using Energy = OutputLow<37>;

using SensorHi1 = InputLow<22>;
using SensorHi2 = InputLow<24>;
using SensorHi3 = InputLow<26>;
using SensorLo3 = InputLow<28>;


template<typename = void>
struct Io {
  ValveIn1 valveIn1;
  ValveIn2 valveIn2;
  RecirPump1 recirPump1;
  RecirPump2 recirPump2;
  AqueductPump aqueductPump;
  FillPump fillPump;
  Energy energy;

  SensorHi1 sensorHi1;
  SensorHi2 sensorHi2;
  SensorHi3 sensorHi3;
  SensorLo3 sensorLo3;
};

}
