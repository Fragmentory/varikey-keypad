#!/bin/bash
# SPDX-FileCopyrightText: 2025 Roman Koch <koch.roman@gmail.com>
# SPDX-License-Identifier: MIT
# SPDX-FileComment: Build-Skript für Host-Backlight-Demo
# SPDX-FileType: SCRIPT

set -e

cd "$(dirname "$0")/../host"

# Prüfen, ob hidapi installiert ist
if ! pkg-config --exists hidapi-hidraw; then
    echo "hidapi-hidraw nicht gefunden. Bitte installieren (z.B. sudo apt install libhidapi-dev)."
    exit 1
fi

# Kompilieren
g++ -std=c++17 -o host_backlight_demo host_backlight_demo.cpp $(pkg-config --cflags --libs hidapi-hidraw)

echo "Demo gebaut unter $PWD/host_backlight_demo"
