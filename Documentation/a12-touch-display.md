# Galaxy A12 touch and display bring-up

## Touchscreen findings

The live phone exposes the touch device as Android input `sec_touchscreen`, with a 10-slot multitouch range of X=0..719 and Y=0..1599. Its kernel binds an SPI driver named `ilit_ts`.

Samsung's SM-A125F MT6765 input device tree and kernel source identify the active ILITEK path as an ILI9881x firmware on the MTK SPI4 controller, chip-select 0, at up to 5 MHz. The device-tree source lists GPIO 10 as an active-low interrupt, GPIO 172 as reset, and GPIOs 13-16 for SPI4. It names firmware `tsp_ilitek/ili9881x_a12.hex`. This is source-derived wiring information; it has not yet been confirmed against the phone's unreadable live device tree.

The vendor tree also describes an alternative Novatek controller on SPI4 chip-select 1 for other panel variants. The running `ilit_ts` binding points to the ILITEK path, but the exact panel ID should be confirmed before selecting firmware or initialization sequences.

Reference sources:

- [Samsung A12 MT6765 input device tree](https://github.com/ASKSAP/android_kernel_samsung_a12/blob/A125FXXU1AUC3/arch/arm64/boot/dts/mediatek/a12/mt6765_input_a12_common.dtsi)
- [Samsung A12 ILITEK driver](https://github.com/ASKSAP/android_kernel_samsung_a12/tree/A125FXXU1AUC3/drivers/input/touchscreen/ili9881x)

The Android driver is Linux-specific and cannot be loaded directly by XNU. Porting needs an MTK SPI4/GPIO/interrupt path in the loader or kernel, power/reset sequencing, the right controller firmware, and an XNU multitouch event bridge. This repository does not yet have those MTK services.

## First display path

The phone reports a PowerVR Rogue GE8320 GPU. The related MT6768 port uses Mali-G52 drivers, which do not apply to this GPU. There is no GE8320 driver in this tree.

The first practical screen milestone is a CPU-rendered test pattern into a framebuffer whose address, pixel format, and lifetime are established by a device-specific loader. That can validate display scanout without claiming GPU acceleration. A GE8320 driver is a separate project and needs a documented register interface, clocks, power domains, memory mappings, and firmware details.

## Current status

- Touch controller family and wiring: identified from the matching Samsung kernel source; not yet exercised outside Android.
- Touch event parsing: not implemented in XNU.
- Framebuffer handoff: not implemented for this phone.
- PowerVR acceleration: not implemented.
- No firmware or partitions have been modified.
