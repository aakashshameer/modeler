#version 400

in vec3 Color;
in vec2 Texcoord;

out vec4 frag_color;

uniform sampler2D DiffuseMap;

void main()
{
    // EXTRA CREDIT: Implement your alpha test shader here
	vec4 texture_color = texture(DiffuseMap, Texcoord);
    texture_color.a = 0;
    frag_color = texture_color;
}
