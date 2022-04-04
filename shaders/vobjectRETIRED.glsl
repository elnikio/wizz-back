#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(binding = 0) uniform sampler2D samp;
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

out vec4 varyingColor;
out vec2 texCoord2;

struct DirLight
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 direction;
};

struct Material
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float glossiness;
};

uniform DirLight dirLight;
uniform Material material;
uniform mat4 normMatrix;
uniform vec4 globalAmbient;

void main(void) {

	// compute diffuse light
	vec4 P = mvMatrix * vec4(position, 1.0);
	//vec3 N = normalize((normMatrix * vec4 (normal, 1.0)).xyz);	// CHECKS OUT
	vec3 N = normalize((normMatrix * vec4 (normal, 1.0)).xyz);	// CHECKS OUT
	vec3 L = normalize(100*(-dirLight.direction.xyz) - P.xyz);
	vec3 V = normalize(-(mvMatrix * vec4(position, 1.0)).xyz);
	vec3 R = reflect(L, N);
	vec3 ambient = ((globalAmbient * material.ambient) + (dirLight.ambient * material.ambient)).xyz;
	vec3 diffuse = dirLight.diffuse.xyz * material.diffuse.xyz * max(dot(N, L), 0.0);
	//vec3 specular = material.specular.xyz * dirLight.specular.xyz * pow(max(dot(R, V), 0.0), material.glossiness);
	//vec3 specular = dirLight.specular.xyz * pow(max(dot(R, V), 0.0), material.glossiness);
	vec3 specular = dirLight.specular.xyz * pow(max(dot(R, V), 0.0), 50.0);

	gl_Position = mvpMatrix * vec4 (position, 1.0);
	varyingColor = vec4 (position, 1.0) * 0.5 + vec4 (0.5, 0.5, 0.5, 0.5);
	texCoord2 = texCoord;
	varyingColor = vec4 ((ambient + diffuse + specular), 1.0);
	//varyingColor = vec4 (normal, 1.0);
}
