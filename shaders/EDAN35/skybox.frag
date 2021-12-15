#version 410

uniform samplerCube skybox_texture;

in VS_OUT {
	vec3 texcoord;
} fs_in;

out vec4 frag_color;

void main()
{
	frag_color = vec4(1, 0, 0, 1);
	//frag_color = texture(skybox_texture, fs_in.texcoord);

}