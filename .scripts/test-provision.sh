#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roman Koch
# SPDX-License-Identifier: MIT
set -eu

provision=$(dirname "$(realpath "$0")")/provision.sh
scratch=$(mktemp -d)
trap 'rm -rf "$scratch"' EXIT
cd "$scratch"
export PICOTOOL="$scratch/picotool"
cat > picotool <<'MOCK'
#!/usr/bin/env bash
set -eu
printf '%s\n' "$@" >> calls
case "$1" in
    info) printf ' binary start:  %s\n' "${IMAGE_START:-0x10000000}" ;;
    load) exit "${LOAD_STATUS:-0}" ;;
    *) exit 2 ;;
esac
MOCK
chmod +x picotool
image="$scratch/factory image.uf2"
printf 'fixture' > "$image"
"$provision" "$image" 1 12
printf '%s\n' info -a "$image" load -v -x "$image" --bus 1 --address 12 > expected
diff -u expected calls
rm calls
if "$provision" "$scratch/missing.uf2" || "$provision" "$image" 1; then exit 1; fi
test ! -e calls
if IMAGE_START=0x10010000 "$provision" "$image"; then exit 1; fi
if grep -qx load calls; then exit 1; fi
rm calls
status=0
LOAD_STATUS=7 "$provision" "$image" || status=$?
test "$status" -eq 7
printf '%s\n' info -a "$image" load -v -x "$image" > expected
diff -u expected calls
