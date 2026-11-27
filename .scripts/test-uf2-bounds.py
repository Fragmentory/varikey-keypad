#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Roman Koch
# SPDX-License-Identifier: MIT
"""Exercise the actual provisioning entry point without opening a USB device."""
import os
from pathlib import Path
import struct
import json
import subprocess
import tempfile
import unittest

SCRIPTS = Path(__file__).resolve().parent
BASE = 0x10000000
SERVICE = 0x101FD000
PAGE = 256


def block(address, index, count, family=0xE48BFF56):
    header = struct.pack("<8I", 0x0A324655, 0x9E5D5157, 0x2000, address,
                         PAGE, index, count, family)
    return header + bytes(476) + struct.pack("<I", 0x0AB16F30)


class ProvisionBounds(unittest.TestCase):
    def setUp(self):
        scratch = tempfile.TemporaryDirectory()
        self.addCleanup(scratch.cleanup)
        self.folder = Path(scratch.name)
        self.image = self.folder / "factory image.uf2"
        self.calls = self.folder / "calls"
        tool = self.folder / "picotool"
        tool.write_text("""#!/usr/bin/env python3
import json, os, sys
with open(os.environ["CALLS"], "a") as log:
    log.write(json.dumps(sys.argv[1:]) + chr(10))
if sys.argv[1] == "info":
    print(" binary start:  " + os.environ.get("IMAGE_START", "0x10000000"))
    if os.environ.get("REPLACE_SOURCE"):
        with open(os.environ["REPLACE_SOURCE"], "wb") as image:
            image.write(b"replacement")
elif sys.argv[1] == "load":
    with open(sys.argv[4], "rb") as image, open(os.environ["LOADED"], "wb") as copy:
        copy.write(image.read())
    sys.exit(int(os.environ.get("LOAD_STATUS", "0")))
else:
    sys.exit(2)
""")
        tool.chmod(0o755)
        self.loaded = self.folder / "loaded"
        self.env = dict(os.environ, PICOTOOL=str(tool), CALLS=str(self.calls), LOADED=str(self.loaded))

    def provision(self, data, allowed, *selection):
        self.image.write_bytes(data)
        self.calls.unlink(missing_ok=True)
        result = subprocess.run([SCRIPTS / "provision.sh", self.image, *selection],
                                env=self.env, capture_output=True, text=True)
        self.assertEqual(result.returncode == 0, allowed, result.stderr)
        commands = self.calls.read_text().splitlines() if self.calls.exists() else []
        self.assertEqual(any(json.loads(cmd)[0] == "load" for cmd in commands), allowed)
        if allowed:
            args = json.loads(commands[-1])
            self.assertEqual(args[:3], ["load", "-v", "-x"])
            self.assertEqual(self.loaded.read_bytes(), data)
            if selection:
                self.assertEqual(args[4:], ["--bus", selection[0], "--address", selection[1]])
            else:
                self.assertEqual(args[4:], [])

    def test_protected_sectors(self):
        for address in (SERVICE, SERVICE + 4096, SERVICE + 8192):
            with self.subTest(address=hex(address)):
                self.provision(block(BASE, 0, 2) + block(address, 1, 2), False)

    def test_last_application_page(self):
        self.provision(block(BASE, 0, 2) + block(SERVICE - PAGE, 1, 2), True)

    def test_malformed_or_out_of_range(self):
        first = block(BASE, 0, 2)
        for data in (first, first[:-1], first + block(BASE, 1, 2),
                     first + block(BASE + PAGE, 0, 2),
                     first + block(BASE + PAGE, 1, 2, family=0),
                     first + block(SERVICE - 1, 1, 2),
                     first + block(BASE - PAGE, 1, 2),
                     first + block(0xffffff00, 1, 2),
                     block(BASE + PAGE, 0, 1)):
            with self.subTest(tail=data[-512:-480].hex()):
                self.provision(data, False)

    def test_selected_device_and_input_snapshot(self):
        self.env["REPLACE_SOURCE"] = str(self.image)
        self.provision(block(BASE, 0, 1), True, "1", "12")
        self.assertEqual(self.image.read_bytes(), b"replacement")

    def test_regressions(self):
        self.provision(block(BASE, 0, 1), False, "1")
        self.env["IMAGE_START"] = "0x10010000"
        self.provision(block(BASE, 0, 1), False)
        self.image.unlink()
        result = subprocess.run([SCRIPTS / "provision.sh", self.image], env=self.env,
                                capture_output=True)
        self.assertNotEqual(result.returncode, 0)

    def test_usb_error_is_returned(self):
        self.image.write_bytes(block(BASE, 0, 1))
        self.env["LOAD_STATUS"] = "7"
        result = subprocess.run([SCRIPTS / "provision.sh", self.image], env=self.env,
                                capture_output=True)
        self.assertEqual(result.returncode, 7)


if __name__ == "__main__":
    unittest.main()
