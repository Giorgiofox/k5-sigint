# Attribution and License Policy

Date: 2026-06-27

K5-SIG is intended to become a public repository. Attribution, copyright notices and license compatibility are project requirements, not cleanup work for later.

## Policy

- Keep upstream copyright headers intact.
- Document every imported codebase, script, algorithm and binary reference.
- Do not copy code from a project unless its license is known and compatible with the target repository.
- Treat unclear-license code as reference only.
- Keep generated firmware artifacts separate from source.
- Keep user radio backups private and do not commit them to a public repository.
- Public release should include `NOTICE`, `LICENSE`, and this attribution history.

## Intended Project License

Candidate: Apache License 2.0.

Reason:

- Egzumer, DualTachyon and KD8CEC source trees use Apache 2.0 lineage.
- Apache 2.0 is permissive, preserves attribution, and is compatible with a public firmware project.

This can change only if a required dependency forces a different license.

## Upstream References

### DualTachyon UV-K5 Firmware

- Repository: https://github.com/DualTachyon/uv-k5-firmware
- Local path: `dualtachyon-base/`
- Role: original open reimplementation baseline.
- License: Apache 2.0.
- Usage status: compatible source reference and possible code base ancestry through Egzumer.

Required attribution:

- Preserve copyright notices by Dual Tachyon.
- Mention as foundational reverse-engineered open firmware.

### Egzumer UV-K5 Firmware Custom

- Repository: https://github.com/egzumer/uv-k5-firmware-custom
- Local path: `egzumer/`
- Role: initial K5-SIG source base candidate.
- License: Apache 2.0 inherited from DualTachyon.
- Usage status: compatible source base.

Required attribution:

- Preserve upstream copyright headers.
- Mention merge lineage from DualTachyon, OneOfEleven and Fagci work as described by upstream README.

### F4HWN / Armel UV-K5 Firmware Custom

- Repository: https://github.com/armel/uv-k5-firmware-custom
- Local path: `f4hwn-v1/`
- Role: V1/DP32G030 feature reference and possible future source base.
- License: Apache 2.0 lineage.
- Usage status: compatible source reference/base candidate.

Required attribution:

- Preserve F4HWN/Armel notices and upstream lineage.

### F4HWN V3 / Armel + Muzkr UV-K1/K5V3 Firmware

- Repository: https://github.com/armel/uv-k1-k5v3-firmware-custom
- Local path: `f4hwn-v3/`
- Role: V3/PY32F071 hardware reference and serialtool reference.
- License: firmware Apache 2.0 lineage; serialtool files observed with MIT headers.
- Usage status: reference only for this V1 project unless V3 support is added.

Required attribution:

- Preserve Armel, Muzkr and contributor notices for any copied code.
- Keep MIT-licensed serialtool code attribution if reused.

### KD8CEC UVK5CEC

- Repository: https://github.com/phdlee/uvk5cec
- Local paths: `cec-v01p/`, `cec-cw/`
- Role: CW and APRS/SSTV/WSPR experimentation reference.
- License: Apache 2.0 headers observed in source files.
- Usage status: compatible source reference for CW; APRS code availability must be verified per tag/release before copying.

Required attribution:

- Preserve KD8CEC copyright notices.
- Document exactly which CW/APRS files or algorithms are ported.

### A mnemonic Quansheng UV-K5 Firmware Collection

- Repository: https://github.com/amnemonic/Quansheng_UV-K5_Firmware
- Local path: `official-firmware-collection/`
- Role: official firmware archive, tools, hardware docs, pack/unpack reference.
- License: mixed/varies by included material; official firmware binaries are not open-source code.
- Usage status: reference only unless a specific file license is checked.

Required attribution:

- Cite as firmware/hardware/tooling reference.
- Do not redistribute official firmware binaries in K5-SIG releases unless permission/license is clear.

### SQ5BPF UVK5 Reverse Engineering

- Repository: https://github.com/sq5bpf/uvk5-reverse-engineering
- Local path: `reverse-engineering/`
- Role: programming protocol and EEPROM reverse-engineering reference.
- License: README says CC-BY-SA-4.0 for repository; `deobfuscate.py` says GPLv3.
- Usage status: reference only unless we intentionally accept license obligations.

Required attribution:

- Cite protocol/EEPROM insights if used.
- Do not copy GPLv3 code into an Apache 2.0 firmware tree.

### LoseHu UV-K5 Firmware Custom

- Repository: https://github.com/losehu/uv-k5-firmware-custom
- Local path: `losehu/`
- Role: SI4732/SSB and advanced feature reference.
- License: must be checked file-by-file before copying.
- Usage status: reference only for now.

### QRP73 K5TOOL

- Repository: https://github.com/qrp73/K5TOOL
- Local path: `K5TOOL/`
- Role: flashing and EEPROM tooling.
- License: GPL-3.0 per repository metadata/README listing.
- Usage status: external tool only.

Required attribution:

- Cite as external flashing tool used during development.
- Do not copy GPL-3.0 code into the Apache 2.0 firmware source tree.

## Public Repository Checklist

- Add root `LICENSE`.
- Add root `NOTICE`.
- Add `docs/ATTRIBUTION_AND_LICENSES.md`.
- Add `.gitignore` entries for backups, dumps and local build artifacts.
- Include upstream source notices in copied files.
- Mention firmware is experimental and user assumes flashing risk.
- Document legal constraints for TX, APRS and CW operation.
