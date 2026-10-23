#include "capture.h"
#include "capture.pio.h"

#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/vreg.h"
#include "pico/stdlib.h"

#define CLOCK_FREQ_KHZ 192000
#define DOT_CLOCK_KHZ 16000
_Static_assert(
  CLOCK_FREQ_KHZ / DOT_CLOCK_KHZ * DOT_CLOCK_KHZ == CLOCK_FREQ_KHZ,
  "Synchronize the clocks"
);
#define DATA_BITS 4
#define DATA_BASE_GPIO 0
#define HSYNC_GPIO 4
#define VSYNC_GPIO 5

#define PIXEL_PHASE 8
#define CYRLES_PER_PIXEL (CLOCK_FREQ_KHZ / DOT_CLOCK_KHZ)
_Static_assert(CYRLES_PER_PIXEL == 12, "Hardcoded in capture.pi");
_Static_assert(PIXEL_PHASE < CYRLES_PER_PIXEL, "Not inside of pixel");

#define LINE_WIDTH 640
#define LINES_COUNT 200
#define LEFT_MARGIN 180
#define TOP_MARGIN 34

#define MAX_CAPTURE_BUFFER_SIZE (LINE_WIDTH * (LINES_COUNT + TOP_MARGIN))

static PIO pio = pio0;
static uint32_t state_machine;
static uint32_t capture_pio_offset;
static int dma_chan;

static uint32_t capture_buffer[MAX_CAPTURE_BUFFER_SIZE / sizeof(uint32_t)];

static bool capture_frame(void);
static inline bool wait_vsync(void);
static void reset_capture(void);
static void start_capture();
static bool capture_stop(void);

void capture_init(void) {
  set_sys_clock_khz(CLOCK_FREQ_KHZ, true);

  capture_pio_offset = pio_add_program(pio, &capture_program);
  state_machine = pio_claim_unused_sm(pio, true);

  for (uint i = 0; i < DATA_BITS + 2; i++) {
    pio_gpio_init(pio, DATA_BASE_GPIO + i);
  }
  pio_sm_set_consecutive_pindirs(pio, state_machine, DATA_BASE_GPIO,
                                 DATA_BITS + 2, false);

  pio_sm_config c = capture_program_get_default_config(capture_pio_offset);
  sm_config_set_in_pins(&c, DATA_BASE_GPIO);
  // Shift reads right, autopush when 32 bits are in the shift register
  sm_config_set_in_shift(&c, true, true, 32);
  // Use the system clock as we adjust it to match the dot clock
  sm_config_set_clkdiv(&c, 1.0f);
  pio_sm_init(pio, state_machine, capture_pio_offset, &c);

  dma_chan = dma_claim_unused_channel(true);
  dma_channel_config dc = dma_channel_get_default_config(dma_chan);
  channel_config_set_transfer_data_size(&dc, DMA_SIZE_32);
  channel_config_set_read_increment(
      &dc, false); // Read from the same PIO RX FIFO address
  channel_config_set_write_increment(
      &dc, true); // Write to consecutive addresses in the capture buffer
  uint dreq = pio_get_dreq(pio, state_machine, false); // false = RX
  channel_config_set_dreq(&dc, dreq);                  // false = RX
  dma_channel_configure(dma_chan, &dc, capture_buffer, &pio->rxf[state_machine],
                        MAX_CAPTURE_BUFFER_SIZE / sizeof(uint32_t), false);
}

const uint8_t *capture(void) {
  if (!capture_frame()) {
    return NULL;
  }
  const uint8_t *src = (const uint8_t *)capture_buffer;
  return src + TOP_MARGIN * LINE_WIDTH / 2;
}

static inline bool wait_vsync(void) {
  absolute_time_t d = make_timeout_time_ms(100);
  while (!gpio_get(VSYNC_GPIO)) {
    if (time_reached(d))
      return false;
  }
  while (gpio_get(VSYNC_GPIO)) {
    if (time_reached(d))
      return false;
  }
  return true;
}

static void reset_capture(void) {
  pio_sm_set_enabled(pio, state_machine, false);
  pio_sm_clear_fifos(pio, state_machine);
  pio_sm_restart(pio, state_machine);
  pio_sm_clkdiv_restart(pio, state_machine);
  pio_sm_exec(pio, state_machine, pio_encode_jmp(capture_pio_offset));

  dma_channel_set_write_addr(dma_chan, capture_buffer, false);
  dma_channel_set_trans_count(
      dma_chan, MAX_CAPTURE_BUFFER_SIZE / sizeof(uint32_t), false);
}

static void start_capture() {
  dma_channel_start(dma_chan);
  pio_sm_put_blocking(pio, state_machine, LINE_WIDTH - 1);
  pio_sm_put_blocking(pio, state_machine,
                      (LEFT_MARGIN - 2) * CYRLES_PER_PIXEL + PIXEL_PHASE);
  pio_sm_set_enabled(pio, state_machine, true);
}

static bool capture_stop(void) {
  pio_sm_set_enabled(pio, state_machine, false);
  uint32_t captured = (dma_channel_hw_addr(dma_chan)->write_addr -
                       (uint32_t)(uintptr_t)capture_buffer) /
                      4;
  dma_channel_abort(dma_chan);
  return captured != 0;
}

static bool capture_frame(void) {
  reset_capture();
  if (!wait_vsync()) {
    return false;
  }
  start_capture();

  // TODO: wait for dma completion instead to not drop to 30fps  
  if (!wait_vsync()) {
    capture_stop();
    return false;
  }
  return capture_stop();
}
