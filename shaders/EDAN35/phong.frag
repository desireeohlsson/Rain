#version 410

uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform vec3 light_position;
uniform vec3 diffuse;
uniform vec3 ambient;
uniform vec3 specular;
uniform vec3 camera_position;
uniform float shininess;
uniform int use_normal_mapping;

in VS_OUT {
	vec3 vertex;
	vec3 normal;
	vec2 texcoord;
	vec3 tangent;
	vec3 binormal;
} fs_in;

out vec4 frag_color;

void main()
{
	
	vec3 l = normalize(light_position - fs_in.vertex);
	vec3 v = normalize(camera_position - fs_in.vertex);
	vec4 tex_colour = texture(diffuse_texture, fs_in.texcoord);
	vec4 spec_colour = texture(specular_texture, fs_in.texcoord);
	vec4 norm_colour = texture(normal_texture, fs_in.texcoord);
	mat3 tangent_to_object = mat3(fs_in.tangent, fs_in.binormal, fs_in.normal);
	vec3 object_normal = normalize(tangent_to_object * (2 * norm_colour.rgb - 1));
	
	if (use_normal_mapping != 0) {
		vec3 r = reflect(-l, object_normal);
		frag_color = 
			vec4(ambient, 1.0) * 0.5 +
			tex_colour * clamp(dot(normalize(object_normal), l), 0.0, 1.0) +
			spec_colour * vec4(specular, 1.0) * pow(clamp(dot(r, v), 0.0, 1.0), shininess);
	} else {
		vec3 r = reflect(-l, fs_in.normal);
		frag_color = 
			vec4(ambient, 1.0) +
			tex_colour * clamp(dot(normalize(fs_in.normal), l), 0.0, 1.0) +
			spec_colour * vec4(specular, 1.0) * pow(clamp(dot(r, v), 0.0, 1.0), shininess);
	}
	
}
