#version 430

layout (location = 0) in vec3 position;

out vec2 texCoord;

attribute vec4 InVertex;

uniform float x0;
uniform float x1;
uniform float y0;
uniform float y1;

uniform float x0win;
uniform float x1win;
uniform float y0win;
uniform float y1win;

void main(void) {

	if (gl_VertexID == 0) {
 		gl_Position = vec4( x0, y1, 0.0, 1.0);
		texCoord = vec2(0,1);
	}
	else if (gl_VertexID == 1) {
	 	gl_Position = vec4( x0, y0, 0.0, 1.0);
		texCoord = vec2(0,0);
	}
	else if (gl_VertexID == 2) {
	 	gl_Position = vec4( x1, y1, 0.0, 1.0);
		texCoord = vec2(1,1);
	}
	else if (gl_VertexID == 3) {
	 	gl_Position = vec4( x1, y0, 0.0, 1.0);
		texCoord = vec2(1,0);
	}
	else if (gl_VertexID == 4) {
	 	gl_Position = vec4( x1, y1, 0.0, 1.0);
		texCoord = vec2(1,1);
	}
	else {
		gl_Position = vec4( x0, y0, 0.0, 1.0);
		texCoord = vec2(0,0);
	}
}
