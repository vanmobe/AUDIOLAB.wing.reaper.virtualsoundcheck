#!/usr/bin/env bash
set -euo pipefail

TARGET_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/REAPER/UserPlugins"
rm -f "$TARGET_DIR/reaper_wingconnector.so"
rm -f "$TARGET_DIR/config.json"

echo "Removed Wing Connector from $TARGET_DIR"
