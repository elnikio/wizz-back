#version 430

uniform mat4 m_matrix;	// the model transformation.
uniform mat4 p_matrix;	// the projection of the camera.
uniform mat4 v_matrix;	// the transformation of the entire scene in the direction of the camera.

uniform mat4 modelMatrix;

// meta:
#define BOX 1
#define TRI 2
#define SLOPE 3
#define RAY 4
uniform int type;

// box:
uniform float left;
uniform float right;
uniform float roof;
uniform float floor;

// tri:
uniform vec2 p0;
uniform vec2 p1;
uniform vec2 p2;

// slope:
uniform float slope;
uniform float start;
uniform float end;

void main(void) {
	mat4 mvp_matrix = p_matrix * v_matrix;

	if (type == BOX) {
		if (gl_VertexID == 0) gl_Position = mvp_matrix * vec4 (left, floor, 0.0, 1.0);
		else if (gl_VertexID == 1) gl_Position = mvp_matrix * vec4 (right, roof, 0.0, 1.0);
		else if (gl_VertexID == 2) gl_Position = mvp_matrix * vec4 (right, floor, 0.0, 1.0);

		else if (gl_VertexID == 3) gl_Position = mvp_matrix * vec4 (left, floor, 0.0, 1.0);
		else if (gl_VertexID == 4) gl_Position = mvp_matrix * vec4 (left, roof, 0.0, 1.0);
		else if (gl_VertexID == 5) gl_Position = mvp_matrix * vec4 (right, roof, 0.0, 1.0);
	}
	
	else if (type == TRI) {
		if (gl_VertexID == 0) gl_Position = mvp_matrix * vec4 (p0.x, p0.y, 0.0, 1.0);
		else if (gl_VertexID == 1) gl_Position = mvp_matrix * vec4 (p1.x, p1.y, 0.0, 1.0);
		else if (gl_VertexID == 2) gl_Position = mvp_matrix * vec4 (p2.x, p2.y, 0.0, 1.0);
	}
	else if (type == RAY) {
		if (gl_VertexID == 0) gl_Position = mvp_matrix * vec4 (p0, 0.0, 1.0);
		else if (gl_VertexID == 1) gl_Position = mvp_matrix * vec4 (p1, 0.0, 1.0);
	}

}
