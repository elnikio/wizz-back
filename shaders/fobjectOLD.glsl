#version 430

out vec4 color;
in vec2 texCoord2;
in vec4 varyingColor;
layout(binding = 0) uniform sampler2D samp;

uniform mat4 mvp_matrix;

void main (void) {
	//color = vec4 (1.0, 0.0, 0.0, 1.0);
	color = varyingColor;
	color = texture (samp, texCoord2);
}
