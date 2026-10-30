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



// Pour chaque canal (R, G, B, A), on applique un décalage à droite (>>) 
// suivi d'un masque logique (& 0xffu) où 0xff = 255 (soit 8 bits) :
// - R : color & 0xffu                  (bits 0-7)
// - G : (color >> 8u) & 0xffu          (bits 8-15)
// - B : (color >> 16u) & 0xffu         (bits 16-23)
// - A : (color >> 24u) & 0xffu         (bits 24-31)
// La division finale par 255.0 normalise les valeurs dans [0.0, 1.0].
vec4 unpackColor(uint color) {
    // Convertit la couleur empaquetée en composantes RGBA normalisées.
    return vec4(
        float(color & 0xffu),
        float((color >> 8u) & 0xffu),
        float((color >> 16u) & 0xffu),
        float((color >> 24u) & 0xffu)
    ) / 255.0;
}


void main() {
    uvec2 dimensions = uvec2(resolution);
    // Convertit l'origine OpenGL (bas-gauche) en origine de la grille (haut-gauche).
    uvec2 pixel = uvec2(gl_FragCoord.xy);
    pixel.y = dimensions.y - 1u - pixel.y;
    uint index = pixel.x + pixel.y * dimensions.x;
    // Affiche la couleur de capture, ou du noir si la particule reste active.
    finalColor = particles[index].state == 2u
            ? unpackColor(particles[index].color)
            : vec4(0.0, 0.0, 0.0, 1.0);
}
