#version 400

// Interpolated inputs from Vertex Shader
in vec3 world_normal;
in vec3 world_vertex;
in vec3 world_eye;
in vec2 UV;

// Final color of this fragment
out vec4 frag_color;

// Built-in Variables
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

// User-defined Variables
vec3 EmissiveColor;
vec3 DiffuseColor;
vec3 SpecularColor;

vec3 DirLightContribution(int light_num) {
    // Directional Light Vectors
    vec3 N = normalize(world_normal);
    vec3 V = normalize(world_eye - world_vertex);
    vec3 L = -normalize(dir_light_direction[light_num]);
    vec3 H = normalize(V+L);

    // B factor
    float B = 1.0;
    if (dot(N, L) < 0.00001) { B = 0.0; }

    // Contribution
    float diffuseShade = max(dot(N, L), 0.0);
    float shininess = Shininess > 0 ? Shininess : 0.00001;
    float specularShade = B * pow(max(dot(H, N), 0.0), shininess);

    vec3 ambient = DiffuseColor * dir_light_ambient[light_num];
    vec3 diffuse = diffuseShade * DiffuseColor * dir_light_intensity[light_num];
    vec3 specular = specularShade * SpecularColor * dir_light_intensity[light_num];

    return ambient + diffuse + specular;
}

vec3 PointLightContribution(int light_num) {

    // REQUIREMENT: Compute the point light contribution for this light
    vec3 N = normalize(world_normal);
    vec3 L = normalize(point_light_position[light_num] - world_vertex);
    vec3 V = normalize(world_eye - world_vertex);
    vec3 H = normalize(V+L);

    float B = 1.0;
    if (dot(N, L) < 0.00001) { B = 0.0; }

    vec3 ambient = DiffuseColor * point_light_ambient[light_num];
    vec3 diffuse = max(dot(N,L), 0.0) * DiffuseColor * point_light_intensity[light_num];
    float shininess = Shininess > 0 ? Shininess : 0.00001;
    vec3 specular = B * pow(max(dot(N,H),0.0), shininess) * SpecularColor * point_light_intensity[light_num];

    vec3 rVec = point_light_position[light_num] - world_vertex;
    float r = length(rVec);
    float atten = (point_light_atten_quad[light_num]*r*r + point_light_atten_linear[light_num]*r + point_light_atten_const[light_num]);
    return (atten == 0 ? vec3(0, 0, 0) : ambient + (diffuse + specular)/atten);


    // modify the following codes once you have implemented this requirement
}

void main() {
    // Collect contributions from the lights
    EmissiveColor = texture(Emissive, UV).xyz;
    DiffuseColor = texture(Diffuse, UV).xyz;
    SpecularColor = texture(Specular, UV).xyz;
    vec3 dirlight_contribution = vec3(0, 0, 0);
    for (int i = 0; i < 4; i++) dirlight_contribution += DirLightContribution(i);
    vec3 pointlight_contribution = vec3(0, 0, 0);
    for (int i = 0; i < 4; i++) pointlight_contribution += PointLightContribution(i);
    frag_color = vec4(dirlight_contribution + pointlight_contribution + EmissiveColor, 1.0);
}
