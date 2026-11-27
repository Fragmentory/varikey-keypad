#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roman Koch
# SPDX-License-Identifier: MIT
set -eu

usage() {
    echo "Usage: $0 FACTORY.uf2 [USB_BUS USB_ADDRESS]"
    echo "Connect the intended RP2040 in USB BOOTSEL mode. This writes the factory image."
}
if [[ $# == 1 && $1 == --help ]]; then usage; exit 0; fi
if [[ $# != 1 && $# != 3 ]]; then usage >&2; exit 2; fi
image=$1
if [[ $image != *.uf2 || ! -f $image || ! -r $image || ! -s $image ]]; then
    echo "Expected a readable, nonempty factory UF2: $image" >&2
    exit 2
fi
if [[ $image != /* ]]; then image=$PWD/$image; fi
device=()
if [[ $# == 3 ]]; then
    if [[ ! $2 =~ ^[0-9]+$ || ! $3 =~ ^[0-9]+$ ]]; then usage >&2; exit 2; fi
    device=(--bus "$2" --address "$3")
fi
tool=${PICOTOOL:-picotool}
if ! command -v "$tool" >/dev/null; then
    echo "USB-enabled host picotool required. Install it on PATH or set PICOTOOL." >&2
    exit 1
fi

# Inspect and program the same private copy even if a build replaces the input.
scripts=$(dirname "$(realpath "$0")")
scratch=$(mktemp -d)
trap 'rm -rf "$scratch"' EXIT
cp -- "$image" "$scratch/keypad.uf2"
image=$scratch/keypad.uf2
"$scripts/check-uf2.py" "$image"

# Inspect the file offline so an application linked at an offset cannot be
# mistaken for the factory image used to boot a blank RP2040.
metadata=$("$tool" info -a "$image") || { echo "$metadata" >&2; exit 1; }
if ! grep -Eq '^[[:space:]]*binary start:[[:space:]]+0x10000000[[:space:]]*$' <<< "$metadata"; then
    echo "Expected a factory image linked at flash base 0x10000000." >&2
    exit 1
fi
"$tool" load -v -x "$image" "${device[@]}"
