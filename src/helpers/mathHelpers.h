#pragma once

#include <iostream>
#include <algorithm>

inline double clamp(double value, double minVal, double maxVal)
{
    return std::max(minVal, std::min(value, maxVal));
}

inline double lerp(double a, double b, double t)
{
    t = clamp(t, 0.0, 1.0);
    return a + t * (b - a);
}