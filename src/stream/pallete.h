#pragma once
#include <stdint.h>

static const uint8_t c128_ribg_to_rgb332[16] = {
    0x00, // 0  0000 black
    0x14, // 1  000G green
    0xC0, // 2  00R0 red
    0xB4, // 3  00RG dark yellow (0xA8 for CGA-style brown)
    0x49, // 4  0I00 dark gray
    0x5D, // 5  0I0G light green
    0xC0, // 6  0IR0 light red
    0xFD, // 7  0IRG yellow
    0x02, // 8  B000 blue
    0x16, // 9  B00G cyan
    0xA2, // 10 B0R0 magenta
    0xB6, // 11 B0RG light gray
    0x4B, // 12 BI00 light blue
    0x5F, // 13 BI0G light cyan
    0xEB, // 14 BIR0 light magenta
    0xFF, // 15 BIRG white
};
