#version 430

out vec4 color;
layout (binding = 0) uniform sampler2D samp;

uniform float x0win;
uniform float x1win;
uniform float y0win;
uniform float y1win;

uniform int chapter;
uniform int background;

uniform float colorR;
uniform float colorG;
uniform float colorB;
uniform float outlineR;
uniform float outlineG;
uniform float outlineB;

in vec4 varyingColor;
in vec2 texCoord;
uniform float time;

#define FOREST_R 0
#define FOREST_G 100
#define FOREST_B 0
#define FOREST_BG_R 0
#define FOREST_BG_G 8
#define FOREST_BG_B 0


#define TITLE 0
#define FOREST 1

void main (void) {
	vec4 tex_color = texture(samp, texCoord);
	color = tex_color;
	//color = vec4 (1.0, 0.0, 0.0, 0.5);
}
