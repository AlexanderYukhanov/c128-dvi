#pragma once
#include <stdint.h>

static const uint8_t c128_ibgr_to_rgb332[16] = {
    0x00, // 0  0000 black
    0xA0, // 1  000R red
    0x14, // 2  00G0 green
    0xB4, // 3  00GR dark yellow (0xA8 for CGA-style brown)
    0x03, // 4  0B00 blue
    0xA2, // 5  0B0R magenta
    0x16, // 6  0BG0 cyan
    0xB6, // 7  0BGR light gray
    0x49, // 8  I000 dark gray
    0xE9, // 9  I00R light red
    0x5D, // 10 I0G0 light green
    0xFD, // 11 I0GR yellow
    0x4B, // 12 IB00 light blue
    0xEB, // 13 IB0R light magenta
    0x5F, // 14 IBG0 light cyan
    0xFF, // 15 IBGR white
};
