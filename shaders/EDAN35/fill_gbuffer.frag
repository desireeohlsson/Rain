#version 410

uniform bool has_diffuse_texture;
uniform bool has_specular_texture;
uniform bool has_normals_texture;
uniform bool has_opacity_texture;
uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D normals_texture;
uniform sampler2D opacity_texture;
uniform mat4 normal_model_to_world;
uniform bool is_floor;

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
	if (has_opacity_texture && texture(opacity_texture, fs_in.texcoord).r < 1.0)
		discard;

	// Diffuse color
	geometry_diffuse = vec4(0.0f);
	if (has_diffuse_texture)
		geometry_diffuse = texture(diffuse_texture, fs_in.texcoord);

	float scale1 = 0.01;
	float scale2 = 0.013;
	float wetness = max(0, cos(scale1 * fs_in.plane_loc.x) *
		cos(scale2 * fs_in.plane_loc.y)) * float(is_floor);
	
	// Specular color
	if (is_floor) {
		geometry_specular = vec4(1, 1, 1, wetness);
	} else if (has_specular_texture) {
		geometry_specular = texture(specular_texture, fs_in.texcoord);
	} else {
		geometry_specular = vec4(0.0f);
	}

	// Worldspace normal
	geometry_normal = 2 * texture(normals_texture, fs_in.texcoord) - 1;
	
	vec4 t = vec4(normalize(fs_in.tangent), 0.0f);
	vec4 b = vec4(normalize(fs_in.binormal), 0.0f);
	vec4 n = vec4(normalize(fs_in.normal), 0.0f);
	mat4 tbn = mat4(t, b, n, vec4(0.0f, 0.0f, 0.0f, 0.0f));

	geometry_normal = (normalize(tbn * geometry_normal) + 1) / 2;
	//if (wetness > 0.5)
	//	geometry_normal = vec4(fs_in.normal, 0.0);

	//geometry_normal = (normal_model_to_world * vec4(normalize(fs_in.normal), 0.0) + 1) / 2;
	//geometry_diffuse =geometry_normal;
	
}
