#pragma once

#ifdef PROFILER_ENABLED

#include "gba.h"
#include "mgba.h"

char profilerBuffer[100] = {};
uint8 profilerTop;
const char *hexLUT = "0123456789ABCDEF";

void initProfiler() {
    profilerBuffer[99] = 0;
}

static inline
char *uint16ToHex(uint16 a) {
    profilerTop = 99;
    do {
        profilerTop--;
        profilerBuffer[profilerTop] = hexLUT[a & 0xF];
        a = a >> 4;
    } while (a > 0);
    return profilerBuffer + profilerTop;
}

struct ProfilerState {
    uint16 vcountStart;
    inline
    ProfilerState(const char *name, int len) {
        mgba_puts(MGBA_LOG_INFO, "(", 1);
        mgba_puts(MGBA_LOG_INFO, name, len);
        vcountStart = REG_VCOUNT;
    }
    inline
    ~ProfilerState() {
        uint16 vcountEnd = REG_VCOUNT;
        char *hex = uint16ToHex(vcountEnd - vcountStart);
        mgba_puts(MGBA_LOG_DEBUG, hex, 99 - profilerTop);
        mgba_puts(MGBA_LOG_DEBUG, ")", 1);
    }
};

#define PROFILE(name) ProfilerState ps ## __LINE__ (name, strlen(name))

#ifdef MICRO_PROFILER_ENABLED

#define MICRO_PROFILE_START() do {REG_TIMER0_CONTROLLER = TIMER_ENABLE | TIMER_FREQ_SYS;} while(0)
#define MICRO_PROFILE_STOP(name) do {                       \
        mgba_puts(MGBA_LOG_DEBUG, name, strlen(name));      \
        char *hex = uint16ToHex(REG_TIMER0_DATA);           \
        mgba_puts(MGBA_LOG_DEBUG, hex, 99 - profilerTop);   \
        REG_TIMER0_CONTROLLER = 0;                          \
    } while(0)

#else // MICRO_PROFILER_ENABLED

#define MICRO_PROFILE_START() do{}while(0)
#define MICRO_PROFILE_STOP(name) do{}while(0)

#endif // MICRO_PROFILER_ENABLED

#else  // PROFILER_ENABLED

#define initProfiler() do{}while(0)
#define PROFILE(name) do{}while(0)
#define MICRO_PROFILE_START() do{}while(0)
#define MICRO_PROFILE_STOP(name) do{}while(0)

#endif // PROFILER_ENABLED