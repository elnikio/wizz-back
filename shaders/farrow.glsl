#version 430

out vec4 color;

in vec4 varyingColor;

uniform float x0win;
uniform float x1win;
uniform float y0win;
uniform float y1win;

void main (void) {
	color = varyingColor;
}
