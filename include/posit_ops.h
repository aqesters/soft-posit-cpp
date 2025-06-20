#ifndef POSIT_OPS_H
#define POSIT_OPS_H

#include "softposit.h"

// Define overloaded operators for posit32_t
inline posit32_t operator+(const posit32_t& a, const posit32_t& b) {
    return p32_add(a, b);
}

inline posit32_t operator-(const posit32_t& a, const posit32_t& b) {
    return p32_sub(a, b);
}

inline posit32_t operator*(const posit32_t& a, const posit32_t& b) {
    return p32_mul(a, b);
}

inline posit32_t operator/(const posit32_t& a, const posit32_t& b) {
    return p32_div(a, b);
}

// Conversion functions for posit
inline posit32_t ui32_to_p32(uint32_t a) {
    posit32_t p;
    p.v = a;
    return p;
}

// Conversion functions for posit
inline posit32_t int_to_posit32(int value) {
    return ui32_to_p32(value);
}

// Conversion functions for posit
inline posit32_t double_to_posit32(double value) {
    return convertDoubleToP32(value);
}

inline double posit32_to_double(posit32_t value) {
    return convertP32ToDouble(value);
}

#endif // POSIT_OPS_H