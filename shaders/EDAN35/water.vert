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

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec3 texcoord;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform float ellapsed_time_s;

out VS_OUT {
	vec3 vertex;
	vec3 normal;
	vec3 binormal;
	vec3 tangent;
	vec2 texcoord;
} vs_out;

float A1 = 1*1.0;
float A2 = 1*0.5;
vec2 D1 = vec2(-1.0, 0.0);
vec2 D2 = vec2(-0.7, 0.7);
float f1 = 0.2;
float f2 = 0.37;
float p1 = 1.5;
float p2 = 2.3;
float k1 = 2.0;
float k2 = 3.0;

void displace(in float A, in vec2 D, in float f, in float t, in float p,
				in float k, in vec3 vertex, out float G, out float Gdx, out float Gdz)
{
	float alpha = (sin((D.x * vertex.x + D.y * vertex.z) * f + t * p) + 1) * 0.5;
	G = A * pow(alpha, k);
	Gdx = 0.5 * k * f * A * pow(alpha, k-1) * cos((D.x * vertex.x + D.y * vertex.z) * f + t * p) * D.x;
	Gdz = 0.5 * k * f * A * pow(alpha, k-1) * cos((D.x * vertex.x + D.y * vertex.z) * f + t * p) * D.y;
}


void main()
{
	float y1;
	float y2;
	float Gdx1;
	float Gdx2;
	float Gdz1;
	float Gdz2;
	float Gdx;
	float Gdz;
	displace(A1, D1, f1, ellapsed_time_s, p1, k1, vertex, y1, Gdx1, Gdz1);
	displace(A2, D2, f2, ellapsed_time_s, p2, k2, vertex, y2, Gdx2, Gdz2);
	Gdx = Gdx1 + Gdx2;
	Gdz = Gdz1 + Gdz2;
	vs_out.vertex = vec3(vertex_model_to_world * vec4(vertex.x, y1 + y2, vertex.z, 1.0));
	vs_out.normal = normalize(vec3(normal_model_to_world * vec4(-Gdx, 1, -Gdz, 0)));
	vs_out.binormal = normalize(vec3(normal_model_to_world * vec4(0.0, Gdz, 1.0, 0)));
	vs_out.tangent = normalize(vec3(normal_model_to_world * vec4(1.0, Gdx, 0.0, 0)));
	vs_out.texcoord = texcoord.xy;

	gl_Position = camera.view_projection * vec4(vs_out.vertex, 1.0);
}



