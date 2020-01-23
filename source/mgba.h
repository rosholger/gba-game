/*
 mgba.h
 Copyright (c) 2016 Jeffrey Pfau

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MGBA_H
#define MGBA_H

#ifdef __cplusplus
extern "C" {
#endif

#define MGBA_LOG_FATAL 0
#define MGBA_LOG_ERROR 1
#define MGBA_LOG_WARN 2
#define MGBA_LOG_INFO 3
#define MGBA_LOG_DEBUG 4

#define REG_DEBUG_ENABLE (volatile unsigned short*) 0x4FFF780
#define REG_DEBUG_FLAGS (volatile unsigned short*) 0x4FFF700
#define REG_DEBUG_STRING (char*) 0x4FFF600
#define REG_DEBUG_STRING_V (char*) 0x4FFF600

bool mgba_open(void);
void mgba_close(void);

void mgba_poprintf(int level, const char* string, ...);
void mgba_printf(int level, const char* string, ...);
bool mgba_console_open(void);

    // TODO: Optimize me more heavily please
static inline
void * vmemcpy (volatile void *dest, const void *src, size_t len) {
    volatile char *d = (volatile char *)dest;
    const char *s = (const char *)src;
    while (len--)
        *d++ = *s++;
    return (void *)dest;
}

static inline
void mgba_puts(int level, const char* ptr, int length) {
	level &= 0x7;
    vmemcpy(REG_DEBUG_STRING, ptr, length);
	*REG_DEBUG_FLAGS = level | 0x100;
}



#ifdef __cplusplus
}
#endif

#endif
