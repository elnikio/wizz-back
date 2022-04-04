#version 430

layout (location = 0) in vec3 position;

uniform mat4 mvp_matrix;

out vec4 varyingColor;

void main(void) {
	gl_Position = mvp_matrix * vec4 (position, 1.0);
	varyingColor = vec4 (position, 1.0) * 0.5 + vec4 (0.5, 0.5, 0.5, 0.5);
}
