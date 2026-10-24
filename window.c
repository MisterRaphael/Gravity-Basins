//
// Created by raph on 16/02/2026.
// Window initialization and configuration
//

#include <stdio.h>
#include "raylib.h"
#include "constants.h"

// Create and configure the application window
void create_window(void) {
    // Enable anti-aliasing (doit être appelé AVANT InitWindow)
    SetConfigFlags(FLAG_MSAA_4X_HINT ); // à mettre (synchronisation verticale) : | FLAG_VSYNC_HINT

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gravity Basins");

    SetTargetFPS(FPS);
    
    fprintf(stdout, "window: Window created (%dx%d) at %d FPS\n",
            WINDOW_WIDTH, WINDOW_HEIGHT, FPS);
}
