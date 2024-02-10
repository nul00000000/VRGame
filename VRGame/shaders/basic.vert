#version 400 core

in vec3 position;
in vec3 color;
in vec3 normal;

out vec3 colorPass;
out vec3 pNormal;
out vec3 fragPos;

uniform mat4 transform;
uniform mat4 camera;
uniform mat4 projection;

void main(void) {
    gl_Position = projection * camera * transform * vec4(position, 1.0);
    colorPass = color;
    fragPos = vec3(transform * vec4(position, 1.0));
    pNormal = vec3(transform * vec4(normal, 1.0)) - transform[3].xyz;
}