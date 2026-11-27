<!-- SPDX-FileCopyrightText: 2023 HID Engage Laboratory -->
<!-- SPDX-License-Identifier: MIT -->

# VariKey

The VaryKey platform is a customizable keyboard/keypad solution featuring a key matrix, microcontroller,
and the capability to control a small display and smart LED backlighting using the HID protocol.

## Overview

The VariKey platform is a highly customizable keyboard solution that combines a key matrix and a microcontroller.
The firmware of this platform enables not only the control of a small display but also the management of smart LEDs
for backlighting.

Control is facilitated through the HID protocol, allowing for seamless integration.

## Host Tools

This repository includes host-side tools to test and control the Varikey device.

### Backlight Control Demo

A small C++ program to control the backlight settings via USB HID.

**Prerequisites:**

- `hidapi` library installed (e.g., `sudo apt install libhidapi-dev` on Debian/Ubuntu)
- A C++17 compatible compiler (e.g., `g++`)

**Build and Run:**

From the repository root, build and run the demo:

```bash
.scripts/build_host_demo.sh
./host/host_backlight_demo
```

The program will connect to the device and cycle through different backlight programs (SET, MORPH, ALARM, OFF).

**Customization:**

- **VID/PID:** The Vendor and Product IDs are hardcoded in `host_backlight_demo.cpp`. You can change them there if needed.
- **Functionality:** The source code can be easily modified to send different color values or to create a more interactive command-line interface.

