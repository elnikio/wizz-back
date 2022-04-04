#version 430

layout (location = 0) in vec3 position;

attribute vec4 InVertex;

out vec4 varyingColor;

uniform float x0;
uniform float x1;
uniform float y0;
uniform float y1;

uniform float x0win;
uniform float x1win;
uniform float y0win;
uniform float y1win;

uniform int outline;

void main(void) {

	float scrWidth = 1920;
	float scrHeight = 1080;
	if (gl_VertexID == 0) {
 		gl_Position = vec4( x0, y0, 0.0, 1.0);
	}
	else if (gl_VertexID == 1) {
	 	gl_Position = vec4( x1, y1, 0.0, 1.0);
	}
	if (outline == 0)
		varyingColor = vec4 (1.0, 1.0, 1.0, 1.0);
	else
		varyingColor = vec4 (1.0, 0.0, 0.0, 1.0);
}
