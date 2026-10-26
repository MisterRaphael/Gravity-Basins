#ifndef UTIPE_CONFIG_H
#define UTIPE_CONFIG_H

#include <stdbool.h>

typedef struct {
    // Fenêtre
    int window_width;
    int window_height;
    int fps;

    // Corpus / Objets / Planètes
    int number_of_objects;
    int number_of_planets;
    double radius_object;
    double radius_planet;
    bool random_planets;

    // Physique
    double g;
    double friction_coeff;
    double softening_sq;

    // Capture
    double capturation_tolerance_kinetic_energy;
    double capturation_tolerance_position_difference;

    // Vitesse de simulation et rendu
    int sub_steps;
    int sim_steps_per_frame;
    int texture_update_steps;
    int gpu_steps_per_dispatch;
} Config;

// Instance globale accessible partout
extern Config g_config;

// Initialise la structure avec les valeurs par défaut
void config_init_defaults(Config *cfg);

// Charge la configuration depuis un fichier (.cfg ou .yaml avec syntaxe clé: valeur ou clé = valeur)
bool config_load_from_file(Config *cfg, const char *filepath);

// Parse les arguments de la ligne de commande (CLI)
// Retourne true pour continuer, false pour quitter proprement (ex: après --help)
bool config_parse_cli(Config *cfg, int argc, char **argv);

// Affiche la configuration courante sur la sortie standard
void config_print(const Config *cfg);

#endif // UTIPE_CONFIG_H

