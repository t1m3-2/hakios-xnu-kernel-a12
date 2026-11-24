# Galaxy A12 (SM-A125F) bring-up notes

This is an initial hardware inventory, not a bootable port. Nothing in this document has been flashed to the phone.

## Confirmed on the connected phone

| Item | Observation |
| --- | --- |
| Model / device | Samsung SM-A125F / `a12` |
| SoC | MediaTek MT6765 (`ro.board.platform=mt6765`), Helio P35 family |
| Android | Android 12, build `A125FXXS6CXJ5` |
| Display | 720 x 1600, 60 Hz |
| Graphics | Imagination Technologies PowerVR Rogue GE8320 |
| Touch input | Samsung `sec_touchscreen`, coordinates 720 x 1600; controller model/bus still unknown |
| ADB root | Not available (`su` is absent) |

The live flattened device tree exists at `/sys/firmware/fdt`, but Android's shell user cannot read it. The device-tree `model` and `compatible` properties are also denied. A rooted Android session or an extracted stock firmware/boot image is needed to obtain the board description without guessing.

## Related code and limits

- The main `hakios-xnu-kernel` tree's current porting guide describes Qualcomm devices; it has no MT6765 platform package.
- The separate `0xBEEBAD/hakios-mt6768` project is a MediaTek reference for the Infinix Hot 20 (MT6768), not this Samsung device. It targets iOS 16.5-era XNU and includes its own loader, board-specific drivers, and experiments. Its MT6768 code cannot be assumed to work on MT6765.
- This phone reports a PowerVR GE8320, while that reference phone uses a Mali-G52. Display/GPU code is therefore not reusable as-is.
- This is still research toward an experimental port. A usable iOS installation, Sileo, cellular service, camera, and normal Android safety guarantees are not established.

## Next bring-up requirements

1. Obtain the A12's live device tree and matching Samsung MT6765 kernel source/configuration. Do not infer register addresses from the MT6768 phone.
2. Identify the A12's boot-image layout and a reversible, RAM-only boot method before attempting any device execution.
3. Establish a minimal loader/kernel boot and serial/log path, then validate memory, interrupt/timer, storage, display scanout, and touch one at a time.
4. Only after those stages work should an iOS 16.5 user-space image and package-manager compatibility be investigated.

No flashing, partition changes, or firmware extraction has been performed.
