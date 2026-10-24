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


int main(void) {
    fprintf(stdout, "main: Starting application\n");
    printf("Nombre de pixel de l'interface graphique :%d", WINDOW_WIDTH*WINDOW_HEIGHT);

    create_window();
    draw_main();


    CloseWindow();
    fprintf(stdout, "main: Cleanup completed, exiting\n");

    return 0;
}
