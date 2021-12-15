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

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 binormal;

out VS_OUT {
	vec3 normal;
	vec2 texcoord;
	vec3 tangent;
	vec3 binormal;
	vec2 plane_loc;
} vs_out;


void main() {
	vs_out.normal = normalize(vec3(normal_model_to_world * vec4(normal, 0.0)));
	vs_out.texcoord = vec2(texcoord.x, texcoord.y);
    vs_out.tangent = normalize(vec3(normal_model_to_world * vec4(tangent, 0.0)));
    vs_out.binormal = normalize(vec3(normal_model_to_world * vec4(binormal, 0.0)));
	vs_out.plane_loc = vertex.xz;

	gl_Position = camera.view_projection * vertex_model_to_world * vec4(vertex, 1.0);
}
