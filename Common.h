#pragma once
#include <algorithm>
#include <cmath>

static inline float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}
