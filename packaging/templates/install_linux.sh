#!/usr/bin/env bash
set -euo pipefail

PLUGIN_NAME="reaper_wingconnector.so"
CONFIG_NAME="config.json"
SOURCE_DIR="$(cd "$(dirname "$0")" && pwd)"
TARGET_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/REAPER/UserPlugins"

mkdir -p "$TARGET_DIR"
cp "$SOURCE_DIR/$PLUGIN_NAME" "$TARGET_DIR/$PLUGIN_NAME"
cp "$SOURCE_DIR/$CONFIG_NAME" "$TARGET_DIR/$CONFIG_NAME"
chmod 755 "$TARGET_DIR/$PLUGIN_NAME"

echo "Installed to $TARGET_DIR"
echo "Restart REAPER to load the extension."
