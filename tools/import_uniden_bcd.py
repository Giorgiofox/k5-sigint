#!/usr/bin/env python3
"""Build a UV-K5 EEPROM image from a Uniden BCD SQLite export.

This importer is intentionally local-data agnostic: it reads a user-provided
database and writes generated outputs to a caller-provided directory.
"""

from __future__ import annotations

import argparse
import csv
import shutil
import sqlite3
import struct
from dataclasses import dataclass
from pathlib import Path


MR_CHANNELS = 200

BANK_PLAN = [
    ("BANK 1", 38),
    ("BANK 2", 57),
    ("BANK 3", 57),
    ("BANK 4", 20),
    ("BANK 5", 16),
    ("BANK 6", 12),
]

STEP_MAP = {
    250: 0,     # 2.5 kHz
    500: 1,     # 5 kHz
    625: 2,     # 6.25 kHz
    1000: 3,    # 10 kHz
    1250: 4,    # 12.5 kHz
    2500: 5,    # 25 kHz
    833: 6,     # 8.33 kHz
}


@dataclass
class Channel:
    system: str
    group: str
    name: str
    frequency_10hz: int
    modulation: str
    step_10hz: int


def infer_modulation(modulation: str, frequency_10hz: int) -> int:
    mod = (modulation or "").upper()
    mhz = frequency_10hz / 100000.0
    if mod == "AM" or 108.0 <= mhz < 137.0 or 225.0 <= mhz <= 400.0:
        return 1
    return 0


def infer_step(modulation: str, frequency_10hz: int, step: int) -> int:
    if step in STEP_MAP:
        return STEP_MAP[step]

    mhz = frequency_10hz / 100000.0
    if (modulation or "").upper() == "AM" and (108.0 <= mhz < 137.0 or 225.0 <= mhz <= 400.0):
        return STEP_MAP[2500]
    if 156.0 <= mhz <= 162.1:
        return STEP_MAP[2500]
    if 446.0 <= mhz <= 446.2:
        return STEP_MAP[1250]
    return STEP_MAP[1250]


def valid_uvk5_rx(frequency_10hz: int) -> bool:
    return 1_800_000 <= frequency_10hz <= 63_000_000 or 84_000_000 <= frequency_10hz <= 130_000_000


def uvk5_band(frequency_10hz: int) -> int:
    if frequency_10hz < 10_800_000:
        return 0
    if frequency_10hz < 13_700_000:
        return 1
    if frequency_10hz < 17_400_000:
        return 2
    if frequency_10hz < 35_000_000:
        return 3
    if frequency_10hz < 40_000_000:
        return 4
    if frequency_10hz < 47_000_000:
        return 5
    return 6


def clean_name(value: str) -> str:
    text = " ".join((value or "").replace("\x00", " ").split())
    return text[:10]


def load_channels(db_path: Path) -> list[Channel]:
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    rows = conn.execute(
        """
        select
            s.sys_name,
            g.grp_name,
            c.chn_name,
            c.chn_freq,
            c.chn_modulation,
            c.chn_step,
            c.chn_lockout,
            c.chn_tlockout,
            s.sys_seqno,
            g.grp_seqno,
            c.chn_sortorder,
            c.chn_index
        from channel c
        left join [group] g on g.grp_index = c.grp_index
        left join system s on s.sys_index = c.sys_index
        where coalesce(c.chn_lockout, 0) = 0
          and coalesce(c.chn_tlockout, 0) = 0
          and c.chn_freq is not null
        order by s.sys_seqno, s.sys_index, g.grp_seqno, g.grp_index, c.chn_sortorder, c.chn_index
        """
    ).fetchall()
    conn.close()

    channels: list[Channel] = []
    for row in rows:
        frequency_10hz = int(row["chn_freq"]) * 10
        if not valid_uvk5_rx(frequency_10hz):
            continue
        channels.append(
            Channel(
                system=row["sys_name"] or "",
                group=row["grp_name"] or "",
                name=row["chn_name"] or "",
                frequency_10hz=frequency_10hz,
                modulation=row["chn_modulation"] or "AUTO",
                step_10hz=int(row["chn_step"] or 0),
            )
        )
    return channels


def select_for_radio(channels: list[Channel]) -> list[tuple[str, Channel]]:
    selected: list[tuple[str, Channel]] = []
    source_systems = list(dict.fromkeys(channel.system for channel in channels))

    for index, (bank_name, limit) in enumerate(BANK_PLAN):
        if index >= len(source_systems):
            break
        system_name = source_systems[index]
        bank_channels = [channel for channel in channels if channel.system == system_name]
        selected.extend((bank_name, channel) for channel in bank_channels[:limit])
    return selected[:MR_CHANNELS]


def write_channel(eeprom: bytearray, index: int, channel: Channel) -> None:
    offset = index * 16
    modulation = infer_modulation(channel.modulation, channel.frequency_10hz)
    step = infer_step(channel.modulation, channel.frequency_10hz, channel.step_10hz)
    bandwidth = 0 if modulation == 1 else 1

    eeprom[offset : offset + 8] = struct.pack("<II", channel.frequency_10hz, 0)
    eeprom[offset + 8 : offset + 16] = bytes(
        [
            0,
            0,
            0,
            (modulation << 4),
            (0 << 4) | (0 << 2) | (bandwidth << 1) | 0,
            0,
            step,
            0,
        ]
    )

    attr_offset = 0x0D60 + (index & ~7)
    attr = uvk5_band(channel.frequency_10hz) | (0 << 4) | (1 << 6) | (1 << 7)
    eeprom[attr_offset + (index & 7)] = attr

    name = clean_name(channel.name or channel.group or channel.system).encode("ascii", "ignore")[:10]
    name_data = bytearray(16)
    name_data[: len(name)] = name
    name_offset = 0x0F50 + (index * 16)
    eeprom[name_offset : name_offset + 8] = name_data[:8]
    eeprom[name_offset + 8 : name_offset + 16] = name_data[8:16]


def clear_channels(eeprom: bytearray) -> None:
    for index in range(MR_CHANNELS):
        eeprom[index * 16 : index * 16 + 16] = b"\xFF" * 16
        attr_offset = 0x0D60 + (index & ~7)
        eeprom[attr_offset + (index & 7)] = 0xFF
        name_offset = 0x0F50 + (index * 16)
        eeprom[name_offset : name_offset + 16] = b"\x00" * 16


def write_csv(path: Path, rows: list[tuple[str, Channel]]) -> None:
    with path.open("w", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(["bank", "channel", "system", "group", "name", "frequency_mhz", "modulation", "step_10hz"])
        for index, (bank, channel) in enumerate(rows, start=1):
            writer.writerow(
                [
                    bank,
                    index,
                    channel.system,
                    channel.group,
                    channel.name,
                    f"{channel.frequency_10hz / 100000:.5f}",
                    channel.modulation,
                    channel.step_10hz,
                ]
            )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--bcd", required=True, type=Path)
    parser.add_argument("--base-eeprom", required=True, type=Path)
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()

    args.out_dir.mkdir(parents=True, exist_ok=True)

    eeprom = bytearray(args.base_eeprom.read_bytes())
    if len(eeprom) != 0x2000:
        raise SystemExit(f"Expected 0x2000-byte EEPROM, got {len(eeprom)} bytes")

    channels = load_channels(args.bcd)
    selected = select_for_radio(channels)

    clear_channels(eeprom)
    for index, (_, channel) in enumerate(selected):
        write_channel(eeprom, index, channel)

    out_eeprom = args.out_dir / "uvk5-eeprom.raw"
    out_csv = args.out_dir / "uvk5-import.csv"
    out_all_csv = args.out_dir / "all-receivable.csv"

    out_eeprom.write_bytes(eeprom)
    write_csv(out_csv, selected)
    write_csv(out_all_csv, [("ALL", channel) for channel in channels])
    shutil.copy2(args.bcd, args.out_dir / args.bcd.name)

    print(f"Receivable channels: {len(channels)}")
    print(f"Imported channels:   {len(selected)}")
    print(f"EEPROM image:        {out_eeprom}")
    print(f"Radio CSV:           {out_csv}")
    print(f"All-channel CSV:     {out_all_csv}")


if __name__ == "__main__":
    main()
