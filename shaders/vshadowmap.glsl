#version 430
layout (location = 0) in vec3 position;
uniform mat4 m_matrix;	// model matrix
uniform mat4 p_matrix;	// projection matrix (not needed for directional light)
uniform mat4 v_matrix;	// view-matrix

void main (void) {
	gl_Position = p_matrix * v_matrix * m_matrix * vec4 (position, 1.0);
}

// shadow mapping:
// 1. render depth buffer of complete scene from POV of light [1].
// 2. render depth buffer of object onle [2].
// 3. for every pixel in fragment shader, if corresponding pixel in.

//
