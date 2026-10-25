#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/cmake-build-release"

echo "=== Configuration du projet avec CMake ==="
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release

echo "=== Compilation ==="
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo "=== Lancement de Gravity Basins ==="
"${BUILD_DIR}/Gravity_Basins" "$@"
