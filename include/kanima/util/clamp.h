#ifndef CLAMP_H
#define CLAMP_H

#include <algorithm>

// clamp not available in C++11

template <typename T>
inline T clamp(T val, T minVal, T maxVal) {
    return std::max(minVal, std::min(val, maxVal));
}

#endif // CLAMP_H
