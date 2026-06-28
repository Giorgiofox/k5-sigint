# K5-SIG Roadmap

Date: 2026-06-28

## Guiding Rule

The radio should behave like a practical scanner first. CW, APRS and dashboard features should support that workflow rather than competing with it.

## Phase 0: Stabilize The Scanner Core

Status: in progress.

Implemented:

- single-frequency main screen;
- simplified menu;
- AIR AM range preset;
- faster range scan for SIGINT presets;
- Uniden BCD import workflow;
- memory bank filter;
- `F+0..F+6` bank toggles.

Next:

- flash and test M0.7;
- verify scan does not include disabled banks;
- verify imported channel names/frequencies/modes;
- persist bank enable mask;
- add a minimal bank status display;
- tune memory scan dwell if needed.

Acceptance:

- user can import scanner memories;
- user can enable only selected banks;
- user can start scan without navigating a complex menu;
- radio remains stable during long memory scans.

## Phase 1: Hit Logging

Goal: every interesting signal hit can be captured.

Tasks:

- add in-RAM ring buffer for hits;
- record frequency, memory channel, bank, mode, RSSI and time-since-boot;
- record squelch-open and squelch-close events;
- add bookmark action;
- display last hit or hit count on the main screen;
- expose hit events over UART in ASCII.

Acceptance:

- scanning produces hit records;
- user can bookmark the current signal;
- computer can capture events without screen scraping.

## Phase 2: UART Protocol

Goal: make the radio a useful data source for a future dashboard.

Initial line format:

```text
K5SIG,VER=1,EVT=HIT,FREQ=131.500000,CH=1,BANK=BANK1,MODE=AM,RSSI=-72,PEAK=-68,T=123456
K5SIG,VER=1,EVT=BOOKMARK,FREQ=145.600000,CH=173,BANK=BANK2,MODE=FM,RSSI=-83,T=124000
K5SIG,VER=1,EVT=BANK,MASK=0x003F,T=125000
```

Tasks:

- version the protocol;
- add `HIT`, `BOOKMARK`, `BANK`, `SCAN_START`, `SCAN_STOP`;
- rate-limit events;
- document serial speed and parsing rules;
- build a small local listener script.

Acceptance:

- events can be logged to CSV/JSON from a terminal or script.

## Phase 3: CW

Goal: useful CW support with safe defaults.

Tasks:

- inspect KD8CEC CW implementation in detail;
- isolate keyer/sidetone logic into `app/cw.*`;
- add sidetone practice mode first;
- add straight-key support;
- add WPM and pitch settings;
- add stored message support;
- only then evaluate RF CW/CWN/CWF behavior.

Acceptance:

- user can practice CW without RF TX;
- RF CW requires explicit user action and obeys TX limits.

## Phase 4: APRS TX

Goal: manual APRS reporting, not automatic unattended beaconing.

Tasks:

- implement AX.25 UI frame encoder;
- implement Bell 202 AFSK TX path or port verified logic;
- add manual status beacon;
- add manual fixed-position beacon;
- add signal report object from hit log;
- add callsign/SSID/path/profile storage.

Acceptance:

- no automatic APRS beacon is enabled by default;
- user can send a manual APRS status/report on legal frequencies.

## Phase 5: Companion App

Goal: richer UI without overloading the radio.

Tasks:

- local SQLite database for memories/logs;
- import Uniden BCD fully;
- profile editor for selecting 200-channel radio loads;
- serial live monitor for K5SIG UART events;
- hit log table;
- sweep/bandscope from scan events;
- backup/restore workflow.

Acceptance:

- the app can manage memories and show scan/log data;
- the radio remains usable standalone.

## Technical Debt

- Flash space is tight. Avoid pulling in libc formatting helpers such as `snprintf`.
- Bank names and ranges are hardcoded and should move to generated metadata or EEPROM later.
- The menu is intentionally reduced; do not add features back as ordinary menu clutter unless there is no better command path.
- Keep public source and private radio data separated.
- Add release notes per firmware version.

## Release Checklist

Before any public release:

- verify build from clean checkout;
- verify license headers and attribution;
- remove private artifacts;
- include `LICENSE`, `NOTICE`, and docs;
- document target hardware and risk;
- publish source, not personal EEPROM data;
- tag firmware version.
