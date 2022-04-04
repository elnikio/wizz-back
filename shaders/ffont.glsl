#version 430

in vec2 texCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float alpha;

void main (void) {
	vec4 continuous;
	vec4 sampled = vec4 (1.0, 1.0, 1.0, texture (text, texCoords).r);
	continuous = vec4 (textColor, alpha) * sampled;
	if (continuous.w > 0.32)
		color = vec4(textColor, 1.0);
	else
		color = vec4(0.0, 0.0, 0.0, 0.0);
}
