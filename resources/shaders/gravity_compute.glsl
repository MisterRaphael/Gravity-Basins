#version 430

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct Particle {
    vec4 positionVelocity;
    uint state;
    uint color;
    uint reserved0;
    uint reserved1;
};

layout(std430, binding = 1) readonly buffer PlanetPositions {
    vec4 planetPositions[];
};

layout(std430, binding = 2) readonly buffer PlanetColors {
    uint planetColors[];
};

layout(std430, binding = 3) buffer Particles {
    Particle particles[];
};

uniform int width;
uniform int height;
uniform int planetCount;
uniform int stepsPerDispatch;
uniform float timeStep;
uniform float gravity;
uniform float friction;
uniform float softeningSquared;
uniform float captureEnergy;
uniform float captureDistance;

const uint PARTICLE_ACTIVE = 1u;
const uint PARTICLE_CAPTURED = 2u;

void main() {
    uvec2 pixel = gl_GlobalInvocationID.xy;
    if (pixel.x >= uint(width) || pixel.y >= uint(height)) {
        return;
    }

    uint index = pixel.x + pixel.y * uint(width);
    Particle particle = particles[index];
    if (particle.state != PARTICLE_ACTIVE) {
        return;
    }

    vec2 position = particle.positionVelocity.xy;
    vec2 velocity = particle.positionVelocity.zw;

    for (int step = 0; step < stepsPerDispatch; step++) {
        vec2 acceleration = vec2(0.0);
        for (int planet = 0; planet < planetCount; planet++) {
            vec2 delta = planetPositions[planet].xy - position;
            float radiusSquared = dot(delta, delta) + softeningSquared;
            float inverseRadius = inversesqrt(radiusSquared);
            float inverseRadiusCubed = inverseRadius * inverseRadius * inverseRadius;
            acceleration += delta * inverseRadiusCubed;
        }

        acceleration *= gravity;
        velocity += (acceleration - friction * velocity) * timeStep;
        position += velocity * timeStep;
    }

    particle.positionVelocity = vec4(position, velocity);

    float kineticEnergy = 0.5 * dot(velocity, velocity);
    if (kineticEnergy <= captureEnergy) {
        for (int planet = 0; planet < planetCount; planet++) {
            vec2 difference = abs(position - planetPositions[planet].xy);
            if (difference.x <= captureDistance && difference.y <= captureDistance) {
                particle.state = PARTICLE_CAPTURED;
                particle.color = planetColors[planet];
                break;
            }
        }
    }

    particles[index] = particle;
}
