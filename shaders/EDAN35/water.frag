#version 410

uniform vec3 light_position;
uniform vec3 camera_position;
uniform samplerCube skybox_texture;
uniform sampler2D normal_texture;
uniform float ellapsed_time_s;

in VS_OUT {
	vec3 vertex;
	vec3 normal;
	vec3 binormal;
	vec3 tangent;
	vec2 texcoord;
} fs_in;

//out vec4 frag_color;

layout (location = 0) out vec4 geometry_diffuse;
layout (location = 1) out vec4 geometry_specular;
layout (location = 2) out vec4 geometry_normal;

void main()
{
	
	geometry_diffuse = vec4(0.8, 0.8, 1.0, 0.4);
	geometry_specular = vec4(1.0, 1.0, 1.0, 1.0);
	
	/**
	geometry_normal = 2 * texture(normal_texture, fs_in.texcoord) - 1;
	
	vec4 t = vec4(normalize(fs_in.tangent), 0.0f);
	vec4 b = vec4(normalize(fs_in.binormal), 0.0f);
	vec4 n = vec4(normalize(fs_in.normal), 0.0f);
	mat4 tbn = mat4(t, b, n, vec4(0.0f, 0.0f, 0.0f, 0.0f));

	geometry_normal = (normalize(tbn * geometry_normal) + 1) / 2;
	*/
	geometry_normal = vec4(fs_in.normal.xyz, 0.5);
	return;
	/**
	vec2 tex_scale = vec2(8, 4);
	float normal_time = mod(ellapsed_time_s, 100.0);
	vec2 normal_speed = vec2(-0.05, 0.0);
	vec3 n = vec3(0.0);
	vec2 normcoord;
	for(int i = 0; i < 3; i++) {
		normcoord = fs_in.texcoord * tex_scale * pow(2, i) 
			+ normal_time * normal_speed * (i > 0 ? pow(2, i + 1) : 1.0);
		n += texture(normal_texture, normcoord).xyz * 2 -1;
	}
	n = normalize(n);
	mat3 tangent_to_object = mat3(fs_in.tangent, fs_in.binormal, fs_in.normal);
	vec3 object_normal = normalize(tangent_to_object * (2 * n - 1));
	
	vec4 color_deep = vec4(0.0, 0.0, 0.1, 1.0);
	vec4 color_shallow = vec4(0.0, 0.5, 0.5, 1.0);
	
	vec3 v = normalize(camera_position - fs_in.vertex);
	vec3 L = normalize(light_position - fs_in.vertex);
	float facing = 1 - max(dot(v, object_normal), 0);
	vec4 water_color = mix(color_deep, color_shallow, facing);
	vec4 R = texture(skybox_texture, reflect(-v, object_normal));
	
	float R0 = 0.02037;
	float fast_fresnel = R0 + (1 - R0) * pow((1 - dot(v, object_normal)), 5);
	
	float n_air2wat = 1.33;
	float n_wat2air = 1.0 / 1.33;
	vec4 refraction = texture(skybox_texture, refract(-v, object_normal, n_wat2air));
	
	geometry_diffuse = water_color
				+ R * fast_fresnel
				+ refraction * (1 - fast_fresnel);
	
	
	//frag_color = refraction * (1 - fast_fresnel);
	*/
}
