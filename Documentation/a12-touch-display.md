# Galaxy A12 touch and display bring-up

## Touchscreen findings

The live phone exposes the touch device as Android input `sec_touchscreen`, with ten multitouch slots and X=0..719, Y=0..1599. Its kernel binds an SPI driver named `ilit_ts`.

Samsung's SM-A125F MT6765 device tree describes an ILITEK SPI4 device at chip-select 0, up to 5 MHz. It gives GPIO 10 as an active-low interrupt, GPIO 172 as reset, and GPIOs 13-16 as SPI4 pins. The node is named `ilitek_ili9882x` and has compatible `iliteck,ili9882x-spi`, while its firmware property uses the legacy filename `tsp_ilitek/ili9881x_a12.hex`. The matching touch LCD type is `0x0a7250`. This comes from Samsung's A125FXXU1AUC3 source and has not been checked against the phone's live device tree.

The MT6765 SoC device tree maps SPI4 at physical address `0x11014000` for `0x1000` bytes and assigns it GIC SPI interrupt 95, level-low. This is enough to identify the controller's register window, but the driver still needs the MTK clock, pinmux, interrupt-controller, and power setup.

The same device tree describes an alternative Novatek controller on SPI4 chip-select 1. The running `ilit_ts` binding and the Samsung A12 source point to the ILITEK path, but the exact live panel ID still needs confirmation.

The vendor driver decodes ten 4-byte contact records. Each contact's bytes 1-3 contain packed 12-bit X/Y values; three `0xff` bytes mark an empty slot. It scales the coordinates from a 2048x2048 range to panel dimensions. A standalone decoder for that report format is in `drivers/board/ili9881x_report.h`. It is not connected to an SPI transport or XNU touch events, and it has not been hardware-tested.

References:

- [Samsung A12 MT6765 input device tree](https://github.com/ASKSAP/android_kernel_samsung_a12/blob/A125FXXU1AUC3/arch/arm64/boot/dts/mediatek/a12/mt6765_input_a12_common.dtsi)
- [Samsung MT6765 SoC device tree](https://github.com/ASKSAP/android_kernel_samsung_a12/blob/A125FXXU1AUC3/arch/arm64/boot/dts/mediatek/mt6765.dts)
- [Samsung A12 display device tree](https://github.com/ASKSAP/android_kernel_samsung_a12/blob/A125FXXU1AUC3/arch/arm64/boot/dts/mediatek/a12/display_lcd_a12_common.dtsi)
- [Samsung A12 ILITEK driver](https://github.com/ASKSAP/android_kernel_samsung_a12/tree/A125FXXU1AUC3/drivers/input/touchscreen/ili9881x)

The display device tree uses a Samsung MTK DSI panel and selects ILI9882N for LCD ID `0x0A7250`, labeled A12. It specifies 720x1600 resolution, three DSI lanes, packed RGB888 pixels, burst video mode, and an 830 data rate. The current local device-tree match is source-derived; the running phone's live display ID has not been read.

The vendor touch driver is Linux-specific and cannot be loaded directly by XNU. Porting needs MTK SPI4/GPIO/interrupt support, power and reset sequencing, the correct controller firmware, and an XNU multitouch event path.

## First display path

The phone reports a PowerVR Rogue GE8320 GPU. The related MT6768 port uses Mali-G52 drivers, which do not apply to this GPU. There is no GE8320 driver in this tree.

The first practical screen milestone is a CPU-rendered test pattern through a device-specific framebuffer handoff. That requires a loader to establish the framebuffer address, pixel format, and lifetime. A GE8320 driver is a separate project and needs the register interface, clocks, power domains, memory mappings, and firmware details.

## Current status

- Touch controller and wiring: identified from matching Samsung kernel source; live panel ID not confirmed.
- Touch report decoding: a standalone header exists; not integrated or hardware-tested.
- MTK SPI4/GPIO/interrupt path: not implemented in XNU.
- DSI display configuration: source details identified; no loader framebuffer handoff exists.
- PowerVR acceleration: not implemented.
- No firmware or partitions have been modified.
