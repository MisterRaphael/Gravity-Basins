#version 430

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

uniform mat4 mvp;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = vertexTexCoord;
    // Transforme le sommet pour l'afficher à l'écran.
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
