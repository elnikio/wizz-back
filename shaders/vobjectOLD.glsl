#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(binding = 0) uniform sampler2D samp;

uniform mat4 mvp_matrix;

out vec4 varyingColor;
out vec2 texCoord2;

void main(void) {
	gl_Position = mvp_matrix * vec4 (position, 1.0);
	varyingColor = vec4 (position, 1.0) * 0.5 + vec4 (0.5, 0.5, 0.5, 0.5);
	texCoord2 = texCoord;
}
