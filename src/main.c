#include <stdio.h>

#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "capture.h"
#include "stream.h"

int main() {
  stdio_init_all();
  printf("Commodore 128 -> DVI converter\n");

  capture_init();
  stream_init();
  stream_draw_no_signal();

  multicore_launch_core1(stream_run);

  uint32_t last_report = time_us_32();
  int frames = 0;
  char fps_buf[64];
  for (;;) {
    uint32_t now = time_us_32();
    if (now - last_report > 1000000) {
      sprintf(fps_buf, "FPS: %.2d   ", frames);
      printf("%s\n", fps_buf);
      stream_report(fps_buf);
      last_report = now;
      frames = 0;
    }

    ++frames;
    const uint8_t *frame = capture();
    if (frame) {
      stream_draw_640x200_irgb(frame);
    } else {
      frames = 0;
    }
  }

  return 0;
}
