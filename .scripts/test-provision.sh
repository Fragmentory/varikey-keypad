#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roman Koch
# SPDX-License-Identifier: MIT
set -eu
exec "$(dirname "$(realpath "$0")")/test-uf2-bounds.py"
