#version 430

out vec4 color;
layout (binding = 0) uniform sampler2D sampMask;
layout (binding = 1) uniform sampler2D sampR;
layout (binding = 2) uniform sampler2D sampG;
layout (binding = 3) uniform sampler2D sampB;

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
uniform float realtime;
uniform float animtime;

#define FOREST_R 0
#define FOREST_G 100
#define FOREST_B 0
#define FOREST_BG_R 0
#define FOREST_BG_G 8
#define FOREST_BG_B 0


#define TITLE 0
#define FOREST 1

void main (void) {
	vec3 sprite_color = vec3 (1.0, 1.0, 1.0);
	vec3 sprite_bg_color;
	vec4 colorMask = texture(sampMask, texCoord);

	vec2 coord;
	//coord.x = texCoord.x + sin(realtime);
	//coord.y = texCoord.y + sin(realtime);
	coord.x = texCoord.x + sin(realtime) / 16;
	coord.y = texCoord.y + sin(realtime * 4) / 24;


	vec4 colorR = texture(sampR, coord);
	vec4 colorG = texture(sampG, coord);
	vec4 colorB = texture(sampB, coord);

	/*
	if (chapter == TITLE) {
		float r = sin(time * 100);
		float g = sin(time * 1000);
		float b = 1.0 - (r + g);
		sprite_color = vec3 (r, g, b);
		sprite_bg_color = vec3 (8.0 / 255, 8.0 / 255, 8.0 / 255);
	}

	if (chapter == FOREST) {
		//sprite_color = vec3 (float(FOREST_R) / 255, float(FOREST_G) / 255 + sin(time * 400) / 32, float(FOREST_B) / 255);
		sprite_color = vec3 (float(FOREST_R) / 255, float(FOREST_G) / 255, float(FOREST_B) / 255);
		sprite_bg_color = vec3 (float(FOREST_BG_R) / 255, float(FOREST_BG_G) / 255, float(FOREST_BG_B) / 255);
	}

	color = vec4 (
		sprite_color.x * (tex_color.x) + sprite_bg_color.x * (1 - tex_color.x),
		sprite_color.y * (tex_color.y) + sprite_bg_color.y * (1 - tex_color.y),
		sprite_color.z * (tex_color.z) + sprite_bg_color.z * (1 - tex_color.z),
		tex_color.w
	);
	*/

	/*
	if (background == 1)
		color.w /= 2;
	*/
	
	color = vec4 (
		colorMask.xyz,
		colorMask.w
	);

	if (colorMask.r > 0)
		color = colorR;
	if (colorMask.g > 0)
		color = colorG;
	if (colorMask.b > 0)
		color = colorB;
	if (colorMask.rgb == vec3(1.0, 1.0, 1.0)) {
		color = colorR / 4;
	}
	color.w = colorMask.w;

	color.g *= 1.5;
}
