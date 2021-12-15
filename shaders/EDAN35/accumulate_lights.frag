#version 410

struct ViewProjTransforms
{
	mat4 view_projection;
	mat4 view_projection_inverse;
};

layout (std140) uniform CameraViewProjTransforms
{
	ViewProjTransforms camera;
};

layout (std140) uniform LightViewProjTransforms
{
	ViewProjTransforms lights[4];
};

uniform int light_index;

uniform sampler2D depth_texture;
uniform sampler2D normal_texture;
uniform sampler2DShadow shadow_texture;

uniform vec2 inverse_screen_resolution;

uniform vec3 camera_position;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 light_direction;
uniform float light_intensity;
uniform float light_angle_falloff;

layout (location = 0) out vec4 light_diffuse_contribution;
layout (location = 1) out vec4 light_specular_contribution;


void main()
{
	//light_diffuse_contribution  = vec4(0.1, 0.0, 0.0, 1.0);
	//light_specular_contribution = vec4(0.0, 0.0, 0.0, 1.0);

	vec2 textureCoords = vec2(gl_FragCoord.x * inverse_screen_resolution.x, gl_FragCoord.y * inverse_screen_resolution.y);
	float depth = texture(depth_texture, textureCoords).r;
	vec4 projPos = vec4(vec3(textureCoords, depth), 1.0) * 2 - 1; 
	vec4 worldPos = camera.view_projection_inverse * projPos;
	vec3 vertex = worldPos.xyz / worldPos.w;

	vec4 light = vec4(normalize(light_position - vertex), 1.0);
	vec4 normal = texture(normal_texture, textureCoords) * 2 - 1;
	vec3 view = normalize(camera_position - vertex);
	vec3 reflect = normalize(reflect(-light.xyz, normal.xyz));

	float distance = distance(light_position, vertex);
	float distance_falloff = 1.0 / (distance * distance);
	float linear_falloff = clamp(dot(-light.xyz, normalize(light_direction)) - cos(light_angle_falloff), 0.0, 1.0)/(1-cos(light_angle_falloff));
	//float theta = dot(-light.xyz, normalize(light_direction));
	//float linear_falloff = max((acos(theta)-light_angle_falloff)/(-light_angle_falloff), 0.0);

	float intensity = distance_falloff * linear_falloff * light_intensity;

	vec4 shadow_pos = lights[light_index].view_projection * vec4(vertex, 1.0);
	vec3 shadow_vert = (shadow_pos.xyz/shadow_pos.w) * 0.5 + 0.5;
	shadow_vert.z -= 0.0001;

	int end_pts = 2;
	int sample_pts = (end_pts*2+1)*(end_pts*2+1);
	float shadow_comp = 0.0;
	vec3 sample_point;

	vec2 shadowmap_texel_size = 1.0 / textureSize(shadow_texture, 0);
	float x, y = 0.0;
	for(int i = -end_pts; i <= end_pts; i++)
	{
		x = i * shadowmap_texel_size.x;
		for(int j = -end_pts; j <= end_pts; j++){
			y = j * shadowmap_texel_size.y;
			if(shadow_vert.x + x > 0.0 && shadow_vert.x + x < 1.0 && shadow_vert.y + y > 0.0 && shadow_vert.y + y < 1.0){
				sample_point = vec3(shadow_vert.x+x, shadow_vert.y+y, shadow_vert.z);
				shadow_comp += texture(shadow_texture, sample_point);
			} else{
				sample_pts--;
			}
		}
	}
	shadow_comp /= sample_pts;

	
	//float shadow_comp = texture(shadow_texture, shadow_vert);


	light_diffuse_contribution = shadow_comp * intensity * vec4(light_color * clamp(dot(normal, light), 0.0, 1.0), 1.0);
	light_specular_contribution = shadow_comp * intensity * vec4(light_color * clamp(pow(dot(reflect,view), 100.0f), 0.0, 1.0), 1.0);

}
