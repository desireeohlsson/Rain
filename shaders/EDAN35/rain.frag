#version 410

uniform mat4 normal_model_to_world;

in VS_OUT {
	vec3 normal;
	vec2 texcoord;
	vec3 tangent;
	vec3 binormal;
	vec2 plane_loc;
} fs_in;


layout (location = 0) out vec4 geometry_diffuse;
layout (location = 1) out vec4 geometry_specular;
layout (location = 2) out vec4 geometry_normal;

void main()
{
	float magic = pow(abs(fs_in.normal.z) * fs_in.texcoord.y, 0.5);
	// Diffuse color
	geometry_diffuse = vec4(1.0f, 1.0f, 1.0f, magic);
	
	// Specular color
	geometry_specular =  vec4(0.3f, 0.3f, 0.3f, 0.5f);

	// Worldspace normal
	
	vec4 t = vec4(normalize(fs_in.tangent), 0.0f);
	vec4 b = vec4(normalize(fs_in.binormal), 0.0f);
	vec4 n = vec4(normalize(fs_in.normal), 0.0f);
	mat4 tbn = mat4(t, b, n, vec4(0.0f, 0.0f, 0.0f, 0.0f));

	geometry_normal = (normalize(tbn * geometry_normal) + 1) / 2;
	geometry_normal.w = 0.5;
	
}
