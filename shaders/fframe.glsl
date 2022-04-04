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

in vec4 varyingColor;
in vec2 texCoord;

float ground (float x) {
	return -pow(log(-x + 1.2), 3);
	return x;
}

float round (float x) {
	return -pow(log(-x/8 + 1.1), 3) * 8;
	//return sin (x * 1.57079);
}

float dist (float x0, float y0, float x1, float y1) {
	return sqrt ((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
}

void main (void) {
	vec4 normal = vec4 (0.5, 0.5, 1.0, 1.0);

	color = vec4 (colorR, colorG, colorB, 1.0);
	float mask = 1.0;

	if (gl_FragCoord.x < x0win + 16)
		normal.x = 0.5 - ground(dist (gl_FragCoord.x, y0win, x0win + 16, y0win) / 16)/2;
	if (gl_FragCoord.x > x1win - 16)
		normal.x = 0.5 + ground(dist (gl_FragCoord.x, y0win, x1win - 16, y0win) / 16)/2;

	if (gl_FragCoord.y < y0win + 16)
		normal.y = 0.5 - ground(dist (x0win, gl_FragCoord.y, x0win, y0win + 16) / 16)/2;
	if (gl_FragCoord.y > y1win - 16)
		normal.y = 0.5 + ground(dist (x0win, gl_FragCoord.y, x0win, y1win - 16) / 16)/2;

	if ((gl_FragCoord.x < x0win + 8) &&
		(gl_FragCoord.y > y1win - 8)) {
		if (dist (gl_FragCoord.x, gl_FragCoord.y, x0win + 8, y1win - 8) > 8) {
			color.w = 0.0;
			normal.w = 0.0;
			mask = 0.0;
		}
	}
	else if ((gl_FragCoord.x < x0win + 8) &&
		(gl_FragCoord.y < y0win + 8)) {
		if (dist (gl_FragCoord.x, gl_FragCoord.y, x0win + 8, y0win + 8) > 8) {
			color.w = 0.0;
			normal.w = 0.0;
			mask = 0.0;
		}
	}
	else if ((gl_FragCoord.x > x1win - 8) &&
		(gl_FragCoord.y < y0win + 8)) {
		if (dist (gl_FragCoord.x, gl_FragCoord.y, x1win - 8, y0win + 8) > 8) {
			color.w = 0.0;
			normal.w = 0.0;
			mask = 0.0;
		}
	}
	else if ((gl_FragCoord.x > x1win - 8) &&
		(gl_FragCoord.y > y1win - 8)) {
		if (dist (gl_FragCoord.x, gl_FragCoord.y, x1win - 8, y1win - 8) > 8) {
			color.w = 0.0;
			normal.w = 0.0;
			mask = 0.0;
		}
	}
	color = normal;
	float lum = abs(dot(normalize(sunDir), normalize(normal)));
	float lum2 = abs(dot(normalize(vec4(-2.2, -6.4, 2.9, 0.0)), normalize(normal)));
	float lum3 = dot(normalize(vec3(0.0, 0.0, 1.0)), normalize(normal.xyz));
	vec3 rgb;
	// Orange Reflection:
	if (pow(lum, 2) < 0.4) {
		rgb = vec3 (colorR * 0.85, colorG * 0.85, colorB * 0.85);
		//rgb = (texture(samp, texCoord) * vec4(rgb, 1.0)).xyz;
	}
	else {
		rgb = vec3 (colorR, colorG, colorB);
		//rgb = (texture(samp, texCoord) * 4.0 * vec4(rgb, 1.0)).xyz;
	}
	// White Highlight:
	if (pow(lum2, 2) > 0.4) {
		rgb = vec3 (1.00, 1.00, 1.00);
	}
	if (pow(lum3, 2) < 0.42 && pow(lum3, 2) > 0.00) {
		rgb = vec3 (colorR, colorG, colorB);
	}
	color = vec4 (rgb.x * lum*2, rgb.y * lum*2, rgb.z * lum*2, mask);
}
