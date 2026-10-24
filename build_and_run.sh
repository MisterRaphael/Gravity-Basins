#!/bin/bash
cd /home/raph/CLionProjects/untitled1/cmake-build-debug
echo "=== Building project ==="
cmake .. 2>&1
if [ $? -eq 0 ]; then
    echo "=== Compiling ==="
    make -j 2>&1
    if [ $? -eq 0 ]; then
        echo "=== Build successful ==="
        echo "=== Running binary (5s timeout) ==="
        timeout 5 ./Gravity_Basins 2>&1 | head -30
    else
        echo "=== Compilation failed ==="
    fi
else
    echo "=== CMake configuration failed ==="
fi
