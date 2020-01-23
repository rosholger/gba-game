#pragma once

#include "gba.h"
#include "profiler.h"

#include <cmath>

#define FIXED_SHIFT 8
#define FIXED_SCALE (1 << FIXED_SHIFT)
#define FIXED_SCALEF ((float)FIXED_SCALE)

// TODO: Make lots of this stuff constexpr in order to use it for template arguments

typedef int Fix24b8;
struct V2i {
    int32 x, y;
};

struct V2fix32b16 {
    int32 x1, y1;
    uint32 x2, y2;
};

// FIXME
int32 abs(int32 n) {
    if (n < 0) {
        return -n;
    } else {
        return n;
    }
}

struct V2fix24b8 {
    Fix24b8 x, y;
};

constexpr
Fix24b8 fix24b8(float a) {
    return (Fix24b8)(a * FIXED_SCALEF);
}

constexpr
float fix24b8ToFloat(Fix24b8 a) {
    return ((float)a)/FIXED_SCALEF;
}

constexpr
V2fix24b8 operator+(V2fix24b8 a, V2fix24b8 b) {
    return V2fix24b8{a.x + b.x, a.y + b.y};
}

constexpr
Fix24b8 mul(Fix24b8 a, Fix24b8 b) {
    return (a*b) >> FIXED_SHIFT;
}

// TODO: 
constexpr
Fix24b8 fxdiv(Fix24b8 a, Fix24b8 b) {
    return a*FIXED_SCALE/b;
}

constexpr
V2fix24b8 operator*(V2fix24b8 v, Fix24b8 n) {
    return V2fix24b8{mul(v.x, n), mul(v.y, n)};
}

constexpr
V2fix24b8 operator*(Fix24b8 n, V2fix24b8 v) {
    return v*n;
}

// FIXME
static inline
V2fix32b16 operator+(V2fix32b16 a, V2fix32b16 b) {
    V2fix32b16 ret;
    ret.x1 = a.x1 + b.x1;
    ret.y1 = a.y1 + b.y1;
    ret.x2 = a.x2 + b.x2;
    ret.y2 = a.y2 + b.y2;
    if (ret.x2 > 0xFFFF) {
        ret.x1 += ret.x1 >= 0 ? 1 : -1;
        ret.x2 = ret.x2 >> 8;
    }
    if (ret.y2 > 0xFFFF) {
        ret.y1 += ret.y1 >= 0 ? 1 : -1;
        ret.y2 = ret.y2 >> 8;
    }
    return ret;
}

// FIXME
static inline
V2fix32b16 operator+(V2fix32b16 a, V2fix24b8 b) {
    V2fix32b16 ret;
    ret.x1 = a.x1 + (b.x >> FIXED_SHIFT);
    ret.y1 = a.y1 + (b.y >> FIXED_SHIFT);
    ret.x2 = a.x2 + (abs(b.x) & 0xFFFF);
    ret.y2 = a.y2 + (abs(b.y) & 0xFFFF);
    if (ret.x2 > 0xFFFF) {
        ret.x1 += ret.x1 >= 0 ? 1 : -1;
        ret.x2 = ret.x2 >> 8;
    }
    if (ret.y2 > 0xFFFF) {
        ret.y1 += ret.y1 >= 0 ? 1 : -1;
        ret.y2 = ret.y2 >> 8;
    }
    return ret;
}

// FIXME
static inline
V2fix32b16 operator+(V2fix24b8 b, V2fix32b16 a) {
    V2fix32b16 ret;
    ret.x1 = a.x1 + (b.x >> FIXED_SHIFT);
    ret.y1 = a.y1 + (b.y >> FIXED_SHIFT);
    ret.x2 = a.x2 + (abs(b.x) & 0xFFFF);
    ret.y2 = a.y2 + (abs(b.y) & 0xFFFF);
    if (ret.x2 > 0xFFFF) {
        ret.x1 += ret.x1 >= 0 ? 1 : -1;
        ret.x2 = ret.x2 >> 8;
    }
    if (ret.y2 > 0xFFFF) {
        ret.y1 += ret.y1 >= 0 ? 1 : -1;
        ret.y2 = ret.y2 >> 8;
    }
    return ret;
}

constexpr
V2fix24b8 operator/(V2fix24b8 v, Fix24b8 x) {
    return V2fix24b8{.x = fxdiv(v.x, x), .y = fxdiv(v.y, x)};
}

constexpr
V2i v2fix24b8ToV2i(V2fix24b8 v) {
    return V2i{.x = (v.x >> FIXED_SHIFT), .y = (v.y >> FIXED_SHIFT)};
}

constexpr
V2fix24b8 v2iToV2fix24b8(V2i v) {
    return V2fix24b8{.x = v.x << FIXED_SHIFT, .y = v.y << FIXED_SHIFT};
}

constexpr 
int32 fix24b8ToInt(Fix24b8 n) {
    return n / FIXED_SCALE;
}

constexpr
Fix24b8 intToFix24b8(int n) {
    return n * FIXED_SCALE;
}

constexpr
V2fix24b8 v2fix24b8(float x, float y) {
    return V2fix24b8{.x = fix24b8(x), .y = fix24b8(y)};
}

constexpr
Fix24b8 sqrLength(V2fix24b8 v) {
    return mul(v.x, v.x) + mul(v.y, v.y);
}

// FIXME
static inline
Fix24b8 max(Fix24b8 a, Fix24b8 b) {
    if (a < b) {
        return b;
    } else {
        return a;
    }
}

// FIXME
static inline
Fix24b8 min(Fix24b8 a, Fix24b8 b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

// alpha max plus beta min, largest error is 2.4%, could be lowered to -1.13% at the cost of a lot more addition/multiplication
// FIXME: make constexpr
// FIXME: make one that returns Fix16b16 instead
static inline
Fix24b8 length(V2fix24b8 v) {
    Fix24b8 minVal;
    Fix24b8 maxVal;
    if (abs(v.x) < abs(v.y)) {
        minVal = abs(v.x);
        maxVal = abs(v.y);
    } else {
        maxVal = abs(v.x);
        minVal = abs(v.y);
    }
    return max(maxVal, mul(fix24b8(29.0f/32.0f), maxVal) + mul(fix24b8(61.0f/128.0f), minVal));
}

// TODO: micro optimize this, atm it takes ~2 scanlines to run, ie way too long!
// TODO: Maybe use some sort of newtons method, needs a decent first guess, check if ID fast inverse sqrt is fastest etc.
// FIXME: should use fxdiv, but we could use unscaledLength and then just divide to get back to correct scale.
static inline
V2fix24b8 normalize(V2fix24b8 v) {
    MICRO_PROFILE_START();
    Fix24b8 vLength = length(v);
    MICRO_PROFILE_STOP("length");
    MICRO_PROFILE_START();
    V2fix24b8 ret = v/vLength;
    MICRO_PROFILE_STOP("div");
    return ret;
}

constexpr
Fix24b8 floor(Fix24b8 n) {
    return n & (0xFFFFFF00);
}

constexpr
Fix24b8 sign(Fix24b8 n) {
    return (n > 0) - (n < 0);
}