#version 430

out vec4 finalColor;

struct Particle {
    vec4 positionVelocity;
    uint state;
    uint color;
    uint reserved0;
    uint reserved1;
};

layout(std430, binding = 3) readonly buffer Particles {
    Particle particles[];
};

uniform vec2 resolution;

vec4 unpackColor(uint color) {
    return vec4(
        float(color & 0xffu),
        float((color >> 8u) & 0xffu),
        float((color >> 16u) & 0xffu),
        float((color >> 24u) & 0xffu)
    ) / 255.0;
}

void main() {
    uvec2 dimensions = uvec2(resolution);
    // gl_FragCoord is independent of raylib's batched texture coordinates.
    // OpenGL starts at the bottom-left; the simulation grid starts at the top-left.
    uvec2 pixel = uvec2(gl_FragCoord.xy);
    pixel.y = dimensions.y - 1u - pixel.y;
    uint index = pixel.x + pixel.y * dimensions.x;
    finalColor = particles[index].state == 2u
            ? unpackColor(particles[index].color)
            : vec4(0.0, 0.0, 0.0, 1.0);
}
