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
uniform float outlineR;
uniform float outlineG;
uniform float outlineB;

in vec4 varyingColor;
in vec2 texCoord;

void main (void) {
//	color = color * texture(samp, texCoord) * vec4 (1.0,1.0,1.0,1.0);
	if (gl_FragCoord.x < (x0win + 1))
		color = vec4 (outlineR, outlineG, outlineB, 1.0);
	if (gl_FragCoord.x > (x1win - 1))
		color = vec4 (outlineR, outlineG, outlineB, 1.0);
	if (gl_FragCoord.y < (y0win + 1))
		color = vec4 (outlineR, outlineG, outlineB, 1.0);
	if (gl_FragCoord.y > (y1win - 1))
		color = vec4 (outlineR, outlineG, outlineB, 1.0);
	//color = varyingColor * texture(samp, texCoord);
//	color = vec4 (colorR, colorG, colorB, 1.0) * texture (samp, texCoord);
//	color = vec4 (1.0, 1.0, 1.0, 1.0) * texture (samp, texCoord);
//	color = vec4 (texture (samp, texCoord).x * colorR, texture(samp,texCoord).y*colorG,texture(samp,texCoord).z*colorB,1.0);

	//>> Temporarily set to grayscale:
	//color = colorR * texture (samp, texCoord);
	vec4 tex_color = texture(samp, texCoord);
	color = vec4 (tex_color.r, tex_color.r, tex_color.r, 1.0);
}
