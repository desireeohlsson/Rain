#version 410

in VS_OUT {
	vec3 vertex;
	vec3 normal;
	vec3 binormal;
	vec3 tangent;
	vec2 texcoord;
} fs_in;

layout (location = 0) out vec4 geometry_diffuse;
layout (location = 1) out vec4 geometry_specular;
layout (location = 2) out vec4 geometry_normal;

void main()
{
	
	geometry_diffuse = vec4(0.8, 0.8, 1.0, 0.4);
	geometry_specular = vec4(1.0, 1.0, 1.0, 1.0);
	geometry_normal = vec4(fs_in.normal.xyz, 0.5);

}
