# K5-SIGINT Firmware

K5-SIGINT is an experimental SIGINT-style firmware for the Quansheng UV-K5 V1 / DP32G030.

The idea is simple: use a cheap handheld radio as a compact field receiver for finding, scanning, sorting and reporting RF activity. This is not an SDR replacement and it is not a classic ham-radio feature fork. The firmware is being shaped around scanner/SIGINT workflows: one active frequency, fast bank selection, memory imports, signal hits, UART logs, and later APRS/CW reporting.

This project is based on the open UV-K5 firmware lineage from DualTachyon, Egzumer and related contributors. Attribution and license notes are maintained in [docs/ATTRIBUTION_AND_LICENSES.md](docs/ATTRIBUTION_AND_LICENSES.md).

## Why "SIGINT"

Here "SIGINT" means signal intelligence in the practical receiver-operator sense:

- scan relevant bands quickly;
- keep channels grouped by mission-style banks;
- identify where activity is happening;
- record hits instead of only listening live;
- export useful events to a computer or dashboard;
- report selected findings manually via APRS later;
- keep CW as a serious operating/practice mode, not a novelty.

The UV-K5 has real limits: narrowband receiver, no IQ stream, no SDR-class bandwidth, and no trunking engine. K5-SIGINT works within those limits and focuses on workflows the radio can actually do well.

## Differences From Normal UV-K5 Firmware

Most UV-K5 custom firmware focuses on broader radio features: more menus, more modulation options, spectrum views, unlocks, convenience settings, and general ham/utility use.

K5-SIGINT changes the priorities:

| Area | Normal firmware | K5-SIGINT direction |
| --- | --- | --- |
| Main screen | Dual VFO/channel display | Single active frequency/channel for scanner-style use |
| Menu | Broad configuration menu | Reduced operational menu, advanced items hidden |
| Scanning | Generic VFO/memory scan | Band presets and memory-bank scan |
| Memories | Radio channels | Imported scanner banks from external databases |
| Logging | Mostly live listening | Planned hit log with frequency, bank, RSSI and time |
| UART | Configuration/programming | Planned event stream for dashboard/logging |
| APRS | Not primary | Planned manual signal reports/status only |
| CW | Usually absent or separate forks | Planned first-class CW/practice path |

## Implemented Now

Current tested firmware: `K5SIG M1.0`.

Implemented:

- SIGINT boot screen with version.
- Single-frequency main screen.
- Simplified operational menu.
- AIR AM preset scanning with 25 kHz step.
- Faster SIGINT range scanning.
- Uniden BCD import tool.
- 200-channel memory import workflow.
- Memory-bank scanning.
- Persistent bank mask in EEPROM.
- Bank toggle UART event.
- Bank selection automatically moves the radio into memory mode so `SCAN` scans the selected banks instead of accidentally starting VFO/range scan.

Bank shortcuts:

- `F+0`: all banks / no banks
- `F+1`: BANK 1
- `F+2`: BANK 2
- `F+3`: BANK 3
- `F+4`: BANK 4
- `F+5`: BANK 5
- `F+6`: BANK 6

## Planned Next

Near-term:

- Hit logging during scan.
- UART `HIT`, `SCAN_START`, `SCAN_STOP`, `BOOKMARK` events.
- Better on-radio bank status page.
- Better import profiles for Uniden BCD data.

Later:

- Companion dashboard/app.
- CW sidetone/practice and straight key support.
- APRS manual beacon and signal report objects.

## Hardware Target

Currently targeted:

- Quansheng UV-K5 V1 / DP32G030.
- Bootloader observed during development: `2.00.06`.

Not yet targeted:

- UV-K5 V3 / PY32F071.
- Other UV-K5-family hardware revisions.

## Build

```sh
make VERSION_STRING=M1.0
```

Outputs:

- `firmware`
- `firmware.bin`
- `firmware.packed.bin`

The local development workspace path contains spaces, so the Makefile has been patched to quote include paths.

## Flash

Use bootloader mode: power on while holding PTT, LED blinking.

```sh
k5tool -port /dev/cu.usbserial-1140 -wrflash firmware.packed.bin
```

See [docs/OPERATIONS.md](docs/OPERATIONS.md) for exact local commands, backup, EEPROM write and restore procedures.

## Memory Import

K5-SIGINT includes a public importer script:

```sh
tools/import_uniden_bcd.py
```

It reads a Uniden `.bcd` SQLite database, selects up to 200 UV-K5 memory channels, and produces an EEPROM image.

See [docs/MEMORY_BANKS_AND_IMPORT.md](docs/MEMORY_BANKS_AND_IMPORT.md).

## Documentation

- [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md): current implementation state.
- [docs/OPERATIONS.md](docs/OPERATIONS.md): build, flash, EEPROM and restore.
- [docs/MEMORY_BANKS_AND_IMPORT.md](docs/MEMORY_BANKS_AND_IMPORT.md): Uniden import and bank mapping.
- [docs/ROADMAP.md](docs/ROADMAP.md): next implementation phases.
- [docs/RELEASE_NOTES.md](docs/RELEASE_NOTES.md): firmware version history.
- [docs/K5_SIG_SPEC.md](docs/K5_SIG_SPEC.md): product/feature specification.
- [docs/STATE_OF_ART.md](docs/STATE_OF_ART.md): firmware landscape notes.
- [docs/ATTRIBUTION_AND_LICENSES.md](docs/ATTRIBUTION_AND_LICENSES.md): upstream attribution and license policy.

## Safety

This firmware is experimental. Flashing custom firmware can make a radio unusable and may require recovery tools. Keep a known-good firmware image and EEPROM backup before testing new builds.

Transmit, APRS and CW features must be used only where legal for the operator, band and country.
