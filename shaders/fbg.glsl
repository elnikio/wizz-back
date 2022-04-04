#version 430

out vec4 color;
layout (binding = 0) uniform sampler2D samp;

uniform float x0win;
uniform float x1win;
uniform float y0win;
uniform float y1win;

uniform float colorR;
uniform float colorG;
uniform float colorB;

uniform vec4 sunDir;

in vec2 texCoord;

void main (void) {
	vec4 rgb = texture(samp, texCoord);
	//color = vec4 (rgb.x/8, rgb.y/16, rgb.z, 1.0);
	color = vec4 (rgb.x, rgb.y, rgb.z, 1.0);
}
