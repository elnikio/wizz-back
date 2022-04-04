#version 430

layout(location = 0) in vec4 position;
out vec2 texCoords;

uniform float xpos;
uniform float ypos;
uniform float w;
uniform float h;

void main(void) {
	float xposP = (xpos / 960) - 1;
	float yposP = (ypos / 540) - 1;
	float wP = w / 960;
	float hP = h / 540;
	
	texCoords = vec2 (0.0, 0.0);
	if (gl_VertexID == 0)
		gl_Position =	vec4 (xposP,      yposP + hP, 0.0, 1.0);
	else if (gl_VertexID == 1) {
		gl_Position = 	vec4 (xposP,      yposP,      0.0, 1.0);
		texCoords.y = 1.0;
	}
	else if (gl_VertexID == 2) {
		gl_Position = 	vec4 (xposP + wP, yposP,      0.0, 1.0);
		texCoords.x = 1.0;
		texCoords.y = 1.0;
	}
	else if (gl_VertexID == 3)
		gl_Position = 	vec4 (xposP,      yposP + hP, 0.0, 1.0);
	else if (gl_VertexID == 4) {
		gl_Position = 	vec4 (xposP + wP, yposP,      0.0, 1.0);
		texCoords.x = 1.0;
		texCoords.y = 1.0;
	}
	else if (gl_VertexID == 5) {
		gl_Position = 	vec4 (xposP + wP, yposP + hP, 0.0, 1.0);
		texCoords.x = 1.0;
	}
}
