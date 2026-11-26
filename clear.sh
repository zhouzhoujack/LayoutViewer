#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"
rm -rf -- build/

echo "Removed CMake build directory: build/"
