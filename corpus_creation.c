#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "constants.h"
#include "corpus_creation.h"


Planet *planets = NULL;
ObjectInfo *objects = NULL;


void init_object(double x, double y) {

    objects = malloc(sizeof(ObjectInfo) * number_of_objects);
    if (objects == NULL) {
        fprintf(stderr, "corpus_creation: Failed to allocate objects\n");
        return;
    }

    for (int i = 0; i < number_of_objects; i++) {
        objects[i].x_position = x;
        objects[i].y_position = y;
    }
    fprintf(stdout, "corpus_creation: %d planets initialized\n", number_of_planets);

}


void init_planet(void) {
    planets = malloc(sizeof(Planet) * number_of_planets);
    if (planets == NULL) {
        fprintf(stderr, "corpus_creation: Failed to allocate planets\n");
        return;
    }

    int center_x = WINDOW_WIDTH / 2;
    int center_y = WINDOW_HEIGHT / 2;
    int radius = 50;

    for (int i = 0; i < number_of_planets; i++) {
        planets[i].id = i;
        double angle = (2.0 * M_PI * i / number_of_planets) - (M_PI / 2.0);
        planets[i].x_position = center_x + (int)(radius * cos(angle));
        planets[i].y_position = center_y + (int)(radius * sin(angle));

        // Calcule une teinte répartie sur le cercle de 360 degrés
        float teinte = (float)i * (360.0f / (float)number_of_planets);

        // Saturation = 0.9f (très coloré), Value/Brightess = 0.9f (lumineux)
        planets[i].color = ColorFromHSV(teinte, 0.9f, 0.9f);
        planets[i].color = ColorFromHSV(teinte, 0.9f, 0.9f);
    }
    fprintf(stdout, "corpus_creation: %d planets initialized\n", number_of_planets);
}


void init_random_planet(void) {
    planets = malloc(sizeof(Planet) * number_of_planets);
    if (planets == NULL) {
        fprintf(stderr, "corpus_creation: Failed to allocate planets\n");
        return;
    }

    srand(time(NULL));
    int margin_x = WINDOW_WIDTH / 50;
    int usable_width = WINDOW_WIDTH - 2 * margin_x;
    int margin_y = WINDOW_HEIGHT / 50;
    int usable_height = WINDOW_HEIGHT - 2 * margin_y;

    for (int i = 0; i < number_of_planets; i++) {
        planets[i].id = i;
        planets[i].x_position = margin_x + rand() % usable_width;
        planets[i].y_position = margin_y + rand() % usable_height;

        // Calcule une teinte répartie sur le cercle de 360 degrés
        float teinte = (float)i * (360.0f / (float)number_of_planets);

        // Saturation = 0.9f (très coloré), Value/Brightess = 0.9f (lumineux)
        planets[i].color = ColorFromHSV(teinte, 0.9f, 0.9f);
        planets[i].color = ColorFromHSV(teinte, 0.9f, 0.9f);

        // Pour l'afficher dans la console :
        printf("Planete %d - Teinte: %.1f -> RGBA: (%u, %u, %u, %u)\n",
                i,
                teinte,
                planets[i].color.r,
                planets[i].color.g,
                planets[i].color.b,
                planets[i].color.a);
    }
    fprintf(stdout, "corpus_creation: %d random planets initialized\n", number_of_planets);
}

void cleanup_corpus(void) {
    if (planets != NULL) {
        free(planets);planets = NULL;
    }
    if (objects != NULL) {
        free(objects);
        objects = NULL;
    }
    
    fprintf(stdout, "corpus_creation: Cleanup completed\n");
}



