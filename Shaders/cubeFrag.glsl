#version 430

in vec4 varyingColor;
in vec4 pos;

out vec4 color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

float circleShape(vec2 pos, float radius, vec2 off) {
    return step(radius, length(pos - off));
}

void main(void) {
    vec2 uv = gl_FragCoord.xy / vec2(600, 600);
    
    float circle = circleShape(pos.xy, 0.3, vec2(0.5, 0.5));
    vec4 col = vec4(circle, circle, circle, 1.0);

    color = col;
}
