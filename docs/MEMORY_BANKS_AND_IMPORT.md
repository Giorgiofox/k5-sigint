# Memory Banks and Uniden BCD Import

Date: 2026-06-28

## Source File

The importer accepts a Uniden `.bcd` SQLite database.

Detected schema tables:

- `system`
- `group`
- `channel`
- `site`
- `tfreq`
- `tgid`
- `trunk`
- `bandplan`
- `custombpl`
- `apcobpl`
- `locn`
- `bctool`

The current import ignores trunking concepts because the UV-K5 cannot operate as a trunking scanner.

## UV-K5 Capacity

The UV-K5 firmware has:

- 200 memory channels: `MR 0..199`, displayed as channels 1..200;
- 2 built-in scan list flags;
- no native concept of dozens of scanner banks.

The BCD may contain more channels than the radio can store. The importer selects up to 200 channels and can also export inspection CSVs.

## Import Script

Public script:

```sh
tools/import_uniden_bcd.py
```

Example:

```sh
python3 tools/import_uniden_bcd.py \
  --bcd input.bcd \
  --base-eeprom base-eeprom.raw \
  --out-dir output
```

The script:

1. reads the BCD SQLite database;
2. filters out locked-out channels;
3. filters to broad UV-K5 receive coverage;
4. selects up to 200 memory channels by quota;
5. writes channel records, names and channel attributes into a base EEPROM image;
6. generates CSVs for inspection.

## Current Bank Plan

The 200 imported channels are mapped to fixed channel ranges:

| Bank | Shortcut | Memory Channels | Count | Source |
| --- | --- | ---: | ---: | --- |
| BANK 1 | `F+1` | 1-38 | 38 | first source group |
| BANK 2 | `F+2` | 39-95 | 57 | second source group |
| BANK 3 | `F+3` | 96-152 | 57 | third source group |
| BANK 4 | `F+4` | 153-172 | 20 | fourth source group |
| BANK 5 | `F+5` | 173-188 | 16 | fifth source group |
| BANK 6 | `F+6` | 189-200 | 12 | sixth source group |

`F+0` toggles all banks on / all banks off.

The firmware uses this compiled mapping:

```c
{ "BANK 1", 0,   37  },
{ "BANK 2", 38,  94  },
{ "BANK 3", 95,  151 },
{ "BANK 4", 152, 171 },
{ "BANK 5", 172, 187 },
{ "BANK 6", 188, 199 },
```

The C code uses zero-based MR channel indexes. The radio display uses one-based channel numbers.

## Import Result

Default imported counts:

- BANK 1: 38
- BANK 2: 57
- BANK 3: 57
- BANK 4: 20
- BANK 5: 16
- BANK 6: 12

## Current Firmware Behavior

Memory scan calls `RADIO_FindNextChannel()`.

K5-SIG adds a bank filter layer:

- manual channel selection still works for any valid memory;
- scan only stops on channels whose bank is enabled;
- normal scan-list flags remain available;
- scan-list `ALL` now means all channels inside enabled K5-SIG banks.

## Known Limitations

- Bank mask persists in EEPROM from M0.8 onward.
- Bank mapping is hardcoded.
- No channel count or bank list screen exists yet.
- Import quotas are fixed in `tools/import_uniden_bcd.py`.
- Duplicates are not yet deduplicated across systems/groups.
- The radio cannot hold all channels from the BCD at once.

## Planned Improvements

### Firmware

- Add a compact bank status page.
- Add `BANKS` menu item if it can fit without making the menu complex.
- Show active bank count while scanning memories.
- Add temporary skip during memory scan.
- Add hit log entries tagged with bank name.

### Import Tool

- Add configurable import profiles.
- Add `--include-system` and `--include-group` filters.
- Add duplicate handling.
- Generate a Markdown import report.
- Generate CHIRP-compatible CSV for comparison.
- Optionally write only channel memory areas instead of a full EEPROM image.

### Companion App

- Store full BCD import in local SQLite.
- Let the user choose which banks/channels fit in the UV-K5.
- Push selected profile to EEPROM.
- Keep the full 1274-channel database available on the computer even though the radio stores 200.
