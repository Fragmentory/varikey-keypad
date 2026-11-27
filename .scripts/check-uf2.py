#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Roman Koch
# SPDX-License-Identifier: MIT
"""Check every flash page in the UF2 produced for the RP2040 keypad."""
import argparse
import json
import os
from pathlib import Path
import struct

PROFILE = Path(__file__).resolve().parents[1] / "boards/rp2040-2m-flash.json"
BLOCK_BYTES = 512
PAGE_BYTES = 256
HEADER = struct.Struct("<8I")
END_MAGIC = struct.pack("<I", 0x0AB16F30)


def check_image(path):
    layout = json.loads(PROFILE.read_text())
    base = int(layout["flash_base"], 0)
    limit = base + int(layout["service_offset"], 0)
    sector = int(layout["erase_bytes"], 0)
    pages = set()
    numbers = set()

    with path.open("rb") as stream:
        size = os.fstat(stream.fileno()).st_size
        count, remainder = divmod(size, BLOCK_BYTES)
        if remainder or not 0 < count <= (limit - base) // PAGE_BYTES:
            raise ValueError("invalid UF2 file length for the application area")
        for index in range(count):
            data = stream.read(BLOCK_BYTES)
            if len(data) != BLOCK_BYTES:
                raise ValueError("truncated UF2 block")
            magic0, magic1, flags, address, length, number, total, family = HEADER.unpack_from(data)
            if (magic0, magic1) != (0x0A324655, 0x9E5D5157) or data[-4:] != END_MAGIC:
                raise ValueError(f"block {index}: invalid UF2 magic")
            if flags != 0x2000 or family != 0xE48BFF56 or length != PAGE_BYTES:
                raise ValueError(f"block {index}: expected an RP2040 main-flash page")
            if total != count or number >= count or number in numbers or address in pages:
                raise ValueError(f"block {index}: inconsistent or duplicate UF2 block")
            erase_start = address // sector * sector
            erase_end = (address + length + sector - 1) // sector * sector
            if address % PAGE_BYTES or erase_start < base or erase_end > limit:
                raise ValueError(f"block {index}: page 0x{address:08x} outside aligned application pages")
            numbers.add(number)
            pages.add(address)
    if base not in pages:
        raise ValueError("image has no flash boot page")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("image", type=Path)
    args = parser.parse_args()
    try:
        check_image(args.image)
    except (ValueError, OSError) as error:
        parser.exit(1, f"{args.image}: {error}\n")
