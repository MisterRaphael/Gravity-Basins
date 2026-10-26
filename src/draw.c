#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "raylib.h"

#include "constants.h"
#include "draw.h"
#include "calculus.h"
#include "corpus_creation.h"
#include "gpu_simulation.h"

static void update_texture_when_needed(Texture2D texture, Image image, int *pixel_count) {
    if (*pixel_count >= TEXTURE_UPDATE_STEPS) {
        UpdateTexture(texture, image.data);
        *pixel_count = 0;
    }
}

static bool advance_to_next_pixel(int *pixel_x, int *pixel_y) {
    (*pixel_x)++;
    if (*pixel_x >= WINDOW_WIDTH) {
        *pixel_x = 0;
        (*pixel_y)++;
    }

    return *pixel_y >= WINDOW_HEIGHT;
}

static bool process_capture(int captured_planet, int *pixel_x, int *pixel_y,
                            Image pixel_image, Texture2D pixel_texture,
                            int *pixel_count) {
    ImageDrawPixel(&pixel_image, *pixel_x, *pixel_y, planets[captured_planet].color);
    (*pixel_count)++;
    update_texture_when_needed(pixel_texture, pixel_image, pixel_count);

    if (advance_to_next_pixel(pixel_x, pixel_y)) {
        return true;
    }

    objects[0].x_position = (double)*pixel_x;
    objects[0].y_position = (double)*pixel_y;
    objects[0].x_velocity = 0.0;
    objects[0].y_velocity = 0.0;
    return false;
}

static bool simulate_frame(int *pixel_x, int *pixel_y, Image pixel_image,
                           Texture2D pixel_texture, int *pixel_count) {
    for (int step = 0; step < SIM_STEPS_PER_FRAME; step++) {
        future_position(&objects[0], planets);

        int captured_planet = captured(&objects[0], planets);
        if (captured_planet != -1 &&
            process_capture(captured_planet, pixel_x, pixel_y,
                            pixel_image, pixel_texture, pixel_count)) {
            return true;
        }
    }

    return false;
}

static void draw_scene(Texture2D pixel_texture, bool simulation_complete) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(pixel_texture, 0, 0, WHITE);

    for (int i = 0; i < number_of_planets; i++) {
        draw_planet(&planets[i]);
    }

    if (simulation_complete) {
        DrawText("Simulation complete", 160, 150, 20, WHITE);
    } else {
        draw_object(&objects[0]);
    }

    DrawFPS(10, 40);
    EndDrawing();
}

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
    if (gpu_simulation != NULL) {
        while (!WindowShouldClose()) {
            draw_gpu_scene(gpu_simulation);
        }

        gpu_simulation_destroy(gpu_simulation);
        cleanup_corpus();
        return;
    }

    // CPU fallback for systems without OpenGL 4.3 compute shader support.
    // L'image conserve les pixels déjà calculés entre les frames.
    Image pixel_image = GenImageColor(WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);
    Texture2D pixel_texture = LoadTextureFromImage(pixel_image);

    int pixel_x = 0;
    int pixel_y = 0;
    init_object((double)pixel_x, (double)pixel_y);

    bool simulation_complete = false;
    int pixel_count = 0;

    while (!WindowShouldClose()) {
        if (!simulation_complete) {
            simulation_complete = simulate_frame(&pixel_x, &pixel_y,
                                                 pixel_image, pixel_texture,
                                                 &pixel_count);
        }

        draw_scene(pixel_texture, simulation_complete);
    }

    // Envoyer les derniers pixels calculés à la texture.
    if (pixel_count > 0) {
        UpdateTexture(pixel_texture, pixel_image.data);
    }

    cleanup_corpus();
    UnloadImage(pixel_image);
    UnloadTexture(pixel_texture);
}


void draw_planet(Planet *planet) {
    if (planet == NULL) {
        fprintf(stderr, "draw: planet is NULL\n");
        return;
    }
    Vector2 center = {(float)round(planet->x_position), (float)round(planet->y_position)};
    DrawCircleGradient(center, (float)RADIUS_PLANET, BLACK, planet->color);
}

void draw_object(ObjectInfo *object) {
    if (object == NULL) {
        fprintf(stderr, "draw: object is NULL\n");
        return;
    }
    Vector2 center = {(float)round(object->x_position), (float)round(object->y_position)};
    DrawCircleGradient(center, (float)RADIUS_OBJECT, BEIGE, GRAY);
}

