#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
	vec3 texcoord;
} vs_out;


void main()
{
	vs_out.texcoord = normal;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}
