#version 430

out vec4 color;

uniform float x0win;
uniform float x1win;
uniform float y0win;
uniform float y1win;

uniform float colorR;
uniform float colorG;
uniform float colorB;
uniform float outlineR;
uniform float outlineG;
uniform float outlineB;

in vec4 varyingColor;

void main (void) {
	color = vec4 (colorR, colorG, colorB, 1.0);
	if (gl_FragCoord.x < (x0win + 2))
		color = vec4 (1-outlineR, 1-outlineG, 1-outlineB, 1.0);
	if (gl_FragCoord.x > (x1win - 2))
		color = vec4 (outlineR, outlineG, outlineB, 1.0);
	if (gl_FragCoord.y < (y0win + 2))
		color = vec4 (outlineR, outlineG, outlineB, 1.0);
	if (gl_FragCoord.y > (y1win - 2))
		color = vec4 (1-outlineR, 1-outlineG, 1-outlineB, 1.0);
}
