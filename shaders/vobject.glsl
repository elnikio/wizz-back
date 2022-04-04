#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(binding = 0) uniform sampler2D samp;
layout(binding = 1) uniform sampler2DShadow shadow_tex;
uniform mat4 m_matrix;	// the model transformation.
uniform mat4 p_matrix;	// the projection of the camera.
uniform mat4 v_matrix;	// the transformation of the entire scene in the direction of the camera.
uniform mat4 sun_v_matrix;	// the transformation of the entire scene in the direction of light 0.
uniform mat4 sun_p_matrix;	// the transformation of the entire scene in the direction of light 0.

out vec4 varyingColor;
out vec2 texCoord2;
out vec4 shadow_coord;
out vec3 normal_interpolated;

struct Point_Light
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 direction;
	vec4 pos;
	int enabled;
};

struct Dir_Light
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 direction;
	int enabled;
	int shadow_mapped;
};

struct Material
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float glossiness;
	float roughness;
	float illumination;
};

uniform Point_Light point_light;
uniform Dir_Light dir_light;
uniform Material material;
uniform mat4 norm_matrix;
uniform vec3 camRotV;
uniform vec3 camPosV;

uniform mat4 modelMatrix;

void main(void) {

	// apply model transformations:
	mat4 mvp_matrix = p_matrix * v_matrix * m_matrix;
	//mat4 sun_mvp_matrix = sun_p_matrix * sun_v_matrix * m_matrix;
	mat4 sun_mvp_matrix = sun_p_matrix * sun_v_matrix * m_matrix;

	vec3 ambient = vec3 (0.5, 0.5, 0.5);
	ambient = material.diffuse.xyz;
	vec3 diffuse = vec3 (0.5, 0.5, 0.5);
	diffuse = material.diffuse.xyz;
	vec3 glow = vec3 (0.0, 0.0, 0.0);
	vec3 specular = vec3 (0.0, 0.0, 0.0);

	normal_interpolated = normal;

	// compute point light
	if (point_light.enabled == 1) {
		vec3 N = normal;
		vec3 L = normalize(-point_light.pos.xyz);
		vec3 V = normalize(camPosV - (m_matrix * vec4(position, 1.0)).xyz);
		//vec3 V = normalize(camPosV - position);
		vec3 R = reflect(L, N);
		ambient = (point_light.ambient * material.ambient).xyz;
		diffuse = point_light.diffuse.xyz * material.diffuse.xyz * max(dot(N, L), 0.0);
		glow = material.diffuse.xyz;
		specular = point_light.specular.xyz * pow(max(dot(R, V), 0.0), material.glossiness);
	}

	// compute dir light
	if (dir_light.enabled == 1) {
		vec3 N = normal;
		vec3 L = normalize(dir_light.direction.xyz);
		//vec3 L = normalize(-camPosV);
		vec3 V = normalize(camPosV - (m_matrix * vec4(position, 1.0)).xyz);
		//vec3 V = normalize(camPosV - position);
		vec3 R = reflect(L, N);
		ambient = (dir_light.ambient * material.ambient).xyz;
		diffuse = dir_light.diffuse.xyz * material.diffuse.xyz * max(dot(N, L), 0.0);
		glow = material.diffuse.xyz;
		specular = dir_light.specular.xyz * pow(max(dot(R, V), 0.0), material.glossiness);
	}

	gl_Position = mvp_matrix * modelMatrix * vec4 (position, 1.0);
	texCoord2 = texCoord;
	varyingColor = vec4 (((1.0 - material.illumination) * diffuse + material.illumination * glow + ambient + (1 - material.roughness) * specular), 1.0);
	if (dir_light.shadow_mapped == 1) {
		shadow_coord = sun_mvp_matrix * vec4 (position, 1.0);
	}
}
