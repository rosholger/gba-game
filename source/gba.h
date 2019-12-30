#pragma once

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef signed int int32;
typedef signed short int16;
typedef signed char int8;


// We use 4bpp
typedef uint32 Tile[8];
typedef Tile TileBlock[512];

typedef uint16 MapEntry;
typedef MapEntry MapBlock[32*32];

typedef struct ObjectAttributes {
    uint16 attr0;
    uint16 attr1;
    uint16 attr2;
    uint16 pad;
} __attribute__((packed, aligned(4))) ObjectAttributes;


#define MEM_OAM  ((volatile ObjectAttributes *)0x07000000)

#define REG_DISPLAY_CONTROLLER (*((volatile uint32*)0x04000000))
#define VIDEO_MODE_0 0x0000
#define VIDEO_MODE_1 0x0001
#define VIDEO_MODE_2 0x0002
#define VIDEO_MODE_3 0x0003
#define ENABLE_BG0 0x0100
#define ENABLE_BG1 0x0200
#define ENABLE_BG2 0x0400
#define ENABLE_BG3 0x0800
#define ENABLE_OBJECTS 0x1000
#define MAPPINGMODE_1D 0x0040

#define REG_BG0_CONTROLLER (*(volatile uint16*)(0x04000008))
#define REG_BG1_CONTROLLER (*(volatile uint16*)(0x0400000A))
#define REG_BG2_CONTROLLER (*(volatile uint16*)(0x0400000C))
#define REG_BG3_CONTROLLER (*(volatile uint16*)(0x0400000E))

#define REG_BG0_REGULAR_X_OFFSET (*(volatile uint16*)(0x04000010))
#define REG_BG0_REGULAR_Y_OFFSET (*(volatile uint16*)(0x04000012))
#define REG_BG1_REGULAR_X_OFFSET (*(volatile uint16*)(0x04000014))
#define REG_BG1_REGULAR_Y_OFFSET (*(volatile uint16*)(0x04000016))
#define REG_BG2_REGULAR_X_OFFSET (*(volatile uint16*)(0x04000018))
#define REG_BG2_REGULAR_Y_OFFSET (*(volatile uint16*)(0x0400001A))
#define REG_BG3_REGULAR_X_OFFSET (*(volatile uint16*)(0x0400001C))
#define REG_BG3_REGULAR_Y_OFFSET (*(volatile uint16*)(0x0400001E))

#define REG_BG2_MATRIX_A (*(volatile int16*)(0x04000020))
#define REG_BG2_MATRIX_B (*(volatile int16*)(0x04000022))
#define REG_BG2_MATRIX_C (*(volatile int16*)(0x04000024))
#define REG_BG2_MATRIX_D (*(volatile int16*)(0x04000026))
#define REG_BG2_AFFINE_X_OFFSET (*(volatile int32*)(0x04000028))
#define REG_BG2_AFFINE_Y_OFFSET (*(volatile int32*)(0x0400002C))
#define REG_BG3_MATRIX_A (*(volatile int16*)(0x04000030))
#define REG_BG3_MATRIX_B (*(volatile int16*)(0x04000032))
#define REG_BG3_MATRIX_C (*(volatile int16*)(0x04000034))
#define REG_BG3_MATRIX_D (*(volatile int16*)(0x04000036))
#define REG_BG3_AFFINE_X_OFFSET (*(volatile int32*)(0x04000038))
#define REG_BG3_AFFINE_Y_OFFSET (*(volatile int32*)(0x0400003C))

#define BG_REGULAR_32x32 0
#define BG_REGULAR_64x32 0x4000
#define BG_REGULAR_32x64 0x8000
#define BG_REGULAR_64x64 0xC000
#define BG_AFFINE_16x16 0
#define BG_AFFINE_32x32 0x4000
#define BG_AFFINE_64x64 0x8000
#define BG_AFFINE_128x128 0xC000

#define BG_TILEMAP_MASK 0x1F00
#define BG_TILEMAP_SHIFT 8
#define BG_TILEMAP(n) ((n)<<BG_TILEMAP_SHIFT)

#define BG_PRIO_MASK 0x0003
#define BG_PRIO_SHIFT 0
#define BG_PRIO(n) ((n)<<BG_PRIO_SHIFT)

#define BG_TILEBLOCK_MASK 0x000C
#define BG_TILEBLOCK_SHIFT 2
#define BG_TILEBLOCK(n) ((n)<<BG_TILEBLOCK_SHIFT)

#define BG_SIZE_MASK 0xC000
#define BG_SIZE_SHIFT 14
#define BG_SIZE(n) ((n)<<BG_SIZE_SHIFT)

#define BG_4BPP (0 << 7)

#define MAP_ENTRY_PALBANK_MASK 0xF000
#define MAP_ENTRY_PALBANK_SHIFT 12
#define MAP_ENTRY_PALBANK(n) ((n)<<MAP_ENTRY_PALBANK_SHIFT)

#define MAP_ENTRY_HFLIP 0x0400	//!< Horizontal flip
#define MAP_ENTRY_VFLIP 0x0800	//!< Vertical flip

#define MAP_ENTRY_ID_MASK 0x03FF
#define MAP_ENTRY_ID_SHIFT 0
#define MAP_ENTRY_ID(n) ((n)<<MAP_ENTRY_ID_SHIFT)

#define MAP_ENTRY_FLIP_MASK 0x0C00
#define MAP_ENTRY_FLIP_SHIFT 10
#define MAP_ENTRY_FLIP(n) ((n)<<MAP_ENTRY_FLIP_SHIFT)

struct PaletteBank {
    uint16 colors[16];
};

#define MEM_SPRITE_PALETTE ((PaletteBank *)0x05000200)
#define MEM_BG_PALETTE ((PaletteBank *)0x05000000)
#define MEM_VRAM ((volatile uint32*)0x06000000)
#define MEM_TILE ((TileBlock*)MEM_VRAM)
#define MEM_MAP ((MapBlock*)MEM_VRAM)

#define REG_VCOUNT (*((volatile uint16*)0x04000006))
#define REG_KEYINPUT  (* (volatile uint16*) 0x4000130)

struct DMAChannel
{
	volatile const void * volatile src;
	volatile void * volatile dst;
	volatile uint16 count;
	volatile uint16 controller;
};

#define REG_DMA ((volatile DMAChannel*)(0x40000B0))
#define DMA_DST_INC 0
#define DMA_DST_DEC 0x0020
#define DMA_DST_FIXED 0x0040
#define DMA_DST_INC_RESET 0x0060
#define DMA_SRC_INC 0
#define DMA_SRC_DEC 0x0080
#define DMA_SRC_FIXED 0x0100
#define DMA_REPEAT 0x0200
#define DMA_16 0
#define DMA_32 0x0400
#define DMA_AT_NOW 0
#define DMA_GAMEPAK 0x0800
#define DMA_AT_VBLANK 0x1000
#define DMA_AT_HBLANK 0x2000
#define DMA_AT_SPECIAL 0x3000
#define DMA_AT_FIFO 0x3000
#define DMA_AT_REFRESH 0x3000
#define DMA_IRQ 0x4000
#define DMA_ENABLE 0x8000

#define REG_TIMER0_DATA (*((volatile uint16*)0x4000100))
#define REG_TIMER0_CONTROLLER (*((volatile uint16*)0x4000102))
#define REG_TIMER1_DATA (*((volatile uint16*)0x4000104))
#define REG_TIMER1_CONTROLLER (*((volatile uint16*)0x4000106))
#define REG_TIMER2_DATA (*((volatile uint16*)0x4000108))
#define REG_TIMER2_CONTROLLER (*((volatile uint16*)0x400010A))
#define REG_TIMER3_DATA (*((volatile uint16*)0x400010C))
#define REG_TIMER3_CONTROLLER (*((volatile uint16*)0x400010E))

#define TIMER_FREQ_SYS			 0	//!< System clock timer (16.7 Mhz)
#define TIMER_FREQ_1			 0	//!< 1 cycle/tick (16.7 Mhz)
#define TIMER_FREQ_64		0x0001	//!< 64 cycles/tick (262 kHz)
#define TIMER_FREQ_256		0x0002	//!< 256 cycles/tick (66 kHz)
#define TIMER_FREQ_1024	0x0003	//!< 1024 cycles/tick (16 kHz)
#define TIMER_CASCADE		0x0004	//!< Increment when preceding timer overflows
#define TIMER_IRQ			0x0040	//!< Enable timer irq
#define TIMER_ENABLE		0x0080	//!< Enable timer

#define KEY_A        0x0001
#define KEY_B        0x0002
#define KEY_SELECT   0x0004
#define KEY_START    0x0008
#define KEY_RIGHT    0x0010
#define KEY_LEFT     0x0020
#define KEY_UP       0x0040
#define KEY_DOWN     0x0080
#define KEY_R        0x0100
#define KEY_L        0x0200

#define KEY_MASK     0xFC00

static inline
uint32 getKeyState(uint16 key_code) {
    return !(key_code & (REG_KEYINPUT | KEY_MASK) );
}

static inline
void setSprite4bpp(volatile ObjectAttributes *sprite) {
    sprite->attr0 = sprite->attr0 & 0xDFFF;
}

static inline
void setSprite8bpp(volatile ObjectAttributes *sprite) {
    sprite->attr0 = sprite->attr0 | 0x2000;
}

static inline
void setSpriteSize8x8(volatile ObjectAttributes *sprite) {
    sprite->attr0 = sprite->attr0 & 0x3FFF;
    sprite->attr1 = sprite->attr0 & 0x3FFF;
}

static inline
void setSpriteSize16x16(volatile ObjectAttributes *sprite) {
    sprite->attr0 = sprite->attr0 & 0x3FFF;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x4000;
}

static inline
void setSpriteSize8x16(volatile ObjectAttributes *sprite) {
    sprite->attr0 = sprite->attr0 & 0x3FFF;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x8000;
}

static inline
void setSpriteSize16x8(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0x4000;
    sprite->attr1 = sprite->attr0 & 0x3FFF;
}

static inline
void setSpriteSize32x8(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0x4000;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x4000;
}

static inline
void setSpriteSize8x32(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0x4000;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x8000;
}

static inline
void setSpriteSize32x32(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0x8000;
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0x4000;
    sprite->attr1 = sprite->attr0 & 0x3FFF;
}

static inline
void setSpriteSize32x16(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0x8000;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x4000;
}

static inline
void setSpriteSize16x32(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0x8000;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x8000;
}

static inline
void setSpriteSize64x64(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0xC000;
    sprite->attr1 = sprite->attr0 & 0x3FFF;
}

static inline
void setSpriteSize64x32(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0xC000;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x4000;
}

static inline
void setSpriteSize32x64(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0x3FFF) | 0xC000;
    sprite->attr1 = (sprite->attr0 & 0x3FFF) | 0x8000;
}

static inline
void setSpritePos(volatile ObjectAttributes *sprite, uint8 x, uint8 y) {
    sprite->attr0 = (sprite->attr0 & 0xFF00) | y;
    sprite->attr1 = (sprite->attr1 & 0xFF00) | x;
}

static inline
uint8 getSpriteX(volatile ObjectAttributes *sprite) {
    return sprite->attr1 & 0xFF;
}

static inline
uint8 getSpriteY(volatile ObjectAttributes *sprite) {
    return sprite->attr0 & 0xFF;
}

static inline
void vsync() {
    while (REG_VCOUNT >= 160) {}
    while (REG_VCOUNT < 160) {}
}

static inline
void disable(volatile ObjectAttributes *sprite) {
    sprite->attr0 = (sprite->attr0 & 0xFEFF) | 0x0200;
}

static inline
void enable(volatile ObjectAttributes *sprite) {
    sprite->attr0 = sprite->attr0 & 0xFCFF;
}

static inline
void enable(volatile DMAChannel *channel) {
    channel->controller |= DMA_ENABLE;
}


static inline
void disable(volatile DMAChannel *channel) {
    channel->controller &= ~DMA_ENABLE;
}

#define COLOR(red, green, blue) (red | green << 5 | blue << 10)