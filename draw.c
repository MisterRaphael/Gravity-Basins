#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "raylib.h"
#include <math.h>

#include "constants.h"
#include "draw.h"


#include "calculus.h"
#include "corpus_creation.h"

void draw_main(void) {

    //initialisation
    Image pixel_image = GenImageColor(WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);

    Texture2D pixel_texture = LoadTextureFromImage(pixel_image);

    int next_x = 0;
    int next_y = 0;

    init_object((double)next_x, (double)next_y);

    init_planet();
    bool simulation_complete = false;
    int pixel_count = 0;  // compteur pour TEXTURE_UPDATE_STEPS


    while (!WindowShouldClose()) {
        // Simulation : plusieurs itérations de physique par frame de rendu
        if (!simulation_complete) {
            for (int sim = 0; sim < SIM_STEPS_PER_FRAME; sim++) {
                future_position(&objects[0], planets);

                int c = captured(&objects[0], planets);
                if (c != -1) {
                    // Marquer le pixel de couleur à la position de départ de l'objet (next_x, next_y)
                    ImageDrawPixel(&pixel_image, next_x, next_y, planets[c].color);
                    pixel_count++;
                    if (pixel_count >= TEXTURE_UPDATE_STEPS) {
                        UpdateTexture(pixel_texture, pixel_image.data);
                        pixel_count = 0;
                    }

                    // Faire disparaître l'objet capturé en le repositionnant à droite
                    next_x++;
                    if (next_x >= WINDOW_WIDTH) {
                        // Ligne terminée, passer à la ligne suivante
                        next_x = 0;
                        next_y++;
                    }
                    if (next_y >= WINDOW_HEIGHT) {
                        // Toute la map est colorée, fin de simulation
                        simulation_complete = true;
                        break;
                    }
                    else {
                        // Faire apparaître un nouvel objet à la position suivante
                        objects[0].x_position = (double)next_x;
                        objects[0].y_position = (double)next_y;
                        objects[0].x_velocity = 0.0;
                        objects[0].y_velocity = 0.0;
                    }
                }
            }
        }

        // Rendu : une seule fois par frame
        BeginDrawing();
        ClearBackground(BLACK);

        // Afficher la texture des pixels colorés en premier (en arrière-plan)
        DrawTexture(pixel_texture, 0, 0, WHITE);

        for (int i = 0; i < number_of_planets; i++) {
            draw_planet(&planets[i]);
        }

        if (!simulation_complete) {
            draw_object(&objects[0]);
        }

        if (simulation_complete) {
            DrawText("Simulation complete", 160, 150, 20, WHITE);
        }

        DrawFPS(10, 40);
        EndDrawing();
    }

    // Mettre à jour la texture une dernière fois pour afficher les pixels restants
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

