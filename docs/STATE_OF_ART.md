# Quansheng UV-K5 Firmware Project Notes

Date: 2026-06-27

## Local Workspace

Cloned references:

- `egzumer/`: Egzumer UV-K5/K6/5R firmware, open reimplementation based on DualTachyon plus OneOfEleven/Fagci work.
- `f4hwn-v3/`: F4HWN port for newer UV-K1 / UV-K5 V3 radios using PY32F071 MCU.
- `cec-v01p/`: KD8CEC tag `v_0.1p`, useful for CW/CWF/CWN code study.
- `cec-cw/`: KD8CEC current main branch.
- `dualtachyon-base/`: baseline open reimplementation of stock v2.1.27.
- `official-firmware-collection/`: stock firmware binaries, hardware docs, OpenOCD configs, packing/reverse-engineering notes.
- `reverse-engineering/`: sq5bpf EEPROM/protocol reverse-engineering notes and CHIRP driver.
- `losehu/`: feature-rich firmware with SI4732/SSB support, useful as a reference for receiver extensions.

## Current Firmware Landscape

- DualTachyon is the clean baseline: preservation-oriented open reimplementation, good for understanding stock behavior.
- Egzumer is the best V1/V2 base for a custom firmware: active ecosystem, known build system, broad feature set, spectrum analyzer, AM fix, wide RX, better scanning and UI.
- F4HWN/armel is the practical choice for UV-K5 V3 / PY32F071 hardware. It is derived from Egzumer/F4HWN but adapted to the newer MCU.
- KD8CEC is the best CW reference: its `v_0.1p` release adds CW, CWF, CWN modes and CW settings.
- Losehu is a useful reference for SI4732/SSB-heavy builds, but it is a larger and more divergent codebase.
- The official firmware collection is the binary baseline for comparison and disassembly.

## Recommended Base

Start from Egzumer for a classic UV-K5 V1/V2 with DP32G030.

If the radio is UV-K5 V3 / PY32F071, switch the base to `f4hwn-v3/` and port the same CW plan there.

Reasoning:

- Egzumer and F4HWN are already derived from the open reimplementation, so we can work in source instead of treating the whole firmware as opaque binary.
- CW can be ported from KD8CEC with much lower risk than writing timing, key input and BK4819 offset behavior from scratch.
- Official binaries should be used for comparison, not as the implementation base.

## CW Findings

KD8CEC `v_0.1p` has the useful CW primitives:

- `ceccommon.c`: `CW_TONE`, `CW_SPEED`, `CW_KEYTYPE`, `CW_KEY_ADC`.
- `ceccommon.h`: `GET_FREQ_OFFSET()` changes frequency offset for `MODULATION_CW` / `MODULATION_CWN`.
- `settings.c`: persists CW key type, speed and tone in EEPROM-backed settings.

Initial code quality issues to address before porting:

- Several includes use Windows backslashes, which break GCC builds on macOS/Linux.
- CW code is mixed into broad common files and should be isolated into `app/cw.*` or a similarly scoped module.
- ADC keying thresholds and key-type semantics need hardware verification.
- Timing/keyer behavior should be made explicit and tested on bench before flashing as a daily-use firmware.

## Build Status

Local path issue:

- Building inside `/Users/giorgiocampiotti/Desktop/Quansheng UV-K5` fails because current Makefiles do not quote include paths and the directory name contains a space.
- Building from `/tmp/uvk5-build/egzumer` succeeds with local `arm-none-eabi-gcc 13.3.1`.

Egzumer build result:

- `make` builds `firmware` and `firmware.bin`.
- Packed firmware is not produced because Python package `crcmod` is missing.
- Reported size: `text=60760`, `data=44`, `bss=3172`, total `63976`.

K5-SIG M0 skeleton build result:

- Source path: `k5-sig/`
- Build command: `make AUTHOR_STRING=K5SIG VERSION_STRING=M0SKEL`
- Build works in the workspace path containing a space.
- `firmware`, `firmware.bin` and `firmware.packed.bin` are generated.
- Reported size: `text=60768`, `data=44`, `bss=3172`, total `63984`.
- `firmware.bin` SHA-256: `24c0e3de8e87a5b69620cf8c04c086521d6904d9621c580cfaf426326f53ff10`
- `firmware.packed.bin` SHA-256: `fccd446154b83c0a3991e65c506e99ae131825e30f98983057d79edac2e90eff`

CEC v0.1p build result:

- Build currently fails on macOS at `#include "driver\bk4819.h"`.
- First cleanup task is converting path separators in source includes to portable `/`.

## Connected Radio

Connection checked on 2026-06-27:

- Serial port: `/dev/cu.usbserial-1140`
- Firmware string read over normal serial protocol: `V2.9R5`
- This matches the public IJV 2.9R5 firmware family, which is known for CW-related features.
- Configuration memory should be backed up before firmware experiments.

Current safety state:

- No write or flash operation has been sent to the radio.
- Only firmware-version read and configuration-memory reads have been performed.
- Calibration/configuration backup should exist before any firmware experiment.

## Reverse-Engineering Plan

1. Normalize workspace build path or patch Makefiles to quote include paths.
2. Install `crcmod` or use Docker build to produce flashable `firmware.packed.bin`.
3. Generate `.elf`, `.bin`, `.map`, `.lst` for Egzumer and DualTachyon.
4. Decode/compare official stock binaries from `official-firmware-collection/firmware`.
5. Load stock and open-built binaries in Ghidra as ARM Cortex-M0, base address `0x00000000`.
6. Use source symbols from open builds to map peripherals, BK4819 routines, scheduler, settings and UI flows.
7. Port CW from CEC into the chosen base behind `ENABLE_CW`.
8. Add menu/settings integration: CW mode, CW pitch, WPM, straight key / paddle/keyer mode, sidetone options.
9. Bench-test receive-only behavior first, then carrier/keying behavior into dummy load or service monitor.
10. Keep TX limits conservative and region-configurable.

## First Implementation Milestone

Build a source-level firmware branch with:

- Egzumer base.
- Portable build in a no-space path or patched Makefile.
- `ENABLE_CW=1` compile flag.
- CW settings and EEPROM persistence.
- Initial straight-key CW carrier offset behavior ported from CEC.
- No paddle/iambic keyer until straight-key timing is verified.
