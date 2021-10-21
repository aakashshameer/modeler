#version 400

in vec3 world_normal;
in vec3 world_vertex;
in vec3 world_eye;
in vec2 UV;
out vec4 frag_color;

uniform vec3 point_light_ambient[4];
uniform vec3 point_light_intensity[4];
uniform vec3 point_light_position[4];
uniform float point_light_atten_quad[4];
uniform float point_light_atten_linear[4];
uniform float point_light_atten_const[4];
uniform samplerCube point_light_shadowmap[4];

uniform vec3 dir_light_ambient[4];
uniform vec3 dir_light_intensity[4];
uniform vec3 dir_light_direction[4];

uniform sampler2D Emissive;
uniform sampler2D Diffuse;
uniform sampler2D Specular;

uniform float Shininess;
vec3 EmissiveColor;
vec3 DiffuseColor;
vec3 SpecularColor;

void main()
{
    EmissiveColor = texture(Emissive, UV).xyz;
    DiffuseColor = texture(Diffuse, UV).xyz;
    SpecularColor = texture(Specular, UV).xyz;

    // EXTRA CREDIT: Modify the following codes to implmenet your cartoon shader 
    frag_color = vec4(EmissiveColor, 1.0);
}
