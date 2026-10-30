#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include "constants.h"
#include "corpus_creation.h"


Planet *planets = NULL;

static Color planet_color(int index) {
    const float hue = (float)index * (360.0f / (float)number_of_planets);
    return ColorFromHSV(hue, 0.9f, 0.9f);
}

static bool allocate_planets(void) {
    if (number_of_planets <= 0) {
        fprintf(stderr, "corpus_creation: planet count must be positive\n");
        return false;
    }

    planets = calloc((size_t)number_of_planets, sizeof(*planets));
    if (planets == NULL) {
        fprintf(stderr, "corpus_creation: Failed to allocate planets\n");
        return false;
    }
    return true;
}

static bool save_planet_layout(void) {
    FILE *file = fopen(g_config.planet_layout_file, "w");
    if (file == NULL) {
        fprintf(stderr, "corpus_creation: cannot save planet layout to '%s'\n",
                g_config.planet_layout_file);
        return false;
    }

    fprintf(file, "# One planet position (x y) per line; keep the same planet count to reload.\n");
    for (int i = 0; i < number_of_planets; i++) {
        if (fprintf(file, "%.17g %.17g\n",
                    planets[i].x_position, planets[i].y_position) < 0) {
            fprintf(stderr, "corpus_creation: error writing planet layout '%s'\n",
                    g_config.planet_layout_file);
            fclose(file);
            return false;
        }
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "corpus_creation: error closing planet layout '%s'\n",
                g_config.planet_layout_file);
        return false;
    }

    fprintf(stdout, "corpus_creation: random planet layout saved to '%s'\n",
            g_config.planet_layout_file);
    return true;
}

static bool load_planet_layout(void) {
    FILE *file = fopen(g_config.planet_layout_file, "r");
    if (file == NULL) {
        fprintf(stderr, "corpus_creation: cannot load planet layout '%s'\n",
                g_config.planet_layout_file);
        return false;
    }

    char line[256];
    int planet_index = 0;
    int line_number = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        char *comment = strchr(line, '#');
        if (comment != NULL) {
            *comment = '\0';
        }

        char *position_text = line;
        while (isspace((unsigned char)*position_text)) {
            position_text++;
        }
        if (*position_text == '\0') {
            continue;
        }

        double x;
        double y;
        char extra;
        if (sscanf(position_text, "%lf %lf %c", &x, &y, &extra) != 2 ||
            !isfinite(x) || !isfinite(y) ||
            x < 0.0 || x >= (double)WINDOW_WIDTH ||
            y < 0.0 || y >= (double)WINDOW_HEIGHT) {
            fprintf(stderr, "corpus_creation: invalid position on line %d of '%s'\n",
                    line_number, g_config.planet_layout_file);
            fclose(file);
            return false;
        }
        if (planet_index >= number_of_planets) {
            fprintf(stderr, "corpus_creation: too many positions in '%s'\n",
                    g_config.planet_layout_file);
            fclose(file);
            return false;
        }

        planets[planet_index].id = planet_index;
        planets[planet_index].x_position = x;
        planets[planet_index].y_position = y;
        planets[planet_index].color = planet_color(planet_index);
        planet_index++;
    }

    const bool read_error = ferror(file) != 0;
    fclose(file);
    if (read_error || planet_index != number_of_planets) {
        fprintf(stderr,
                "corpus_creation: '%s' must contain exactly %d valid positions\n",
                g_config.planet_layout_file, number_of_planets);
        return false;
    }

    fprintf(stdout, "corpus_creation: loaded %d planet positions from '%s'\n",
            number_of_planets, g_config.planet_layout_file);
    return true;
}

bool init_planets(void) {
    if (!allocate_planets()) {
        return false;
    }

    int center_x = WINDOW_WIDTH / 2;
    int center_y = WINDOW_HEIGHT / 2;
    int radius = WINDOW_WIDTH/8;

    if (g_config.planet_layout == PLANET_LAYOUT_SAVED) {
        if (!load_planet_layout()) {
            cleanup_corpus();
            return false;
        }
    } else if (g_config.planet_layout == PLANET_LAYOUT_CIRCLE) {
        for (int i = 0; i < number_of_planets; i++) {
            planets[i].id = i;
            if (number_of_planets == 1) {
                planets[i].x_position = center_x;
                planets[i].y_position = center_y;
            } else {
                const double angle = (2.0 * M_PI * i / number_of_planets) - (M_PI / 2.0);
                planets[i].x_position = center_x + (int)(radius * cos(angle));
                planets[i].y_position = center_y + (int)(radius * sin(angle));
            }
            planets[i].color = planet_color(i);
        }
    } else if (g_config.planet_layout == PLANET_LAYOUT_RANDOM) {
        srand((unsigned int)time(NULL));
        const int margin_x = WINDOW_WIDTH / 50;
        const int usable_width = WINDOW_WIDTH - 2 * margin_x;
        const int margin_y = WINDOW_HEIGHT / 50;
        const int usable_height = WINDOW_HEIGHT - 2 * margin_y;
        if (usable_width <= 0 || usable_height <= 0) {
            fprintf(stderr, "corpus_creation: window is too small for random planets\n");
            cleanup_corpus();
            return false;
        }

        for (int i = 0; i < number_of_planets; i++) {
            planets[i].id = i;
            planets[i].x_position = margin_x + rand() % usable_width;
            planets[i].y_position = margin_y + rand() % usable_height;
            planets[i].color = planet_color(i);
        }

        if (!save_planet_layout()) {
            cleanup_corpus();
            return false;
        }
    } else {
        fprintf(stderr, "corpus_creation: unknown planet layout mode\n");
        cleanup_corpus();
        return false;
    }
    fprintf(stdout, "corpus_creation: %d planets initialized\n", number_of_planets);
    return true;
}

void cleanup_corpus(void) {
    if (planets != NULL) {
        free(planets);planets = NULL;
    }
    fprintf(stdout, "corpus_creation: Cleanup completed\n");
}



