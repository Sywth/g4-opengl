#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main() {
    // if (uModel[3][2] == 1) {
    //     return vec4(aPos, 1.0);
    // } else {
    //     return vec4(0.0, 0.0, 0.0, 1.0);
    // }

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
