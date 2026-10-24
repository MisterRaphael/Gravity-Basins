#ifndef UTIPE_CORPUS_CREATION_H
#define UTIPE_CORPUS_CREATION_H

#include "raylib.h"

typedef struct {
    double x_position;
    double y_position;
    double x_velocity;
    double y_velocity;
} ObjectInfo;

typedef struct {
    int id;
    Color color;
    double x_position;
    double y_position;
} Planet;

extern Planet *planets;
extern ObjectInfo *objects;

void init_object(double x,double y);
void init_planet(void);
void init_random_planet(void);
void cleanup_corpus(void);

#endif //UTIPE_CORPUS_CREATION_H
