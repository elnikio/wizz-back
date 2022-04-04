#version 430

in vec3 normal_interpolated;

out vec4 color;
in vec2 texCoord2;
in vec4 varyingColor;
in vec4 shadow_coord;
layout(binding = 0) uniform sampler2D samp;
layout(binding = 1) uniform sampler2DShadow shadow_tex;

uniform float curposX;
uniform float curposY;
uniform float hover;
uniform float shadow_bias;

struct DirLight
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

uniform DirLight dirLight;
uniform Material material;
uniform mat4 normMatrix;
uniform vec4 globalAmbient;

float dist2 (vec2 A, vec2 B) {
	float d;
	d = pow((A.x - B.x), 2) + pow((A.y - B.y), 2);
	return d;
}

void circle (vec2 center, float radius, float rim) {
	if (dist2 (gl_FragCoord.xy, center) < (radius * radius)) {
		if (dist2 (gl_FragCoord.xy, center) > ((radius - rim)* (radius - rim))) {
			color = vec4 (1.0, 0.0, 0.0, 1.0);
		}
	}
}

float shadow_mapping2 () {
	vec3 proj_coord = shadow_coord.xyz / shadow_coord.w;
	proj_coord = proj_coord * 0.5 + 0.5;

	float bias = max(shadow_bias*10 * (1.0 - dot(normalize(normal_interpolated), dirLight.direction.xyz)), shadow_bias);
	bias = 0.5;

	float shadow = 0.0;

	vec2 texel_size = 1.0 / textureSize(shadow_tex, 0);




			float closest_depth = texture (shadow_tex, proj_coord.xyz).r;
			float current_depth = proj_coord.z;
	
			shadow += current_depth - bias > closest_depth ? 1.0 : 0.0;

	/*
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float closest_depth = texture (shadow_tex, proj_coord.xyz + vec3 (vec2 (x, y) * texel_size, 0.0)).r;
			float closest_depth = textureProj (shadow_tex, proj_coord.xy + vec2 (x, y) * texel_size).r;
			float current_depth = proj_coord.z;
	
			shadow += current_depth - bias > closest_depth ? 1.0 : 0.0;
		}
	}
	*/
	if (proj_coord.z > 1.0)
		shadow = 0.0;
	return shadow;
}

void main (void) {
	float illuminated = textureProj (shadow_tex, shadow_coord).r;

	color = texture (samp, texCoord2) * varyingColor;
	//circle (vec2 (curposX, curposY), 32, 1);
	if (shadow_mapping2 () == 1.0)
		//color = texture (samp, texCoord2) * vec4(dirLight.diffuse.xyz * material.diffuse.xyz, 1.0);
		color = texture (samp, texCoord2) * vec4(varyingColor.xyz/2, varyingColor.w);
	//>> Temporarily disabled shadows until I learn how to use depth buffers.
}

