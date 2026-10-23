#pragma once
#include <stdint.h>

void stream_init(void);
void stream_run(void);
void stream_draw_640x200_irgb(const uint8_t *frame);
void stream_draw_no_signal(void);
void stream_report(const char* s);
