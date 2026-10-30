#ifndef UTIPE_CORPUS_CREATION_H
#define UTIPE_CORPUS_CREATION_H

#include <stdbool.h>
#include "raylib.h"

typedef struct {
    int id;
    Color color;
    double x_position;
    double y_position;
} Planet;

extern Planet *planets;

bool init_planets(void);
void cleanup_corpus(void);

#endif //UTIPE_CORPUS_CREATION_H
