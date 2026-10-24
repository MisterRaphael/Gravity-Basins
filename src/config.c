#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Définition de l'instance globale
Config g_config;

static char *trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return str;
}

static int ci_strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = (char)tolower((unsigned char)*s1);
        char c2 = (char)tolower((unsigned char)*s2);
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

void config_init_defaults(Config *cfg) {
    if (!cfg) return;

    cfg->window_width = 1700;
    cfg->window_height = (cfg->window_width * 9) / 16;
    cfg->fps = 60;

    cfg->number_of_objects = 1;
    cfg->number_of_planets = 3;
    cfg->radius_object = (double)cfg->window_height / 85.0;
    cfg->radius_planet = (double)cfg->window_width / 70.0;

    cfg->g = 100000.0;
    cfg->friction_coeff = 0.1;
    cfg->softening_sq = 200.0;

    cfg->capturation_tolerance_kinetic_energy = 150.0;
    cfg->capturation_tolerance_position_difference = 30.0;

    cfg->sub_steps = 1000;
    cfg->sim_steps_per_frame = 5000;
    cfg->texture_update_steps = cfg->window_width;
}

static void apply_key_value(Config *cfg, const char *key, const char *val) {
    if (ci_strcmp(key, "WINDOW_WIDTH") == 0 || ci_strcmp(key, "width") == 0) {
        cfg->window_width = atoi(val);
        // Si la hauteur n'a pas été explicitement fixée, ajuster le ratio 16:9
        cfg->window_height = (cfg->window_width * 9) / 16;
        cfg->radius_planet = (double)cfg->window_width / 70.0;
        cfg->radius_object = (double)cfg->window_height / 85.0;
        cfg->texture_update_steps = cfg->window_width;
    } else if (ci_strcmp(key, "WINDOW_HEIGHT") == 0 || ci_strcmp(key, "height") == 0) {
        cfg->window_height = atoi(val);
        cfg->radius_object = (double)cfg->window_height / 85.0;
    } else if (ci_strcmp(key, "FPS") == 0) {
        cfg->fps = atoi(val);
    } else if (ci_strcmp(key, "NUMBER_OF_PLANETS") == 0 || ci_strcmp(key, "planets") == 0) {
        cfg->number_of_planets = atoi(val);
    } else if (ci_strcmp(key, "NUMBER_OF_OBJECTS") == 0 || ci_strcmp(key, "objects") == 0) {
        cfg->number_of_objects = atoi(val);
    } else if (ci_strcmp(key, "RADIUS_PLANET") == 0) {
        cfg->radius_planet = atof(val);
    } else if (ci_strcmp(key, "RADIUS_OBJECT") == 0) {
        cfg->radius_object = atof(val);
    } else if (ci_strcmp(key, "G") == 0 || ci_strcmp(key, "gravity") == 0) {
        cfg->g = atof(val);
    } else if (ci_strcmp(key, "FRICTION_COEFF") == 0 || ci_strcmp(key, "friction") == 0 || ci_strcmp(key, "f") == 0) {
        cfg->friction_coeff = atof(val);
    } else if (ci_strcmp(key, "SUB_STEPS") == 0 || ci_strcmp(key, "sub_steps") == 0) {
        cfg->sub_steps = atoi(val);
    } else if (ci_strcmp(key, "SOFTENING_SQ") == 0 || ci_strcmp(key, "softening") == 0) {
        cfg->softening_sq = atof(val);
    } else if (ci_strcmp(key, "CAPTURATION_TOLERANCE_KINETIC_ENERGY") == 0 ||
               ci_strcmp(key, "capture_ke") == 0 || ci_strcmp(key, "capture_kinetic") == 0) {
        cfg->capturation_tolerance_kinetic_energy = atof(val);
    } else if (ci_strcmp(key, "CAPTURATION_TOLERANCE_POSITION_DIFFERENCE") == 0 ||
               ci_strcmp(key, "capture_pos") == 0 || ci_strcmp(key, "capture_position") == 0) {
        cfg->capturation_tolerance_position_difference = atof(val);
    } else if (ci_strcmp(key, "SIM_STEPS_PER_FRAME") == 0 || ci_strcmp(key, "sim_steps") == 0) {
        cfg->sim_steps_per_frame = atoi(val);
    } else if (ci_strcmp(key, "TEXTURE_UPDATE_STEPS") == 0 || ci_strcmp(key, "texture_steps") == 0) {
        cfg->texture_update_steps = atoi(val);
    } else {
        fprintf(stderr, "[config] Avertissement: clé inconnue '%s' ignorée\n", key);
    }
}

bool config_load_from_file(Config *cfg, const char *filepath) {
    if (!cfg || !filepath) return false;

    FILE *f = fopen(filepath, "r");
    if (!f) {
        fprintf(stderr, "[config] Erreur: impossible d'ouvrir le fichier de configuration '%s'\n", filepath);
        return false;
    }

    char line[512];
    int line_number = 0;

    while (fgets(line, sizeof(line), f)) {
        line_number++;

        // Supprimer les commentaires en fin de ligne (# ou ;)
        char *comment = strpbrk(line, "#;");
        if (comment) {
            *comment = '\0';
        }

        char *trimmed = trim_whitespace(line);
        if (*trimmed == '\0') {
            continue; // Ligne vide ou commentaire complet
        }

        // Séparateur : '=' ou ':' (supporte .cfg et .yaml simple)
        char *sep = strpbrk(trimmed, "=:");
        if (!sep) {
            fprintf(stderr, "[config] Ligne %d invalide (aucun '=' ou ':'): '%s'\n", line_number, trimmed);
            continue;
        }

        *sep = '\0';
        char *key = trim_whitespace(trimmed);
        char *val = trim_whitespace(sep + 1);

        if (*key && *val) {
            apply_key_value(cfg, key, val);
        }
    }

    fclose(f);
    fprintf(stdout, "[config] Configuration chargée avec succès depuis '%s'\n", filepath);
    return true;
}

static void print_usage(const char *prog_name) {
    printf("Usage: %s [options] [fichier_config.(cfg|yaml)]\n\n", prog_name);
    printf("Options:\n");
    printf("  -h, --help                 Affiche ce message d'aide et quitte\n");
    printf("  -c, --config <fichier>     Charge le fichier de configuration spécifié\n");
    printf("  -g, --g <valeur>           Constante gravitationnelle G (défaut: 100000)\n");
    printf("  -f, --friction <valeur>    Coefficient de frottement (défaut: 0.1)\n");
    printf("  -p, --planets <nombre>     Nombre de planètes (défaut: 3)\n");
    printf("  -s, --sub-steps <nombre>   Sous-pas de physique par calcul (défaut: 1000)\n");
    printf("  -m, --sim-steps <nombre>   Itérations de physique par frame (défaut: 5000)\n");
    printf("  -w, --width <pixels>       Largeur de la fenêtre (défaut: 1700)\n\n");
    printf("Exemples:\n");
    printf("  %s configs/default.cfg\n", prog_name);
    printf("  %s --g 150000 --friction 0.05\n", prog_name);
    printf("  %s configs/default.cfg --planets 4\n\n", prog_name);
}

bool config_parse_cli(Config *cfg, int argc, char **argv) {
    if (!cfg) return false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return false;
        } else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) && i + 1 < argc) {
            config_load_from_file(cfg, argv[++i]);
        } else if ((strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--g") == 0 || strcmp(argv[i], "--gravity") == 0) && i + 1 < argc) {
            cfg->g = atof(argv[++i]);
        } else if ((strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--friction") == 0) && i + 1 < argc) {
            cfg->friction_coeff = atof(argv[++i]);
        } else if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--planets") == 0) && i + 1 < argc) {
            cfg->number_of_planets = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--sub-steps") == 0) && i + 1 < argc) {
            cfg->sub_steps = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--sim-steps") == 0) && i + 1 < argc) {
            cfg->sim_steps_per_frame = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0) && i + 1 < argc) {
            cfg->window_width = atoi(argv[++i]);
            cfg->window_height = (cfg->window_width * 9) / 16;
            cfg->radius_planet = (double)cfg->window_width / 70.0;
            cfg->radius_object = (double)cfg->window_height / 85.0;
            cfg->texture_update_steps = cfg->window_width;
        } else if (argv[i][0] != '-') {
            // Argument positionnel : fichier de configuration
            config_load_from_file(cfg, argv[i]);
        } else {
            fprintf(stderr, "[config] Option inconnue: %s (utilisez --help pour voir la liste)\n", argv[i]);
        }
    }

    return true;
}

void config_print(const Config *cfg) {
    if (!cfg) return;

    printf("\n================ Configuration Gravity Basins ================\n");
    printf("  Fenêtre           : %d x %d (%d FPS)\n", cfg->window_width, cfg->window_height, cfg->fps);
    printf("  Planètes          : %d (rayon: %.1f px)\n", cfg->number_of_planets, cfg->radius_planet);
    printf("  Objets            : %d (rayon: %.1f px)\n", cfg->number_of_objects, cfg->radius_object);
    printf("  Constante G       : %.1f\n", cfg->g);
    printf("  Frottement        : %.4f\n", cfg->friction_coeff);
    printf("  Softening Sq      : %.1f\n", cfg->softening_sq);
    printf("  Capture KE tol    : %.1f J\n", cfg->capturation_tolerance_kinetic_energy);
    printf("  Capture Pos tol   : %.1f px\n", cfg->capturation_tolerance_position_difference);
    printf("  Sub-steps         : %d\n", cfg->sub_steps);
    printf("  Sim-steps / frame : %d\n", cfg->sim_steps_per_frame);
    printf("  Texture update    : tous les %d pixels\n", cfg->texture_update_steps);
    printf("==============================================================\n\n");
}

