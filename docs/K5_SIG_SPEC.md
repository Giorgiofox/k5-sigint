# K5-SIG Firmware Specification

Date: 2026-06-27

## Mission

K5-SIG is a field SIGINT-style firmware for the Quansheng UV-K5 V1 / DP32G030.

The radio is not a software-defined receiver and should not be treated as one. The goal is to make the UV-K5 useful as a compact field tool for finding, characterizing, bookmarking and reporting VHF/UHF activity, with CW and APRS used as operational tools rather than novelty features.

## Product Principles

- Fast field workflow beats feature count.
- Receive, scan and log are primary. Transmit features must have guardrails.
- Every signal hit should be easy to preserve: frequency, mode, RSSI, time and optional note.
- CW is a first-class operating mode.
- APRS is a reporting layer for the radio scout workflow, not just a generic tracker.
- UART should expose machine-readable events so a laptop or phone can become the richer UI.
- The companion app is part of the system design: firmware events should be usable without screen scraping.
- Keep the firmware buildable from source and keep each experimental subsystem behind a compile flag.

## Non-Goals

- Full SDR behavior.
- Heavy digital decoding inside the radio.
- Promising reliable APRS RX before the audio/baseband path is proven.
- Wide-open TX by default.
- One-off binary patching as the main implementation path.

## Target Hardware

Initial target:

- Quansheng UV-K5 V1 / DP32G030.
- Bootloader observed on connected radio: `2.00.06`.
- Existing user firmware: IJV `V2.9R5`.

Deferred targets:

- UV-K5 V2 / PY32F030.
- UV-K5 V3 / PY32F071.

## Build Strategy

Base:

- Start from Egzumer for the first open-source implementation branch.
- Use KD8CEC as reference for CW and APRS experiments.
- Use IJV behavior as user-facing inspiration only unless source becomes available.

Compile flags:

- `ENABLE_SIGINT`
- `ENABLE_CW`
- `ENABLE_APRS`
- `ENABLE_SIG_UART_EVENTS`

Initial policy:

- Flags may compile in stubs before full behavior exists.
- TX-related features default to conservative behavior.
- First flashable image must be a reversible test build, not a daily-driver claim.

## Main Modes

### Monitor

Purpose: inspect one frequency quickly.

Features:

- Large frequency display.
- RSSI numeric and peak hold.
- Noise/glitch view where BK4819 data is available.
- Mode indication: FM, NFM, AM, RAW/BYP, CW when implemented.
- One-key bookmark of current signal.
- UART event on squelch open/close and bookmark.

### Hunt

Purpose: scan ranges and collect signal hits.

Features:

- Scan ranges independent from memory channels.
- Temporary skip and persistent blacklist.
- Hit log with frequency, RSSI peak, mode and time-since-boot.
- Configurable dwell/resume behavior.
- Priority watch channels.

### Log

Purpose: review and export discoveries.

Features:

- Last N signal hits in RAM.
- Optional EEPROM-backed compact log if space allows.
- UART dump in line-oriented text or compact binary records.
- APRS object/report generation for selected hits.

### CW

Purpose: CW operation and practice.

Milestone order:

- Sidetone-only practice mode.
- Straight key support.
- CW/CWF/CWN mode and pitch offset.
- WPM setting.
- Stored messages.
- Beacon mode with explicit confirmation and power/frequency guardrails.
- Paddle/iambic A/B after straight-key timing is verified.

### APRS

Purpose: report operator status and signal discoveries.

Milestone order:

- Encode AX.25 UI frames.
- Generate Bell 202 AFSK 1200 TX audio.
- Manual APRS status beacon.
- Manual APRS position beacon with fixed/manual coordinates.
- APRS signal object/report from log entries.
- UART-provided position input.
- KISS/serial event bridge.
- Experimental APRS RX only after TX and UART reporting are stable.

## APRS Profile

Default profile should be explicit and editable:

- Callsign and SSID.
- Path.
- Symbol.
- Comment.
- Fixed/manual latitude and longitude.
- Beacon interval default: off.
- Manual send action: enabled.

Signal report object format should include:

- Frequency in MHz.
- Mode.
- RSSI peak.
- Short source tag, such as `K5SIG`.
- Optional bearing or note.

## UART Event Model

Initial text format:

```text
K5SIG,EVT=HIT,FREQ=433.500000,MODE=FM,RSSI=-72,PEAK=-68,T=123456
K5SIG,EVT=BOOKMARK,FREQ=145.800000,MODE=FM,RSSI=-83,T=124000
K5SIG,EVT=CW_TX,FREQ=144.050000,WPM=18,T=130000
K5SIG,EVT=APRS_TX,TYPE=OBJECT,T=131000
```

The first version can be plain ASCII. A binary format can come later if throughput becomes a problem.

## Companion App

Purpose: manage the radio as a field notebook and dashboard.

The app should not pretend the UV-K5 is an SDR. Spectrum views are sweep/bandscope views built from tuned frequency, RSSI, squelch and scan events. This is still useful for SIGINT-style work because it shows where activity was seen, how strong it was and how it changed over time.

Core app features:

- Memory/codeplug editor with import/export.
- Radio backup and restore workflow.
- Firmware flash helper with explicit safety checks.
- Live dashboard for current VFO, memory channel, RSSI, squelch state and battery.
- Bandscope/spectrum panel from sweep samples.
- Hit log with frequency, mode, RSSI peak, timestamp, notes and tags.
- Channel and scan-range management.
- APRS profile editor and APRS message/object preview.
- CW settings editor and stored-message manager.
- Export to CSV/JSON/KML for post-mission analysis.

Preferred architecture:

- Firmware exposes ASCII UART events first.
- App stores local data in SQLite.
- Desktop app target can be Tauri or Electron.
- A browser-based prototype can use Web Serial for fast iteration.
- The serial protocol must be versioned so the app can detect incompatible firmware builds.

Initial screens:

- Dashboard: current radio state, active channel and live RSSI.
- Spectrum: sweep/bandscope plot and recent peaks.
- Memories: channels, scan lists and profiles.
- Log: hit table, notes, tags and export.
- APRS/CW: operator profile, beacon preview and CW messages.

## Safety Rules

- Never erase or modify calibration without explicit restore operation.
- Keep a backup before flashing experimental builds.
- Do not enable automatic APRS RF beaconing by default.
- TX actions must check frequency policy and user settings.
- Practice CW must be sidetone-only unless RF TX is explicitly armed.

## Milestones

### M0: Project Skeleton

- `k5-sig/` source tree created.
- Build works in the current workspace path.
- `ENABLE_SIGINT`, `ENABLE_CW`, `ENABLE_APRS`, `ENABLE_SIG_UART_EVENTS` flags exist.
- Stub modules compile without changing runtime behavior.
- Baseline packed firmware can be generated.

### M1: Signal Logger

- Hit record structure.
- In-RAM ring buffer.
- UART text export for hit/bookmark events.
- Key action for bookmark.

### M2: CW Basic

- CW settings structure.
- Sidetone practice mode.
- Straight-key input path.
- CEC-derived pitch/offset behavior evaluated.

### M3: APRS TX Basic

- AX.25 address/control/PID/FCS encoder.
- APRS information field builder.
- AFSK TX prototype.
- Manual status beacon only.

### M4: APRS Signal Reports

- Convert log record into APRS object/report.
- Manual RF send.
- UART preview before RF send.

### M5: Scout UI

- Monitor screen with RSSI peak.
- Log review screen.
- Mission profile shortcuts.

### M6: Companion App Prototype

- Serial connection and firmware version detection.
- Live event ingestion.
- Memory/log database.
- Dashboard and bandscope panels.
- Export of hit log.

## Open Technical Questions

- Exact flash/RAM budget after stubs and logger.
- Best way to generate AFSK: BK4819 tone/FFSK support vs CPU-timed audio path.
- Whether APRS RX is feasible without unacceptable CPU/RAM and audio-path compromises.
- How much of CEC CW can be ported cleanly without inheriting large unrelated changes.
- Whether F4HWN v4.3 V1 should replace Egzumer as base after the first skeleton.
- Best app shell for the first public release: Tauri, Electron or browser/Web Serial prototype.
