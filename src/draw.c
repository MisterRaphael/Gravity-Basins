#include <stdio.h>
#include <math.h>
#include "raylib.h"

#include "constants.h"
#include "draw.h"
#include "corpus_creation.h"
#include "gpu_simulation.h"

static void draw_gpu_scene(GpuSimulation *simulation) {
    gpu_simulation_step(simulation);

    BeginDrawing();
    ClearBackground(BLACK);
    gpu_simulation_draw(simulation);

    for (int i = 0; i < number_of_planets; i++) {
        draw_planet(&planets[i]);
    }

    DrawText("GPU simulation", 10, 10, 20, WHITE);
    DrawFPS(10, 40);
    EndDrawing();
}

void draw_main(void) {
    if (RANDOM_PLANETS) {
        init_random_planet();
    } else {
        init_planet();
    }

    GpuSimulation *gpu_simulation =
            gpu_simulation_create(planets, number_of_planets);
    if (gpu_simulation == NULL) {
        cleanup_corpus();
        return;
    }

    while (!WindowShouldClose()) {
        draw_gpu_scene(gpu_simulation);
    }

    gpu_simulation_destroy(gpu_simulation);
    cleanup_corpus();
}


void draw_planet(Planet *planet) {
    if (planet == NULL) {
        fprintf(stderr, "draw: planet is NULL\n");
        return;
    }
    Vector2 center = {(float)round(planet->x_position), (float)round(planet->y_position)};
    DrawCircleGradient(center, (float)RADIUS_PLANET, BLACK, planet->color);
}
