#pragma once

#include "gba.h"
#include "profiler.h"

#include <cmath>

#define FIXED_SHIFT 8
#define FIXED_SCALE (1 << FIXED_SHIFT)
#define FIXED_SCALEF ((float)FIXED_SCALE)

typedef int Fix24b8;
struct V2i {
    int32 x, y;
};

struct V2fix32b16 {
    int32 x1, y1;
    uint32 x2, y2;
};

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

static inline
Fix24b8 fix24b8(float a) {
    return (Fix24b8)(a * FIXED_SCALEF);
}

static inline
float fix24b8ToFloat(Fix24b8 a) {
    return ((float)a)/FIXED_SCALEF;
}

static inline
V2fix24b8 operator+(V2fix24b8 a, V2fix24b8 b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

static inline
Fix24b8 mul(Fix24b8 a, Fix24b8 b) {
    return (a*b) >> FIXED_SHIFT;
}

// TODO: 
static inline
Fix24b8 fxdiv(Fix24b8 a, Fix24b8 b) {
    return a*FIXED_SCALE/b;
}

static inline
V2fix24b8 operator*(V2fix24b8 v, Fix24b8 n) {
    v.x = mul(v.x, n);
    v.y = mul(v.y, n);
    return v;
}


static inline
V2fix24b8 operator*(Fix24b8 n, V2fix24b8 v) {
    v.x = mul(v.x, n);
    v.y = mul(v.y, n);
    return v;
}


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

static inline
V2fix24b8 operator/(V2fix24b8 v, Fix24b8 x) {
    return V2fix24b8{.x = fxdiv(v.x, x), .y = fxdiv(v.y, x)};
}

static inline
V2i v2fix24b8ToV2i(V2fix24b8 v) {
    return V2i{.x = (v.x >> FIXED_SHIFT), .y = (v.y >> FIXED_SHIFT)};
}

static inline
V2fix24b8 v2iToV2fix24b8(V2i v) {
    return V2fix24b8{.x = v.x << FIXED_SHIFT, .y = v.y << FIXED_SHIFT};
}

static inline
int16 fix24b8ToInt(Fix24b8 n) {
    return n >> FIXED_SHIFT;
}

static inline
V2fix24b8 v2fix24b8(float x, float y) {
    return V2fix24b8{.x = fix24b8(x), .y = fix24b8(y)};
}

static inline
Fix24b8 sqrLength(V2fix24b8 v) {
    return mul(v.x, v.x) + mul(v.y, v.y);
}

static inline
Fix24b8 max(Fix24b8 a, Fix24b8 b) {
    if (a < b) {
        return b;
    } else {
        return a;
    }
}

static inline
Fix24b8 min(Fix24b8 a, Fix24b8 b) {
    if (a < b) {
        return b;
    } else {
        return a;
    }
}

// alpha max plus beta min, largest error is 2.4%, could be lowered to -1.13% at the cost of a lot more addition/multiplication
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
static inline
V2fix24b8 normalize(V2fix24b8 v) {
    //MICRO_PROFILE_START();
    Fix24b8 vLength = length(v);
    //MICRO_PROFILE_STOP("length");
    //MICRO_PROFILE_START();
    V2fix24b8 ret = v/vLength;
    //MICRO_PROFILE_STOP("div");
    return ret;
}