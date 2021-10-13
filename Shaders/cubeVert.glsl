#version 430 

layout (location=0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 varyingColor;
out vec4 pos;

void main(void) {
    pos = proj_matrix * mv_matrix * vec4(position, 1.0);
    gl_Position = pos;

    varyingColor = vec4(position, 1.0); // * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}
