#version 430

layout (location = 0) in vec3 position;

out vec4 varyingColor;
out vec2 texCoord;
out vec3 pos;

attribute vec4 InVertex;

uniform float x0;
uniform float x1;
uniform float y0;
uniform float y1;
uniform float time;
uniform float realtime;
uniform float rewind_time;
uniform int background;

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

/*

	if (gl_VertexID == 0) {
 		gl_Position = vec4( x0, y1, 0.0, 1.0);
		if (background == 1)
		gl_Position.x -= sin(realtime*4) / 1024;
		texCoord = vec2(0,1);
	}
	else if (gl_VertexID == 1) {
		gl_Position = vec4( x0, y0, 0.0, 1.0);
		if (background == 1)
		gl_Position.x += sin(realtime*4) / 1024;
		texCoord = vec2(0,0);
	}
	else if (gl_VertexID == 2) {
	 	gl_Position = vec4( x1, y1, 0.0, 1.0);
		if (background == 1)
		gl_Position.x -= sin(realtime*4) / 1024;
		texCoord = vec2(1,1);
	}
	else if (gl_VertexID == 3) {
		gl_Position = vec4( x1, y0, 0.0, 1.0);
		if (background == 1)
		gl_Position.x += sin(realtime*4) / 1024;
		texCoord = vec2(1,0);
	}
	else if (gl_VertexID == 4) {
	 	gl_Position = vec4( x1, y1, 0.0, 1.0);
		if (background == 1)
		gl_Position.x -= sin(realtime*4) / 1024;
		texCoord = vec2(1,1);
	}
	else {
		gl_Position = vec4( x0, y0, 0.0, 1.0);
		if (background == 1)
		gl_Position.x += sin(realtime*4) / 1024;
		texCoord = vec2(0,0);
	}
*/

	float factor = sqrt(pow(gl_Position.x, 2) + pow(gl_Position.y / 2, 2));
	float factorX = pow(gl_Position.x, 2);
	float factorY = pow(gl_Position.y / 2, 2);
	float e = 2.71828;

/*
	float rewind_x = gl_Position.x + sin(factor*32 - time * 10000) / 4;
	float rewind_y = gl_Position.y + sin(factor*32 - time * 10000) / 4;	

*/
	float rewind_x = gl_Position.x + sin(factorX*128 - time * 2000) / 4;
	float rewind_y = gl_Position.y + cos(factorY*128 - time * 2000) / 4;

	gl_Position.x = mix (gl_Position.x, rewind_x, pow(e, -pow((rewind_time*6 - 3), 2)));
	gl_Position.y = mix (gl_Position.y, rewind_y, pow(e, -pow((rewind_time*6 - 3), 2)));

	if (background == 1)
		gl_Position.z = 0.5;
	varyingColor = vec4(1.0, 1.0, 1.0, 1.0);

	//gl_Position.y *= (1 - (pow(gl_Position.x, 2) / 12));
	//gl_Position.x *= (1 - (pow(gl_Position.y, 2) / 4));
	//gl_Position.x *= (1 - (gl_Position.y / 2));

/*
	gl_Position.x *= (1 - 0.05 * (sin(time * 100 + gl_Position.x * 10) + 1));
	gl_Position.y *= (1 - 0.05 * (sin(time * 100 + gl_Position.y * 10) + 1));
*/

 	pos = gl_Position.xyz;
}
