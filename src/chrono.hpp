#pragma once

#include <estd/chrono.h>

namespace chr = estd::chrono;

constexpr auto operator ""_s(unsigned long long s) {
    return chr::seconds{s};
}

constexpr auto operator ""_ms(unsigned long long ms) {
    return chr::milliseconds{ms};
}
