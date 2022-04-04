#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
uniform mat4 m_matrix;	// the model transformation.
uniform mat4 p_matrix;	// the projection of the camera.
uniform mat4 v_matrix;	// the transformation of the entire scene in the direction of the camera.

uniform mat4 norm_matrix;
uniform vec3 camRotV;
uniform vec3 camPosV;
uniform mat4 modelMatrix;
uniform vec3 scale;

void main(void) {

	// apply model transformations:
	mat4 mv_matrix = v_matrix * m_matrix;
	mat4 mvp_matrix = p_matrix * v_matrix * m_matrix;

	vec3 dist = (m_matrix * vec4(position, 1.0)).xyz - camPosV;

	vec3 position_inflated = position + 0.0015 * length(dist) * vec3(normal.x / scale.x, normal.y / scale.y, normal.z / scale.z);
	gl_Position = mvp_matrix * modelMatrix * vec4 (position_inflated, 1.0);
}
