#pragma once
#include <stdint.h>

void stream_init(void);
void stream_run(void);
void stream_draw_640x200(const uint8_t *frame, const uint8_t palette[16]);
void stream_draw_no_signal(void);
void stream_report(const char* s);
