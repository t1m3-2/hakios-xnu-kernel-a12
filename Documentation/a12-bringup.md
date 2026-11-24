# Galaxy A12 (SM-A125F) bring-up notes

This is an initial hardware inventory and a porting route, not a bootable build. No device image has been flashed.

## Confirmed on the connected phone

| Item | Observation |
| --- | --- |
| Model / device | Samsung SM-A125F / `a12` |
| SoC | MediaTek MT6765 (Helio P35 family) |
| Android | Android 12, build `A125FXXS6CXJ5` |
| Bootloader | `ro.boot.flash.locked=0` in the running system |
| Boot layout | no slot suffix; `/dev/block/by-name/boot` exists |
| Display | 720 x 1600, 60 Hz |
| Graphics | PowerVR Rogue GE8320 |
| Touch input | Samsung `sec_touchscreen`; source indicates ILITEK on SPI4 |
| Android root | unavailable in the current shell (`su` is absent) |

The phone's live flattened device tree is not readable as the Android shell user. Samsung's published A125F kernel source gives a useful board reference; live DRAM reservations and bootloader handoff values still need to be recovered from the stock firmware or a privileged session.

## A viable boot strategy

A close HakiOS reference exists: its Galaxy A22 4G MediaTek port starts a small bare-metal loader in front of Android's kernel from the non-A/B `boot` partition, loads XNU once, and returns to Android. That port targets MT6769 and iOS 16.5, so it is a boot-chain and process reference—not an A12 image to flash. HakiOS's MediaTek guide explicitly says every phone's memory map, ramoops, framebuffer, interrupt/timer setup, and peripherals must be discovered for that phone.

Samsung devices do not provide the normal `fastboot boot` trial path. For this A12, the observed partition layout points to a custom chained Android boot image in the single `boot` partition. That is a partition write even if the loader defaults back to Android; before any such attempt the exact SM-A125F stock boot image and a verified Download Mode recovery path must be available. Current evidence does not establish that a built image would be accepted or boot.

References:

- [HakiOS A22 4G MediaTek port](https://github.com/david7xw/hakios-a22)
- [HakiOS MediaTek porting guide](https://github.com/oosexgang/hakios-xnu-kernel/blob/mtk/docs/mtk-porting.md)
- [HakiOS MediaTek common branch](https://github.com/oosexgang/hakios-xnu-kernel/tree/mtk)
- [Samsung A12 touch and display source notes](a12-touch-display.md)

## What must be adapted for the A12

- The A12's MT6765 memory map, boot-image details, reserved memory, framebuffer and display handoff, GIC/timer state, and eMMC controller settings. Do not reuse A22 values blindly.
- The A12 touch hardware: Samsung's source describes an ILITEK-family controller on MTK SPI4, unlike the A22's I2C touch controllers.
- The A12 display panel is 720x1600 DSI. CPU rendering into a proven bootloader framebuffer is the first target; the A12's PowerVR GE8320 has no driver in this tree, and the A22 Mali driver cannot be reused.
- iOS 16.5 is the practical baseline because the existing MediaTek port runs the iPhone X 20F66 user space. iOS 18 has no compatible port here.

Sileo is included by the existing MTK port in its Procursus `/var/jb` setup. That does not make Sileo installable on this phone yet: the A12 kernel, loader, display and touch still need to work first.

## Next stages

1. Obtain the exact stock A12 boot image and a known-good recovery path; extract the boot header and DTB without writing to the phone.
2. Build and validate the loader and kernel on the host, with an A12-specific board package and device tree.
3. Establish an early framebuffer test and a reliable log path, then validate one CPU, memory, GIC/timer, and eMMC in sequence.
4. Add touch transport and XNU touch events; test the ILI9882x-family report path on this panel.
5. Build the iOS 16.5 user-space image from the required iPhone X IPSW, then consider a reversible boot-partition trial.

No flashing, partition changes, or firmware extraction has been performed.
