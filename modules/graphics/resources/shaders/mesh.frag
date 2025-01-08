#version 430

in vec2 uv;
in vec4 colorOut;

out vec4 color;

void main () {
    color = colorOut;
    //color = vec4(vec3(gl_FragCoord.z), 1.0);
}