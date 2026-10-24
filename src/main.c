//
// Created by raph on 16/02/2026.
// uTIPE - Gravity Basins Simulator
// Main entry point
//

#include <stdio.h>
#include <stdbool.h>
#include "raylib.h"

#include "constants.h"
#include "window.h"
#include "draw.h"


int main(int argc, char *argv[]) {
    // 1. Initialisation de la configuration par défaut
    config_init_defaults(&g_config);

    // 2. Traitement des arguments de la ligne de commande et des fichiers de configuration
    if (!config_parse_cli(&g_config, argc, argv)) {
        return 0;
    }

    // 3. Affichage récapitulatif de la configuration retenue
    config_print(&g_config);

    fprintf(stdout, "main: Starting application\n");
    printf("Nombre de pixel de l'interface graphique : %d\n", WINDOW_WIDTH * WINDOW_HEIGHT);

    create_window();
    draw_main();

    CloseWindow();
    fprintf(stdout, "main: Cleanup completed, exiting\n");

    return 0;
}
