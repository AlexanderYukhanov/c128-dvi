# C128 RGBI to HDMI

Raspberry Pi Pico 2 firmware and hardware that converts the Commodore 128's digital RGBI output (80-column VDC) to HDMI/DVI, so you can use the C128 with a modern display.

> **Status:** Work in progress

## Features

- Converts C128 RGBI + sync signals to a DVI/HDMI video signal
- Runs on a Raspberry Pi Pico 2 (RP2350), no FPGA or extra scaler required
- [Output resolution / refresh rate, e.g. 640x480 @ 60 Hz]
- [Scaling mode: pixel-doubling, aspect ratio handling, etc.]
- [Supported C128 modes: 80-column]

## Bill of Materials

| Part | Qty | Notes |
|------|-----|-------|
| Raspberry Pi Pico 2 or Pico 2 W | 1 | Wi-Fi on the 2 W is not used |
| DVI Sock for Pico | 1 | Provides the HDMI/DVI connector |
| [8-pin DIN connector / cable] | 1 | Connection to the C128 RGBI port |

No level shifters or resistors are needed: the RP2350 GPIOs are 5V tolerant
while the Pico is powered, so the C128's 5V TTL signals connect directly.

> **Note:** Power the Pico before (or together with) the C128. Driving 5V into
> the GPIOs while the Pico is unpowered is outside the 5V-tolerance spec.

### Wiring

| C128 RGBI signal | Pico GPIO |
|------------------|-----------|
| Red | GPIO 0 |
| Green | GPIO 1 |
| Blue | GPIO 2 |
| Intensity | GPIO 3 |
| H-Sync | GPIO 4 |
| V-Sync | GPIO 5 |
| GND | GND |


## Building the Firmware

### Prerequisites

- [Pico SDK](https://github.com/raspberrypi/pico-sdk) 
- CMake and an ARM GNU toolchain


## Demo

TBD (photos / video of the C128 on an HDMI display)

## License

MIT

## Acknowledgements

- Thanks https://github.com/charlysan/pico-ttl2dvi for inspiration.
