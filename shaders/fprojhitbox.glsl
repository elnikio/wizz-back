
#version 430
#define MAX_ENTITIES 100
uniform int entity_id;
uniform int entity_max;
out vec4 color;

void main (void) {
	gl_FragDepth = 1.0 * entity_id / MAX_ENTITIES;	// 10000 is the built-in max entity count. this must be known here, and also in the source program so it can be multiplied by it.
	// gl_FragDepth = 1.0 * entity_id / entity_max;
	color = vec4(0.6, 0.6, 0.6, 1.0);
	color.x = entity_id;	// 10000 is the built-in max entity count. this must be known here, and also in the source program so it can be multiplied by it.
}
