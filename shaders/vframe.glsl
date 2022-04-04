#version 430

layout (location = 0) in vec3 position;

out vec4 varyingColor;
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

	float scrWidth = 1920;
	float scrHeight = 1080;
	if (gl_VertexID == 0) {
 		gl_Position = vec4( x0, y1, 0.0, 1.0);
		texCoord = vec2(0,1);
		texCoord.x += (x0win / scrWidth);
		texCoord.y -= ((scrHeight - y1win) / scrHeight);
	}
	else if (gl_VertexID == 1) {
	 	gl_Position = vec4( x0, y0, 0.0, 1.0);
		texCoord = vec2(0,0);
		texCoord.x += (x0win / scrWidth);
		texCoord.y += (y0win / scrHeight);
	}
	else if (gl_VertexID == 2) {
	 	gl_Position = vec4( x1, y1, 0.0, 1.0);
		texCoord = vec2(1,1);
		texCoord.x -= ((scrWidth - x1win) / scrWidth);
		texCoord.y -= ((scrHeight - y1win) / scrHeight);
	}
	else if (gl_VertexID == 3) {
	 	gl_Position = vec4( x1, y0, 0.0, 1.0);
		texCoord = vec2(1,0);
		texCoord.x -= ((scrWidth - x1win) / scrWidth);
		texCoord.y += (y0win / scrHeight);
	}
	else if (gl_VertexID == 4) {
	 	gl_Position = vec4( x1, y1, 0.0, 1.0);
		texCoord = vec2(1,1);
		texCoord.x -= ((scrWidth - x1win) / scrWidth);
		texCoord.y -= ((scrHeight - y1win) / scrHeight);
	}
	else {
		gl_Position = vec4( x0, y0, 0.0, 1.0);
		texCoord = vec2(0,0);
		texCoord.x += (x0win / scrWidth);
		texCoord.y += (y0win / scrHeight);
	}
	
	varyingColor = vec4(position,1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}
