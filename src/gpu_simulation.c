#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "rlgl.h"

#include "constants.h"
#include "gpu_simulation.h"

#define PLANET_POSITIONS_BINDING 1u
#define PLANET_COLORS_BINDING 2u
#define PARTICLES_BINDING 3u
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000u

#define PARTICLE_ACTIVE 1u

typedef struct {
    float x;
    float y;
    float unused_z;
    float unused_w;
} GpuPlanetPosition;

typedef struct {
    float x;
    float y;
    float velocity_x;
    float velocity_y;
    uint32_t state;
    uint32_t color;
    uint32_t reserved0;
    uint32_t reserved1;
} GpuParticle;

typedef void (*MemoryBarrierFunction)(unsigned int barriers);

_Static_assert(sizeof(GpuPlanetPosition) == 16, "GPU planet layout must match vec4");
_Static_assert(sizeof(GpuParticle) == 32, "GPU particle layout must match the GLSL struct");

struct GpuSimulation {
    int width;
    int height;
    int planet_count;
    unsigned int compute_shader;
    unsigned int compute_program;
    unsigned int planet_positions_buffer;
    unsigned int planet_colors_buffer;
    unsigned int particles_buffer;
    Shader render_shader;
    int steps_per_dispatch_location;
    int render_resolution_location;
    MemoryBarrierFunction memory_barrier;
    bool ready;
};

static uint32_t pack_color(Color color) {
    return (uint32_t)color.r |
           ((uint32_t)color.g << 8u) |
           ((uint32_t)color.b << 16u) |
           ((uint32_t)color.a << 24u);
}

static void set_compute_uniforms(GpuSimulation *simulation) {
    const int width = simulation->width;
    const int height = simulation->height;
    const int planet_count = simulation->planet_count;
    const float time_step = 1.0f / (float)FPS;
    const float gravity = (float)G;
    const float friction = (float)FRICTION_COEFF;
    const float softening_squared = (float)SOFTENING_SQ;
    const float capture_energy = (float)CAPTURATION_TOLERANCE_KINETIC_ENERGY;
    const float capture_distance = (float)CAPTURATION_TOLERANCE_POSITION_DIFFERENCE;

    rlEnableShader(simulation->compute_program);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "width"),
                 &width, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "height"),
                 &height, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "planetCount"),
                 &planet_count, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "timeStep"),
                 &time_step, RL_SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "gravity"),
                 &gravity, RL_SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "friction"),
                 &friction, RL_SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "softeningSquared"),
                 &softening_squared, RL_SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "captureEnergy"),
                 &capture_energy, RL_SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(rlGetLocationUniform(simulation->compute_program, "captureDistance"),
                 &capture_distance, RL_SHADER_UNIFORM_FLOAT, 1);

    simulation->steps_per_dispatch_location =
            rlGetLocationUniform(simulation->compute_program, "stepsPerDispatch");
    rlDisableShader();
}

static void destroy_buffers(GpuSimulation *simulation) {
    if (simulation->particles_buffer != 0) {
        rlUnloadShaderBuffer(simulation->particles_buffer);
    }
    if (simulation->planet_colors_buffer != 0) {
        rlUnloadShaderBuffer(simulation->planet_colors_buffer);
    }
    if (simulation->planet_positions_buffer != 0) {
        rlUnloadShaderBuffer(simulation->planet_positions_buffer);
    }
}

GpuSimulation *gpu_simulation_create(const Planet *planet_list, int planet_count) {
    if (rlGetVersion() != RL_OPENGL_43) {
        fprintf(stderr, "gpu: OpenGL 4.3 is unavailable; using the CPU simulation\n");
        return NULL;
    }
    if (planet_list == NULL || planet_count <= 0 || WINDOW_WIDTH <= 0 || WINDOW_HEIGHT <= 0) {
        fprintf(stderr, "gpu: invalid simulation dimensions or planet list\n");
        return NULL;
    }

    const size_t particle_count = (size_t)WINDOW_WIDTH * (size_t)WINDOW_HEIGHT;
    const size_t particles_size = particle_count * sizeof(GpuParticle);
    const size_t positions_size = (size_t)planet_count * sizeof(GpuPlanetPosition);
    const size_t colors_size = (size_t)planet_count * sizeof(uint32_t);
    if (particles_size > UINT_MAX || positions_size > UINT_MAX || colors_size > UINT_MAX) {
        fprintf(stderr, "gpu: simulation buffers are too large\n");
        return NULL;
    }

    GpuSimulation *simulation = calloc(1, sizeof(*simulation));
    GpuParticle *particles = calloc(particle_count, sizeof(*particles));
    GpuPlanetPosition *positions = calloc((size_t)planet_count, sizeof(*positions));
    uint32_t *colors = calloc((size_t)planet_count, sizeof(*colors));
    if (simulation == NULL || particles == NULL || positions == NULL || colors == NULL) {
        fprintf(stderr, "gpu: unable to allocate initialization data\n");
        free(colors);
        free(positions);
        free(particles);
        free(simulation);
        return NULL;
    }

    simulation->width = WINDOW_WIDTH;
    simulation->height = WINDOW_HEIGHT;
    simulation->planet_count = planet_count;

    for (int y = 0; y < simulation->height; y++) {
        for (int x = 0; x < simulation->width; x++) {
            GpuParticle *particle = &particles[(size_t)y * (size_t)simulation->width + (size_t)x];
            particle->x = (float)x;
            particle->y = (float)y;
            particle->state = PARTICLE_ACTIVE;
            particle->color = 0xff000000u;
        }
    }
    for (int i = 0; i < planet_count; i++) {
        positions[i].x = (float)planet_list[i].x_position;
        positions[i].y = (float)planet_list[i].y_position;
        colors[i] = pack_color(planet_list[i].color);
    }

    char *compute_code = LoadFileText("resources/shaders/gravity_compute.glsl");
    if (compute_code == NULL) {
        fprintf(stderr, "gpu: unable to load gravity_compute.glsl\n");
        free(colors);
        free(positions);
        free(particles);
        free(simulation);
        return NULL;
    }

    simulation->compute_shader = rlLoadShader(compute_code, RL_COMPUTE_SHADER);
    UnloadFileText(compute_code);
    simulation->compute_program = rlLoadShaderProgramCompute(simulation->compute_shader);
    simulation->render_shader = LoadShader("resources/shaders/gravity_render.vs",
                                           "resources/shaders/gravity_render.fs");
    simulation->memory_barrier =
            (MemoryBarrierFunction)rlGetProcAddress("glMemoryBarrier");

    if (simulation->compute_shader == 0 || simulation->compute_program == 0 ||
        simulation->render_shader.id == 0 || simulation->memory_barrier == NULL) {
        fprintf(stderr, "gpu: shader compilation failed; using the CPU simulation\n");
        if (simulation->render_shader.id != 0) {
            UnloadShader(simulation->render_shader);
        }
        if (simulation->compute_program != 0) {
            rlUnloadShaderProgram(simulation->compute_program);
        }
        if (simulation->compute_shader != 0) {
            rlUnloadShader(simulation->compute_shader);
        }
        free(colors);
        free(positions);
        free(particles);
        free(simulation);
        return NULL;
    }

    simulation->planet_positions_buffer =
            rlLoadShaderBuffer((unsigned int)positions_size, positions, RL_STATIC_DRAW);
    simulation->planet_colors_buffer =
            rlLoadShaderBuffer((unsigned int)colors_size, colors, RL_STATIC_DRAW);
    simulation->particles_buffer =
            rlLoadShaderBuffer((unsigned int)particles_size, particles, RL_DYNAMIC_COPY);
    free(colors);
    free(positions);
    free(particles);

    if (simulation->planet_positions_buffer == 0 || simulation->planet_colors_buffer == 0 ||
        simulation->particles_buffer == 0) {
        fprintf(stderr, "gpu: unable to create shader buffers; using the CPU simulation\n");
        destroy_buffers(simulation);
        UnloadShader(simulation->render_shader);
        rlUnloadShaderProgram(simulation->compute_program);
        rlUnloadShader(simulation->compute_shader);
        free(simulation);
        return NULL;
    }

    set_compute_uniforms(simulation);
    simulation->render_resolution_location =
            GetShaderLocation(simulation->render_shader, "resolution");
    simulation->ready = true;
    fprintf(stdout, "gpu: %zu trajectories initialized in GPU memory\n", particle_count);
    return simulation;
}

void gpu_simulation_step(GpuSimulation *simulation) {
    if (simulation == NULL || !simulation->ready) {
        return;
    }

    const int steps_per_dispatch = GPU_STEPS_PER_DISPATCH > 0 ?
            GPU_STEPS_PER_DISPATCH : 1;
    const unsigned int groups_x = (unsigned int)(simulation->width + 15) / 16u;
    const unsigned int groups_y = (unsigned int)(simulation->height + 15) / 16u;

    rlEnableShader(simulation->compute_program);
    rlSetUniform(simulation->steps_per_dispatch_location, &steps_per_dispatch,
                 RL_SHADER_UNIFORM_INT, 1);
    rlBindShaderBuffer(simulation->planet_positions_buffer, PLANET_POSITIONS_BINDING);
    rlBindShaderBuffer(simulation->planet_colors_buffer, PLANET_COLORS_BINDING);
    rlBindShaderBuffer(simulation->particles_buffer, PARTICLES_BINDING);
    rlComputeShaderDispatch(groups_x, groups_y, 1);
    rlDisableShader();

    // Makes SSBO writes visible to the fragment shader used for the next draw.
    simulation->memory_barrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void gpu_simulation_draw(const GpuSimulation *simulation) {
    if (simulation == NULL || !simulation->ready) {
        return;
    }

    const Vector2 resolution = {(float)simulation->width, (float)simulation->height};
    rlBindShaderBuffer(simulation->particles_buffer, PARTICLES_BINDING);
    SetShaderValue(simulation->render_shader,
                   simulation->render_resolution_location,
                   &resolution, SHADER_UNIFORM_VEC2);

    BeginShaderMode(simulation->render_shader);
    DrawRectangle(0, 0, simulation->width, simulation->height, WHITE);
    EndShaderMode();
}

void gpu_simulation_destroy(GpuSimulation *simulation) {
    if (simulation == NULL) {
        return;
    }

    destroy_buffers(simulation);
    if (simulation->render_shader.id != 0) {
        UnloadShader(simulation->render_shader);
    }
    if (simulation->compute_program != 0) {
        rlUnloadShaderProgram(simulation->compute_program);
    }
    if (simulation->compute_shader != 0) {
        rlUnloadShader(simulation->compute_shader);
    }
    free(simulation);
}
