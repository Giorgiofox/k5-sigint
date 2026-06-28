# K5-SIG Release Notes

## M1.0

Date: 2026-06-28

Status: flashed and verified.

Changes:

- Keeps M0.9 persistent bank mask and UART bank event behavior.
- Fixes bank scan UX: after `F+0..F+6`, if at least one bank is enabled, the radio switches to the first valid memory channel in an enabled bank.
- Prevents accidental VFO/range scan after choosing a memory bank while previously sitting on an AIR VFO frequency.

Build:

- `text=60472`
- `data=44`
- `bss=3212`
- total `63728`

SHA-256:

- `firmware.bin`: `2d67c0c2611c84291deede9dccf4f6b7bfc31b54d8f05c87aad2ce2e01cbb12e`
- `firmware.packed.bin`: `201f2680c73695905916ef1e1bc69d33e7e157eeb1e1fa187b5abd2d8294ff8f`

Verification:

- K5TOOL reported `Firmware: "K5SIG M1.0"` after flashing.

## M0.9

Date: 2026-06-28

Status: flashed successfully, superseded by M1.0.

Changes:

- Keeps M0.8 persistent bank mask behavior.
- Emits UART bank-mask events on `F+0..F+6` changes:
  `K5SIG,EVT=BANK,MASK=0x00XX`.
- Avoids libc formatting helpers to preserve flash space.

Build:

- `text=60320`
- `data=44`
- `bss=3212`
- total `63576`

SHA-256:

- `firmware.bin`: `b40e2b11b8f8fcf80895f9f942e4c7a552ca4092f66def46e9ac8f0f420e4dcb`
- `firmware.packed.bin`: `7a5845f788b5840bb05ac275767586e2686d44c2859198c30e9571f8607f5fe0`

Known limitations:

- no UART hit event yet;
- no bank list/status page yet;
- bank mapping is still hardcoded.

## M0.8

Date: 2026-06-28

Status: built, superseded by M0.9 before flashing.

Changes:

- Keeps M0.7 bank-aware memory scanning.
- Persists enabled bank mask in EEPROM byte `0x0F1F`.
- Loads old EEPROMs safely by treating `0xFF` as all banks enabled.

Build:

- `text=60124`
- `data=44`
- `bss=3212`
- total `63380`

SHA-256:

- `firmware.bin`: `c0fd15a4921828cc56fb6a94dd29f83b822fe78d5c234a2dfaf3aa7d7f56b1db`
- `firmware.packed.bin`: `394cb136a7b16d7d2d6460c6cce09efd2d2b1ce9488f8a5fb707ad21e2d81a20`

Known limitations:

- no bank list/status page yet;
- bank mapping is still hardcoded;
- import quotas are still fixed in the importer.

## M0.7

Date: 2026-06-28

Status: built, superseded by M0.8 before flashing.

Changes:

- Adds SIGINT memory-bank scan filtering.
- Adds `F+0..F+6` bank toggles.
- Adds bank status message on the main screen.
- Adds Uniden BCD import tool in `tools/import_uniden_bcd.py`.
- Keeps M0.6 AIR AM 25 kHz scan behavior.

Bank shortcuts:

- `F+0`: all banks / no banks
- `F+1`: BANK 1
- `F+2`: BANK 2
- `F+3`: BANK 3
- `F+4`: BANK 4
- `F+5`: BANK 5
- `F+6`: BANK 6

Build:

- `text=59852`
- `data=44`
- `bss=3212`
- total `63108`

SHA-256:

- `firmware.bin`: `896e530fe10de3f2f73c3222a062e59cb79199a13151f737f9c75310b709dc44`
- `firmware.packed.bin`: `bed2aa8153668249cae7669109960b3ec041fcda6b5012ed1f790470763c44fd`

Known limitations:

- bank mask resets on power cycle;
- bank mapping is hardcoded;
- no bank selection screen yet.

## M0.6

Date: 2026-06-27

Status: flashed successfully.

Changes:

- AIR AM preset changed to 25 kHz step.
- SIGINT range scan dwell changed to about 70 ms.

Build:

- `text=59516`
- `data=44`
- `bss=3192`
- total `62752`

SHA-256:

- `firmware.bin`: `c06661bf554fbc4129cab654b87fb4d1040172da7cae8b68e072be144119bd30`
- `firmware.packed.bin`: `4b904e29693add1251bf48e2435228d514b0dd1736eedb70ac2a45ca464c367c`

## M0.5

Date: 2026-06-27

Status: flashed successfully.

Changes:

- First SIGINT scan preset implementation.
- AIR AM preset initially used 8.33 kHz step.
- User confirmed scan worked, but full AIR scan was slow.

SHA-256:

- `firmware.bin`: `64019c6ef0286eaca103c2179f99405b3a00b720acfc61e825f6ff8771ee064f`
- `firmware.packed.bin`: `b56d858d63620867503cba15065617d7653cc77bd715cd9d0e1b40e1fde6519a`

## M0.4

Date: 2026-06-27

Status: built, superseded before flash.

Changes:

- Single-frequency main screen.
- Simplified menu.
- SIGINT boot screen retained.

## M0.2

Date: 2026-06-27

Status: flashed successfully.

Changes:

- Boot screen: `SIGINT FW`, `K5-SIG`, version line.
- Early K5-SIG skeleton modules.

SHA-256:

- `firmware.packed.bin`: `8780e31731c5083f908d0d0f524e26e0e37a975bdf8d6e23a48621aac8c0d805`
