#include <math.h>
#include <stdbool.h>

#include "calculus.h"
#include "corpus_creation.h"
#include "constants.h"



double calculate_kinetic_energy(double vx, double vy) {
    double kinetic_energy = 0.5 * 1*(vx * vx + vy * vy);
    //DrawText(TextFormat("Kinetic Energy : %.0f J", kinetic_energy), 160, 130, 20, WHITE);
    return kinetic_energy;
}

int captured(ObjectInfo *object_info, Planet *planets){
    double kinetic_energy = calculate_kinetic_energy(object_info->x_velocity, object_info->y_velocity);
    for (int i = 0; i < number_of_planets; i++) {
        if (kinetic_energy <= CAPTURATION_TOLERANCE_KINETIC_ENERGY && fabs(object_info->x_position - planets[i].x_position) <= CAPTURATION_TOLERANCE_POSITION_DIFFERENCE && fabs(object_info->y_position - planets[i].y_position) <= CAPTURATION_TOLERANCE_POSITION_DIFFERENCE )  {
            //DrawText(TextFormat("captured by %d", i), 160, 150, 20, WHITE);
            return planets[i].id;
        }
    }
    //DrawText("captured : False", 160, 150, 20, WHITE);

    return -1;
}


void future_position(ObjectInfo *object_info, Planet *planets) {

    int sub_steps = SUB_STEPS; // Accélère la simulation d'un facteur {x} visuellement
    double physics_dt = 1.0 / (double)FPS ; // Le dt doit rester petit pour la stabilité

    // On charge l'état initial pour Registres CPU
    double x = object_info->x_position;
    double y = object_info->y_position;
    double vx = object_info->x_velocity;
    double vy = object_info->y_velocity;
    double ax = 0.0;
    double ay = 0.0;

    for (int s = 0; s < sub_steps; s++) {
        ax = 0.0;
        ay = 0.0;

        //calcul des distances
        for (int i = 0; i < number_of_planets; i++) {
            double px = planets[i].x_position;
            double py = planets[i].y_position;
            double dx = px - x;
            double dy = py - y;
            double r = dx * dx + dy * dy + SOFTENING_SQ;

            double inv_r3 = 1.0 / (r * sqrt(r));
            ax += dx * inv_r3;
            ay += dy * inv_r3;

        }

        ax *= G;
        ay *= G;

        // Mise à jour de l'état
        vx += ax * physics_dt - FRICTION_COEFF * vx * physics_dt;
        vy += ay * physics_dt - FRICTION_COEFF * vy * physics_dt;
        x  += vx * physics_dt;
        y  += vy * physics_dt;

    }

    // 3. On sauvegarde le résultat final en mémoire
    object_info->x_position = x;
    object_info->y_position = y;
    object_info->x_velocity = vx;
    object_info->y_velocity = vy;
}