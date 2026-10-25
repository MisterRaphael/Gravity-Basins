#ifndef UTIPE_CONSTANTS_H
#define UTIPE_CONSTANTS_H

#include "config.h"

// Redirection dynamique vers l'instance de configuration globale g_config
// Cela préserve la compatibilité avec tout le code existant sans devoir changer les signatures.

// Fenêtre
#define WINDOW_WIDTH                              (g_config.window_width)
#define WINDOW_HEIGHT                             (g_config.window_height)
#define FPS                                       (g_config.fps)

// Corpus
#define number_of_objects                         (g_config.number_of_objects)
#define RADIUS_OBJECT                             (g_config.radius_object)

// Planètes
#define number_of_planets                         (g_config.number_of_planets)
#define RADIUS_PLANET                             (g_config.radius_planet)

// Physique
#define G                                         (g_config.g)
#define FRICTION_COEFF                            (g_config.friction_coeff)

// Simulation
#define SUB_STEPS                                 (g_config.sub_steps)

// Optimisation & Détection de capture
#define SOFTENING_SQ                              (g_config.softening_sq)
#define CAPTURATION_TOLERANCE_KINETIC_ENERGY      (g_config.capturation_tolerance_kinetic_energy)
#define CAPTURATION_TOLERANCE_POSITION_DIFFERENCE (g_config.capturation_tolerance_position_difference)

// Simulation par frame & Texture
#define SIM_STEPS_PER_FRAME                       (g_config.sim_steps_per_frame)
#define TEXTURE_UPDATE_STEPS                      (g_config.texture_update_steps)
#define GPU_STEPS_PER_DISPATCH                    (g_config.gpu_steps_per_dispatch)

#endif //UTIPE_CONSTANTS_H
