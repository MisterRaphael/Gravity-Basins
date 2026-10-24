#ifndef UTIPE_CALCULUS_H
#define UTIPE_CALCULUS_H

#include "corpus_creation.h"

double distance(int *corpus1, int *corpus2);

void future_position(ObjectInfo *object_info, Planet *planets);
void future_position2(ObjectInfo *object_info, Planet *planets);//RK4
double calculate_mechanical_energy(ObjectInfo *object_info, Planet *planets);

/**
 * @brief Calculates the kinetic energy of an object.
 *
 * Kinetic energy is given by the formula: KE = 0.5 * m * v^2
 * where m is the mass of the object and v is its velocity.
 *
 * @param mass The mass of the object.
 * @param vx The velocity of the object in the x-direction.
 * @param vy The velocity of the object in the y-direction.
 * @return The kinetic energy of the object.
 */

double calculate_kinetic_energy(double vx, double vy);
int captured(ObjectInfo *object_info, Planet *planets);

#endif //UTIPE_CALCULUS_H
