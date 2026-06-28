# K5-SIG Operations

Date: 2026-06-28

## Build Firmware

From the firmware source directory:

```sh
make VERSION_STRING=M1.0
```

Outputs:

```sh
firmware
firmware.bin
firmware.packed.bin
```

Hash:

```sh
shasum -a 256 firmware.bin firmware.packed.bin
```

## Radio Modes

Normal mode:

- turn radio on normally;
- used for `-hello`, EEPROM read, EEPROM write.

Bootloader mode:

- turn radio off;
- hold PTT;
- power on while still holding PTT;
- LED should blink;
- used for firmware flash only.

## Check Radio Connection

Normal firmware response:

```sh
k5tool -port <serial-port> -hello
```

Expected form:

```text
Firmware: "K5SIG M0.6"
Using ProtocolV2...
Done
```

Bootloader response:

```sh
k5tool -port <serial-port> -hello
```

Expected form:

```text
Unexpected response Packet2FlashBeaconAck {
  Version="2.00.06"
}
```

K5TOOL reports this as an error for `-hello`, but it confirms the bootloader is present.

## Backup EEPROM

Use normal mode.

```sh
k5tool -port <serial-port> -rdee 0x0000 0x2000 eeprom-backup.raw
```

Always do this before changing memories or settings.

## Write EEPROM

Use normal mode.

```sh
k5tool -port <serial-port> -wree 0x0000 generated-eeprom.raw
```

This writes the radio configuration and memories. It does not flash firmware.

## Flash Firmware

Use bootloader mode.

```sh
k5tool -port <serial-port> -wrflash firmware.packed.bin
```

Successful output ends with:

```text
Done
```

## Restore Previous EEPROM

Use normal mode.

Example:

```sh
k5tool -port <serial-port> -wree 0x0000 eeprom-backup.raw
```

## Current Commands On Radio

Scan preset workflow:

- in VFO/frequency mode, press `SCAN`;
- firmware applies the SIGINT preset matching the current frequency;
- if the current frequency does not match a known preset, AIR AM is the fallback.

Memory bank workflow:

- press `F+0` for all banks / no banks;
- press `F+1` for BANK 1;
- press `F+2` for BANK 2;
- press `F+3` for BANK 3;
- press `F+4` for BANK 4;
- press `F+5` for BANK 5;
- press `F+6` for BANK 6;
- start memory scan normally.

## Safety Notes

- Do not write EEPROM while the radio is in bootloader mode.
- Do not flash firmware while the radio is in normal mode.
- Keep every backup made before a memory import.
- The firmware is experimental; keep a known-good packed firmware and EEPROM backup available.
