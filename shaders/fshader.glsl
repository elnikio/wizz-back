#version 430

out vec4 color;
in vec4 varyingColor;

uniform mat4 mvp_matrix;

void main (void) {
	//color = vec4 (1.0, 0.0, 0.0, 1.0);
	color = varyingColor;
}
