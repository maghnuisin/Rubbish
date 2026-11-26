#!/bin/bash
# Double-click to build Rubbish, install it for Logic, and check it with auval.
cd "$(dirname "$0")" || exit 1
export PATH="/opt/homebrew/bin:/usr/local/bin:$PATH"
mkdir -p build
LOG="build/build-output.txt"

{
    echo "=== Rubbish build $(date)"
    if ! command -v cmake >/dev/null 2>&1; then
        echo "ERROR: cmake is not installed (or not found)."
    elif cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release -j 8; then
        echo "=== BUILD OK"
        echo "=== auval check"
        auval -v aufx Rbsh Rbsh | tail -8
    else
        echo "=== BUILD FAILED"
    fi
} 2>&1 | tee "$LOG"

# Ask macOS to re-read installed plugins so Logic sees the new version.
killall -9 AudioComponentRegistrar 2>/dev/null

echo
echo "Finished. Quit Logic completely (Cmd+Q) and open it again."
read -n 1 -s -r -p "Press any key to close this window."
