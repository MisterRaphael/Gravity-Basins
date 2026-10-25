#ifndef GRAVITY_BASINS_GPU_SIMULATION_H
#define GRAVITY_BASINS_GPU_SIMULATION_H

#include <stdbool.h>

#include "corpus_creation.h"

typedef struct GpuSimulation GpuSimulation;

// Creates one GPU state per pixel. Returns NULL when OpenGL 4.3 or a shader is unavailable.
GpuSimulation *gpu_simulation_create(const Planet *planet_list, int planet_count);
void gpu_simulation_step(GpuSimulation *simulation);
void gpu_simulation_draw(const GpuSimulation *simulation);
void gpu_simulation_destroy(GpuSimulation *simulation);

#endif
