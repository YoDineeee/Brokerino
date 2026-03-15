#!/bin/bash

set -e  # stop on any error

# Paths 
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT_DIR/build"
BINARY="$BUILD_DIR/src/brokerino"
CONFIG="$ROOT_DIR/brokerino.conf"

# Build if binary doesn't exist 
if [ ! -f "$BINARY" ]; then
    echo "[run_broker] Binary not found -- building first..."
    cmake -S "$ROOT_DIR" -B "$BUILD_DIR"
    cmake --build "$BUILD_DIR"
fi

# Create default config if none exists 
if [ ! -f "$CONFIG" ]; then
    echo "[run_broker] No config found -- creating default brokerino.conf"
    cat > "$CONFIG" << EOF
port=1883
db_path=brokerino.db
log_level=info
auth_enabled=true
EOF
fi

#  Run 
echo "[run_broker] Starting BROKERINO..."
"$BINARY" "$CONFIG"