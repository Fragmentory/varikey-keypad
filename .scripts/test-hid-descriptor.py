#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Roman Koch
# SPDX-License-Identifier: MIT
"""Check the USB output report that a host would see in the linked RP2040 image."""

from pathlib import Path
import subprocess
import sys

elf = Path(sys.argv[1])
symbols = subprocess.check_output(["arm-none-eabi-nm", "-C", "-S", str(elf)], text=True)
for line in symbols.splitlines():
    fields = line.split()
    if fields[-1:] == ["__flash_binary_start"]:
        flash_base = int(fields[0], 16)
    elif fields[-1:] == ["desc_hid_report"]:
        address, length = (int(field, 16) for field in fields[:2])

offset = address - flash_base
descriptor = elf.with_suffix(".bin").read_bytes()[offset:offset + length]
cursor = 0
report_id = report_size = report_count = output_bits = 0
while cursor < len(descriptor):
    prefix = descriptor[cursor]
    size = (0, 1, 2, 4)[prefix & 3]
    value = int.from_bytes(descriptor[cursor + 1:cursor + 1 + size], "little")
    item = prefix & 0xfc
    if item == 0x84:  # Report ID
        report_id = value
    elif item == 0x74:  # Report Size
        report_size = value
    elif item == 0x94:  # Report Count
        report_count = value
    elif item == 0x90 and report_id == 0xe0 and not value & 1:  # Data Output
        output_bits += report_size * report_count
    cursor += size + 1

if output_bits != 63 * 8:
    sys.exit(f"HID output report 0xe0 has {output_bits} data bits, expected 63 bytes")
