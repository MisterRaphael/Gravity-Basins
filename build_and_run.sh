#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/cmake-build-debug"

echo "=== Configuration du projet avec CMake ==="
cmake -B "${BUILD_DIR}" -S "${SCRIPT_DIR}"

echo "=== Compilation ==="
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo "=== Lancement de Gravity Basins ==="
"${BUILD_DIR}/Gravity_Basins" "$@"
