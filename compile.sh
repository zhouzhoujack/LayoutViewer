#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

rm -rf build/
# layout-core 预编译包只带了 Debug 配置，这里固定 Debug 并与 .vscode/tasks.json 保持一致。
# 并行度固定 -j3：本机内存有限，-j$(nproc) 会把 Qt/TU 较大的目标编译到 OOM。
cmake -S src -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j3 "$@"

