# K5-SIG Project Status

Date: 2026-06-28

## Current Goal

K5-SIG is an experimental SIGINT-style firmware for the Quansheng UV-K5 V1 / DP32G030.

The immediate goal is not to build a generic ham-radio firmware. The goal is to make the radio usable as a compact field scanner:

- single-frequency main screen;
- fast band-oriented scanning;
- memory-bank scanning from imported scanner data;
- later CW and APRS/reporting features;
- later UART event export for a companion dashboard.

## Hardware

Confirmed target radio:

- Model family: Quansheng UV-K5.
- MCU family: V1 / DP32G030.
- Bootloader observed: `2.00.06`.
- Serial port used locally: `/dev/cu.usbserial-1140`.
- Original firmware observed before experiments: `V2.9R5`.

## Source Base

The active source tree is `k5-sig/`, derived from the Egzumer firmware tree.

Reference trees kept locally:

- `../egzumer/`
- `../dualtachyon-base/`
- `../f4hwn-v1/`
- `../f4hwn-v3/`
- `../cec-v01p/`
- `../cec-cw/`
- `../losehu/`
- `../official-firmware-collection/`
- `../reverse-engineering/`
- `../K5TOOL/`

See [STATE_OF_ART.md](STATE_OF_ART.md) and [ATTRIBUTION_AND_LICENSES.md](ATTRIBUTION_AND_LICENSES.md).

## Implemented Firmware Changes

Compile flags added:

- `ENABLE_SIGINT`
- `ENABLE_CW`
- `ENABLE_APRS`
- `ENABLE_SIG_UART_EVENTS`

New modules:

- `app/sigint.c`
- `app/sigint.h`
- `app/cw.c`
- `app/cw.h`
- `app/aprs.c`
- `app/aprs.h`

Current UI changes:

- Boot screen says `SIGINT FW`, `K5-SIG`, and version such as `K5SIG M0.7`.
- Main screen is single-channel/single-frequency.
- Dual-VFO visual layout is bypassed under `ENABLE_SIGINT`.
- Menu was reduced to operational entries by default; advanced items remain hidden behind the existing hidden-menu path.

Current scanning changes:

- Frequency preset scan exists for VFO/range scanning.
- AIR AM preset uses 118.000-136.975 MHz, AM, 25 kHz step.
- Range scanning dwell is reduced to 70 ms when a SIGINT range is active.
- Memory scan can be filtered by SIGINT bank mask.

Current memory-bank controls:

- `F+0`: toggle all banks / no banks.
- `F+1`: toggle BANK 1.
- `F+2`: toggle BANK 2.
- `F+3`: toggle BANK 3.
- `F+4`: toggle BANK 4.
- `F+5`: toggle BANK 5.
- `F+6`: toggle BANK 6.

The bank mask is persisted in EEPROM byte `0x0F1F` from M0.8 onward. EEPROMs without this byte set fall back to all banks enabled.

## Firmware Versions

### M0.1 / M0.2

Early boot-screen and skeleton builds. User confirmed M0.2 booted correctly.

### M0.4

Single-frequency main UI and simplified menu.

Built successfully but superseded before field testing.

### M0.5

First working SIGINT preset scan build.

User confirmed scan worked. Initial impression was that it appeared stuck, but it was scanning slowly because AIR AM used 8.33 kHz and full-band scan.

### M0.6

AIR AM scan speed improvement:

- AIR AM step changed to 25 kHz.
- SIGINT range dwell changed to about 70 ms per frequency.

Flashed successfully.

### M0.7

Memory bank scan implementation:

- bank mask layer added over memory scan;
- `F+0..F+6` controls added;
- Uniden import workflow added;
- firmware built successfully.

Build size:

- `text=59852`
- `data=44`
- `bss=3212`
- total `63108`

SHA-256:

- `firmware.bin`: `896e530fe10de3f2f73c3222a062e59cb79199a13151f737f9c75310b709dc44`
- `firmware.packed.bin`: `bed2aa8153668249cae7669109960b3ec041fcda6b5012ed1f790470763c44fd`

M0.7 was superseded by M0.8 before flashing.

### M0.8

Bank persistence implementation:

- keeps M0.7 memory bank scan behavior;
- persists bank mask to EEPROM byte `0x0F1F`;
- reloads bank mask during settings initialization;
- preserves compatibility with old EEPROM data by treating `0xFF` as all banks enabled.

Build size:

- `text=60124`
- `data=44`
- `bss=3212`
- total `63380`

SHA-256:

- `firmware.bin`: `c0fd15a4921828cc56fb6a94dd29f83b822fe78d5c234a2dfaf3aa7d7f56b1db`
- `firmware.packed.bin`: `394cb136a7b16d7d2d6460c6cce09efd2d2b1ce9488f8a5fb707ad21e2d81a20`

M0.8 was superseded by M0.9 before flashing.

### M0.9

Bank event implementation:

- keeps M0.8 persistent bank mask behavior;
- emits a UART event when the bank mask changes:
  `K5SIG,EVT=BANK,MASK=0x00XX`;
- avoids `printf`/`snprintf` to keep flash usage controlled.

Build size:

- `text=60320`
- `data=44`
- `bss=3212`
- total `63576`

SHA-256:

- `firmware.bin`: `b40e2b11b8f8fcf80895f9f942e4c7a552ca4092f66def46e9ac8f0f420e4dcb`
- `firmware.packed.bin`: `7a5845f788b5840bb05ac275767586e2686d44c2859198c30e9571f8607f5fe0`

M0.9 was flashed successfully, then superseded by M1.0.

### M1.0

Bank scan UX fix:

- keeps M0.9 behavior;
- when a bank is toggled with `F+0..F+6`, the radio automatically switches to the first valid memory channel in an enabled bank;
- this prevents `SCAN` from accidentally starting VFO/range scan when the previous screen was on an AIR frequency;
- verified by flashing and reading back `Firmware: "K5SIG M1.0"`.

Build size:

- `text=60472`
- `data=44`
- `bss=3212`
- total `63728`

SHA-256:

- `firmware.bin`: `2d67c0c2611c84291deede9dccf4f6b7bfc31b54d8f05c87aad2ce2e01cbb12e`
- `firmware.packed.bin`: `201f2680c73695905916ef1e1bc69d33e7e157eeb1e1fa187b5abd2d8294ff8f`

## Current Known Limitations

- The UV-K5 has 200 MR memory slots; scanner databases may contain more channels than the radio can hold.
- The first import selects 200 channels using fixed quotas. It does not yet provide an interactive bank/channel chooser.
- Bank names/ranges are currently compiled into firmware.
- There is no in-radio UI list of banks yet; only `F+0..F+6` shortcuts and a short status message.
- The radio is not a trunking scanner. Trunked systems/talkgroups cannot be followed in the UV-K5 firmware.

## Next Best Tasks

1. Verify imported memories are visible and scan works.
2. Verify `F+1..F+6` bank toggles affect memory scan.
3. Add SIGINT hit/event logging and UART events.
4. Add an on-radio `BANKS` status page or compact selector.
5. Improve Uniden import quotas and support user-selected bank profiles.
6. Resume CW implementation.
